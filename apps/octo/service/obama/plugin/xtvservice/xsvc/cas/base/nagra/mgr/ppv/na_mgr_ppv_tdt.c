/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Include_________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

#include "na_mgr_cak_ext.h"
#include <na_mgr_cak_ppv.h>
#include "util.h"
#include <linkedlist.h>

#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
/**************************************************************************************************/
#define ________CA_CAK_PPV_Golbal_Value___________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Define__________________________________________________
/**************************************************************************************************/





#define VK_CreateMessageQueue(a, b, c, d)				VK_MSG_Create(a, d, c, b, VK_SUSPENDTYPE_PRIORITY)
#define VK_PostMessage(a, b, c) 						VK_MSG_Send(a, b, c)
#define VK_GetMessage(a ,b, c)						VK_MSG_Receive(a, b, c)
#define VK_WaitGetMessage(a, b, c, d)					VK_MSG_ReceiveTimeout(a, b, d, c)
#define VK_PostMessageTimeout(a, b, c)				VK_MSG_SendTimeout(a, b, c, 5000)


#define TIMEOUT_DURATION_NIT						(1000*10)
#define TIMER_COUNTER_PACKAGELIST					(60)
#define TIMER_COUNTER_PURCHASEHISTORY			(60)
#define TIMEOUT_DURATION_PACKAGELIST				(1000*1)
#define TIMEOUT_DURATION_PURCHASEHISTORY		(1000*1)
#define TIMEOUT_DURATION_RESTART					(1500*1)


/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Type_Define____________________________________________
/**************************************************************************************************/
typedef enum
{
	eNA_PPV_SignalingState_None						= 0x00000000,

	eNA_PPV_SignalingState_PmtWating					= 0x00000001,
	eNA_PPV_SignalingState_CatWating					= 0x00000002,
	eNA_PPV_SignalingState_NitWating					= 0x00000004,
	eNA_PPV_SignalingState_EitWating					= 0x00000008,
	eNA_PPV_SignalingState_SdtWating					= 0x00000010,

	eNA_PPV_SignalingState_PmtReceived				= 0x00000100,
	eNA_PPV_SignalingState_CatReceived					= 0x00000200,
	eNA_PPV_SignalingState_NitReceived					= 0x00000400,
	eNA_PPV_SignalingState_EitReceived					= 0x00000800,
	eNA_PPV_SignalingState_SdtReceived					= 0x00001000,

	eNA_PPV_SignalingState_PmtTimeout					= 0x00010000,
	eNA_PPV_SignalingState_CatTimeout					= 0x00020000,
	eNA_PPV_SignalingState_NitTimeout					= 0x00040000,
	eNA_PPV_SignalingState_EitTimeout					= 0x00080000,
	eNA_PPV_SignalingState_SdtTimeout					= 0x00100000,

	eNA_PPV_SignalingState_RequestPurchaseHistory		= 0x00200000,
	eNA_PPV_SignalingState_RequestPackageList			= 0x00400000,

	eNA_PPV_SignalingState_PurchaseHistoryTimeout		= 0x01000000,
	eNA_PPV_SignalingState_PackageListTimeout			= 0x02000000,

	eNA_PPV_SignalingState_Fail							= 0x80000000,

	eNA_PPV_SignalingState_Max

}NA_PPV_SignalingState_t;

typedef enum
{
	eNA_PPV_PdtSet_None								= 0x00000000,

	eNA_PPV_PdtSet_CPDTPid							= 0x00001000,
	eNA_PPV_PdtSet_HPDTPid							= 0x00002000,

	eNA_PPV_PdtSet_Max
}NA_PPV_PdtSet_t;


typedef enum
{
	eNA_PPV_Restart_None								= 0x00000000,

	eNA_PPV_Restart_Valid,
	eNA_PPV_Restart_Invalid,

	eNA_PPV_Restart_Max
}NA_PPV_Restart_t;


typedef enum
{
	eNA_PPV_LoadState_None							= 0x00000000,

	eNA_PPV_LoadState_ProductLoading					= 0x00000001,
	eNA_PPV_LoadState_ProductLoaded					= 0x00000002,

	eNA_PPV_LoadState_Max
}NA_PPV_LoadState_t;


typedef enum
{
	eNA_PPV_BuildState_None							= 0x00000000,

	eNA_PPV_BuildState_PurchaseHistoryComplete		= 0x00000001,
	eNA_PPV_BuildState_PackageListComplete				= 0x00000002,
	eNA_PPV_BuildState_CreditComplete					= 0x00000004,

	eNA_PPV_BuildState_Max
}NA_PPV_BuildState_t;


typedef enum
{
	eNA_PPV_Period_None								 = 0,
	eNA_PPV_Period_Tdt,
	eNA_PPV_Period_Others,

	eNA_PPV_Period_Max
}NA_PPV_Period_t;


typedef struct tagNaPDTInfo
{
	HUINT16		usTsId;
	HUINT16		usSvcId;
	HUINT16		usOnId;
	HUINT16		usPid;
} NA_PDT_Info_t;



typedef struct tagNaCreditInfo
{
	HUINT32		ulId;
	HUINT32		ulProviderId;
	HUINT32		ulAmount;

} NA_CREDIT_Info_t;


typedef struct tagNaCreditList
{
	HUINT32				ulNumOfCredit;
	NA_CREDIT_Info_t		*pCreditList;
} NA_CREDIT_List_t;


typedef struct tagNaPPVStateInfo
{
/** state info */
	NA_PPV_Period_t				ePeriodType;

	NA_PPV_SignalingState_t		eSignalingState;
	NA_PPV_LoadState_t			eLoadState;
	NA_PPV_BuildState_t			eBuildState;


/** PDT signaling info */
	NA_PPV_PdtSet_t				ePdtSetMask;
	NA_PDT_Info_t				stCPdtInfo;
	NA_PDT_Info_t				stHPdtInfo;

/** Nagra cak request info */
	TCaRequest					*pPurchaseHistoryRequest;
	TCaRequest					*pPackageListRequest;

/** timer info */
	HUINT32						ulRestartTimer;
	NA_PPV_Restart_t				eRestartStatus;

	HUINT32						ulNitWaitTimer;

	HUINT32						ulPurchaseHistoryCounter;
	HUINT32						ulPurchaseHistoryTimer;

	HUINT32						ulPackageListCounter;
	HUINT32						ulPackageListTimer;


/** TS info */
	HUINT16						usTsId;
	HUINT16						usOnId;
	HBOOL						bNagraTs;
	HBOOL						bNagraCaSysId;

/** section list */
	POINTER_LIST_T				*pPmtList;
	POINTER_LIST_T				*pCatList;
	POINTER_LIST_T				*pNitList;
	POINTER_LIST_T				*pEitList;
	POINTER_LIST_T				*pSdtList;
	POINTER_LIST_T				*pTdtList;

}NA_PPV_StateInfo_t;




/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Static_Variable____________________________________________
/**************************************************************************************************/


static NA_PPV_StateInfo_t			s_stTdtPpvStateInfo = {0, };

/*****************************************************************************
  *
  * data
  *
  *****************************************************************************/
static PPV_PACKAGE_LIST			s_stTdtPackageList;
static PPV_PURCHASE_LIST			s_stTdtPurchaseHistory;
static NA_CREDIT_List_t				s_stTdtCreditList;

/*****************************************************************************
  *
  * task
  *
  *****************************************************************************/
static TUnsignedInt32 				s_ulPpvDataSemId = 0;

static TUnsignedInt32 				s_ulPpvTaskId;
static TUnsignedInt32 				s_ulTdtPpvQueueId;


/*****************************************************************************
  * semaphore for s_stTdtPpvStateInfo, s_stTdtPackageList, s_stTdtPurchaseHistory, credit info
  *****************************************************************************/
#define	ENTER_CRITICAL_PPVTDT								\
{															\
	VK_SEM_Get(s_ulPpvDataSemId);							\
}

#define	LEAVE_CRITICAL_PPVTDT								\
{															\
	VK_SEM_Release(s_ulPpvDataSemId);						\
}


/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Static_Prototype_________________________________________
/**************************************************************************************************/

static void						local_cakppv_tdt_Task(void* p);
static NA_PPV_StateInfo_t*			local_cakppv_tdt_getPpvStateInfo(void);
static PPV_PACKAGE_LIST*			local_cakppv_tdt_getPackageList(void);
static PPV_PURCHASE_LIST*			local_cakppv_tdt_getPurchaseHistory(void);
static NA_CREDIT_List_t*			local_cakppv_tdt_getCreditList(void);

static HERROR						local_cakppv_tdt_restartProcess(void);
static void 						local_cakppv_tdt_waitRestartTimerCallback(unsigned long nTimerId,  void *pArg);
static HERROR						local_cakppv_tdt_processProductLoad(HUINT16 ulMessage);
static void						local_cakppv_tdt_waitNitTimerCallback(unsigned long TimerId,  void *pArg);
static HERROR						local_cakppv_tdt_setNitTimer(void);
static HERROR						local_cakppv_tdt_processNitTimeout(HUINT32 ulTimerId);
static HERROR						local_cakppv_tdt_processNit(HUINT8 *pucSection, HUINT16 usOnId);
static HERROR						local_cakppv_tdt_parseNit(HUINT8 *pucNit, NA_PPV_PdtSet_t *pePdtSetMask, NA_PDT_Info_t *pCPdt, NA_PDT_Info_t *pHPdt);
static HERROR						local_cakppv_tdt_retrievePdt(void);
static HERROR						local_cakppv_tdt_initPpvInfo(void);
static HERROR						local_cakppv_tdt_processCat(HBOOL *pIsTdtSystemIds);
static HERROR						local_cakppv_tdt_requestPdtStart(NA_PDT_Info_t *pCPdtInfo, NA_PDT_Info_t *pHPdtInfo);
static HERROR						local_cakppv_tdt_requestPdtStop(void);
static void 						local_cakppv_tdt_waitPackageListTimerCallback(unsigned long nTimerId,  void *pArg);
static HERROR						local_cakppv_tdt_checkPackageListTimer(HUINT32 ulTimerId);
static void						local_cakppv_tdt_packageListCallback(TCaRequest* pxRequest);
static HBOOL						local_cakppv_tdt_IsAvailableBusinessType(TPpid providerId, TUnsignedInt8 ucBusinessType);
static HERROR						local_cakppv_tdt_requestPackageList(HBOOL bFirst, HUINT32 ulProviderId);
static void						local_cakppv_tdt_InitPackageInfo(PACKAGE_INFO *pPackageInfo);
static HERROR						local_cakppv_tdt_initPackageList(PPV_PACKAGE_LIST* pPackageList);
static HERROR						local_cakppv_tdt_buildPackageList(TCaRequest* pxRequest);
static void						local_cakppv_tdt_waitPurchaseHistoryTimerCallback(unsigned long TimerId,  void *pArg);
static HERROR						local_cakppv_tdt_checkPurchaseHistoryTimer(HUINT32 ulTimerId);
static HERROR						local_cakppv_tdt_clearPurchaseHistory(PPV_PURCHASE_LIST* pPurchaseManager);
static HERROR 					local_cakppv_tdt_requestPurchaseHistory(HBOOL bFirst, TUnsignedInt32 ulProviderId);
static void						local_cakppv_tdt_purchaseHistoryCallback(TCaRequest* pxRequest);
static HERROR						local_cakppv_tdt_buildPurchaseHistory(TCaRequest*  pxRequest);
static void						local_cakppv_tdt_freeSection(void *pvContents);

static HERROR						local_cakppv_tdt_getPPVStateInfo(NA_PPV_State_t *peState, HBOOL *pbNagraCaSys);
static HERROR						local_cakppv_tdt_getProductList(HUINT32 *pulNumOfProducts, PPV_PACKAGE_LIST_ITEM **ppPackageList);
static HERROR						local_cakppv_tdt_getPurchaseHistoryList(HUINT32 *pulNumOfPurchase, PPV_PURCHASE_LIST_ITEM **ppPurchaseList);
static HERROR						local_cakppv_tdt_releaseProductList(PPV_PACKAGE_LIST_ITEM *pPackageList);
static HERROR						local_cakppv_tdt_releasePurchaseHistoryList(PPV_PURCHASE_LIST_ITEM *pPurchaseList);

#if defined(CONFIG_CREDIT_TDT)
static void						local_cakppv_tdt_InitCreditInfo(NA_CREDIT_List_t *pCreditInfo);
static HERROR						local_cakppv_tdt_requestCredit(void);
static void						local_cakppv_tdt_creditCallback(TCaRequest* pxRequest);
static HERROR						local_cakppv_tdt_buildCreditList(TCaRequest*  pxRequest);
#endif





#define ____TDT_PPV_TASK____

static void local_cakppv_tdt_Task(void* p)
{
	NaCasMsg_t		 	stInMsg;
	HERROR				hError = ERR_OK;
	TSystemInfoStatus 	eSmartcardStatus;

	while(1)
	{
		if (VK_GetMessage(s_ulTdtPpvQueueId, (void *)&stInMsg, sizeof(NaCasMsg_t)) != ERR_OK)
		{
			continue;
		}

		switch (stInMsg.wMsg)
		{
			case NA_CAK_ID:
				switch (stInMsg.lParam2)
				{
/******************************************************************************************************
 *
 * credit message
 *
 *****************************************************************************************************/
					case CAK_CREDIT_NOTIFICATION:
						HxLOG_Info("CAK_CREDIT_NOTIFICATION !!\n");
#if						defined(CONFIG_CREDIT_TDT)
						hError = CA_NA_CakGetSystemInfo(&eSmartcardStatus, NULL, NULL, NULL);
						if(hError != ERR_OK || eSmartcardStatus != CA_SYSTEM_INFO_SUCCESS)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]CA_NA_CakGetSystemInfo fail !!\n");
							break;
						}
						hError = local_cakppv_tdt_requestCredit();
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_requestCredit fail !!\n");
							break;
						}
#endif
						break;

					case CAK_CREDIT_CALL_BACK:
						HxLOG_Info("CAK_CREDIT_CALL_BACK !!\n");
						break;

					case CAK_PPV_NIT_TIMER_CALL_BACK:
						HxLOG_Info("CAK_PPV_NIT_TIMER_CALL_BACK !!\n");
						hError = local_cakppv_tdt_processNitTimeout((HUINT32)stInMsg.hWnd);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_processNitTimeout fail !!\n");
							break;
						}
						break;

/******************************************************************************************************
 *
 * system info message
 *
 *****************************************************************************************************/
					case CAK_ALARM_ACCESS :
						HxLOG_Info("CAK_ALARM_ACCESS !!\n");
#if						1
						hError = local_cakppv_tdt_restartProcess();
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_restartProcess fail !!\n");
							break;
						}
						hError = local_cakppv_tdt_retrievePdt();
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_restartProcess fail !!\n");
							break;
						}
						hError = local_cakppv_tdt_msgPurchaseHistory(NA_TDT_PROVIDER_ID);
						if(hError != ERR_OK)
						{
							HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_msgPurchaseHistory fail!!\n");
						}
						hError = local_cakppv_tdt_msgPackageList(NA_TDT_PROVIDER_ID);
						if(hError != ERR_OK)
						{
							HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_msgPackageList fail!!\n");
						}
#endif
						break;

/******************************************************************************************************
 *
 * products loading message
 *
 *****************************************************************************************************/
					case CAK_PPV_LOADING_LISTENER:
						HxLOG_Info("CAK_PPV_LOADING_LISTENER !!\n");
						hError = local_cakppv_tdt_processProductLoad((HUINT16)stInMsg.lParam2);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_requestPackageList fail !!\n");
							break;
						}
						break;

					case CAK_PPV_LOADED_LISTENER:
						HxLOG_Info("CAK_PPV_LOADED_LISTENER !!\n");
						hError = local_cakppv_tdt_processProductLoad((HUINT16)stInMsg.lParam2);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_requestPackageList fail !!\n");
							break;
						}
						break;

					case CAK_PPV_PURCHASEHISTORY_LISTENER :
						HxLOG_Info("CAK_PPV_PURCHASEHISTORY_LISTENER !!\n");
						break;


/******************************************************************************************************
 *
 * products list message
 *
 *****************************************************************************************************/
					case CAK_REQUEST_PACKAGE_LIST:
						HxLOG_Info("CAK_REQUEST_PACKAGE_LIST !!\n");
						hError = local_cakppv_tdt_requestPackageList(TRUE, (HUINT32)stInMsg.hWnd);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_requestPackageList fail !!\n");
							break;
						}
						break;

					case CAK_PPV_PACKAGE_TIMER_CALL_BACK:
						HxLOG_Info("CAK_PPV_PACKAGE_TIMER_CALL_BACK !!\n");
						hError = local_cakppv_tdt_checkPackageListTimer((HUINT32)stInMsg.hWnd);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_checkPackageListTimer fail !!\n");
							break;
						}
						hError = local_cakppv_tdt_requestPackageList(FALSE, 0);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_requestPackageList fail !!\n");
							break;
						}
						break;

					case CAK_PPV_PACKAGE_LIST_CALL_BACK:
						HxLOG_Info("CAK_PPV_PACKAGE_LIST_CALL_BACK !!\n");
						hError = local_cakppv_tdt_buildPackageList((TCaRequest*)stInMsg.hWnd);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_buildPackageList fail !!\n");
							break;
						}
						break;


/******************************************************************************************************
 *
 * purchaseHistory message
 *
 *****************************************************************************************************/
					case CAK_REQUEST_PURCHASE_HISTORY:
						HxLOG_Info("CAK_REQUEST_PURCHASE_HISTORY - ulProviderId(0x%x)\t\r\n", stInMsg.hWnd);
						hError = local_cakppv_tdt_requestPurchaseHistory(TRUE, (HUINT32)stInMsg.hWnd);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_requestPurchaseHistory fail !!\n");
							break;
						}
						break;

					case CAK_PPV_PURCHASEHISTORY_TIMER_CALL_BACK:
						HxLOG_Info("CAK_PPV_PURCHASEHISTORY_TIMER_CALL_BACK !!\n");
						hError = local_cakppv_tdt_checkPurchaseHistoryTimer((HUINT32)stInMsg.hWnd);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_checkPurchaseHistoryTimer fail !!\n");
							break;
						}
						hError = local_cakppv_tdt_requestPurchaseHistory(FALSE, 0);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_requestPurchaseHistory fail !!\n");
							break;
						}
						break;

					case CAK_PURCHASE_HISTORY_CALL_BACK:
						HxLOG_Print("CAK_PURCHASE_HISTORY_CALL_BACK - pRequest(%ld)\t\r\n", stInMsg.hWnd);
						hError = local_cakppv_tdt_buildPurchaseHistory((TCaRequest*)stInMsg.hWnd);
						if(hError != ERR_OK)
						{
							HxLOG_Info("[local_cakppv_tdt_Task]local_cakppv_tdt_buildPurchaseHistory fail !!\n");
							break;
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
						HxLOG_Info("CAK_TS_CHECK_REQ_ID !!\n");
						break;

					case CAK_EVT_INFO_REQ_ID:
						HxLOG_Info("CAK_EVT_INFO_REQ_ID !!\n");
						break;

					default:
						break;
				}
				break;

			case NA_SERVICE_ID:
				switch (stInMsg.lParam2)
				{
					case CAK_SERVICE_CHANGE_TP:
						HxLOG_Info("CAK_SERVICE_CHANGE_TP !!\n");
						hError = local_cakppv_tdt_initPpvInfo();
						if(hError != ERR_OK)
						{
							HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_initPpvInfo fail!!\n");
							break;
						}
						hError = local_cakppv_tdt_setNitTimer();
						if(hError != ERR_OK)
						{
							HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_setNitTimer fail!!\n");
						}
						break;

					case CAK_SERVICE_CHANGE_CHANNEL:
						break;

					default:
						break;
				}
				break;

			case NA_FILTER_ID:
				switch ((TSignedInt32)stInMsg.lParam2)
				{
					case FILTER_PSI_CAT_ARRIVAL:
						HxLOG_Info("[FILTER_PSI_CAT_ARRIVAL]\n");

					case CAK_FILTER_NIT_ARRIVAL:
						HxLOG_Info("[CAK_FILTER_NIT_ARRIVAL]\n");
#if						1
						if(hError != ERR_OK)
						{
							HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_initPpvInfo fail!!\n");
						}
						if(stInMsg.lParam2 == FILTER_PSI_CAT_ARRIVAL)
						{
							HBOOL	bNagraSysId = FALSE;

							hError = local_cakppv_tdt_processCat(&bNagraSysId);
							if(hError != ERR_OK || bNagraSysId == FALSE)
							{
								HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_processCat fail!!\n");
								break;
							}
						}
						if(stInMsg.lParam2 == CAK_FILTER_NIT_ARRIVAL)
						{
							hError = local_cakppv_tdt_processNit((HUINT8*)stInMsg.hWnd, (HUINT16)stInMsg.lParam3);
							if(hError != ERR_OK)
							{
								HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_processNit fail!!\n");
								break;
							}
						}

						hError = local_cakppv_tdt_retrievePdt();
						if(hError != ERR_OK)
						{
							HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_retrievePdt fail!!\n");
							break;
						}

						hError = local_cakppv_tdt_msgPurchaseHistory(NA_TDT_PROVIDER_ID);
						if(hError != ERR_OK)
						{
							HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_msgPurchaseHistory fail!!\n");
						}
						hError = local_cakppv_tdt_msgPackageList(NA_TDT_PROVIDER_ID);
						if(hError != ERR_OK)
						{
							HxLOG_Error("[local_cakppv_tdt_Task] local_cakppv_tdt_msgPackageList fail!!\n");
						}
#endif
						break;

					case CAK_FILTER_EIT_PF_ARRIVAL:
						HxLOG_Info("[CAK_FILTER_EIT_PF_ARRIVAL]\n");
						break;

					case CAK_FILTER_TDT_ARRIVAL:
						HxLOG_Info("[CAK_FILTER_TDT_ARRIVAL]\n");
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












#define ____UTIL_FUNC____

static  NA_PPV_StateInfo_t*			local_cakppv_tdt_getPpvStateInfo(void)
{
	return &s_stTdtPpvStateInfo;
}

static PPV_PACKAGE_LIST*			local_cakppv_tdt_getPackageList(void)
{
	return &s_stTdtPackageList;
}

static PPV_PURCHASE_LIST*			local_cakppv_tdt_getPurchaseHistory(void)
{
	return &s_stTdtPurchaseHistory;
}

static NA_CREDIT_List_t*			local_cakppv_tdt_getCreditList(void)
{
	return &s_stTdtCreditList;
}




HERROR				local_cakppv_tdt_msgPurchaseHistory(TUnsignedInt32 ulProviderId)
{
	NaCasMsg_t 		stMsg = {0, };
	TSignedInt32		nErrCode = 0;

	HxLOG_Info("[local_cakppv_tdt_msgPurchaseHistory] ENTER!!\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.hWnd = ulProviderId;
	stMsg.lParam2 = CAK_REQUEST_PURCHASE_HISTORY;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_msgPurchaseHistory] VK_PostMessage fail!!\n");
		return CA_ERROR;
	}
	return CA_NO_ERROR;
}


HERROR				local_cakppv_tdt_msgPackageList(TUnsignedInt32 ulProviderId)
{
	NaCasMsg_t 		stMsg = {0, };
	TSignedInt32		nErrCode = 0;

	stMsg.wMsg = NA_CAK_ID;
	stMsg.hWnd = ulProviderId;
	stMsg.lParam2 = CAK_REQUEST_PACKAGE_LIST;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_msgPurchaseHistory] VK_PostMessage fail!!\n");
		return CA_ERROR;
	}
	return CA_NO_ERROR;
}


#define ____SYSTEM_CHECK_FUNC____

static void 					local_cakppv_tdt_waitRestartTimerCallback(unsigned long nTimerId,  void *pArg)
{
	TSignedInt32				nErrCode;
	NaCasMsg_t 	 			stMsg;

	HxLOG_Info("[local_cakppv_tdt_waitRestartTimerCallback] ENTER \n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_ALARM_ACCESS;
	stMsg.hWnd = (TUnsignedInt32)nTimerId;

	nErrCode = VK_PostMessageTimeout(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if (nErrCode)
	{
		HxLOG_Error("[local_cakppv_tdt_waitRestartTimerCallback] VK_PostMessageTimeout fail!! \n");
		goto EXIT_FUNC;
	}

EXIT_FUNC :

	HxLOG_Info("[local_cakppv_tdt_waitRestartTimerCallback] EXIT \n");
	return;
}



static HERROR				local_cakppv_tdt_restartProcess(void)
{
	HERROR 				hError = ERR_OK;
	NA_PPV_StateInfo_t	*pTdtPpvStateInfo = NULL;
	HUINT8				*pucNitSection = NULL;
	PPV_PACKAGE_LIST	*pPackageList = NULL;
	PPV_PURCHASE_LIST	*pPurchaseHistory = NULL;
	HUINT16				*pusCaSysIds = NULL;
	HUINT32				ulSize = 0;
	HUINT32				ulIndex = 0;
	HBOOL				bNagraSystemId = FALSE;

	HxLOG_Info("[local_cakppv_tdt_restartProcess] ENTER \n");

	ENTER_CRITICAL_PPVTDT;

/******************************************************************************************************
 *
 * data init
 *
 *****************************************************************************************************/
	pPackageList = local_cakppv_tdt_getPackageList();
	pPurchaseHistory = local_cakppv_tdt_getPurchaseHistory();
	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();

#if 1
{
	NA_PPV_Restart_t		eRestartStatus = eNA_PPV_Restart_None;
	TPpid				providerId = 0;
	HINT32				nResult = 0;

	eRestartStatus = eNA_PPV_Restart_Valid;
	hError = CA_NA_CakGetProviderId(0, &providerId);
	if (hError != ERR_OK)
	{
		HxLOG_Info("[local_cakppv_tdt_restartProcess] CA_NA_CakGetProviderId fail!! \n");
		eRestartStatus = eNA_PPV_Restart_Invalid;
	}

	switch(eRestartStatus)
	{
		/** card insert 의 경우 한번더 검사를 해서 반복된 카드 삽입에 대처한다, 반복된 카드 삽입에 의해 CAK에 load 가 걸린다 */
		case eNA_PPV_Restart_Valid :
			if(pTdtPpvStateInfo->eRestartStatus == eNA_PPV_Restart_Valid )
			{
				HxLOG_Info("[local_cakppv_tdt_restartProcess] restart process!! \n");
			}
			else
			{
				HxLOG_Info("[local_cakppv_tdt_restartProcess] VK_TIMER_EventAfter!! \n");
				nResult = VK_TIMER_EventAfter(	TIMEOUT_DURATION_RESTART,
												local_cakppv_tdt_waitRestartTimerCallback,
												NULL,
												0,
												&pTdtPpvStateInfo->ulRestartTimer);
				if(nResult != VK_OK)
				{
					HxLOG_Error("[local_cakppv_tdt_restartProcess] VK_TIMER_EventAfter fail!! \n");
					hError =ERR_FAIL;
					goto EXIT_FUNC;
				}
				pTdtPpvStateInfo->eRestartStatus = eNA_PPV_Restart_Valid;
				hError =ERR_FAIL;
				goto EXIT_FUNC;
			}
			break;

		case eNA_PPV_Restart_Invalid :
			HxLOG_Info("[local_cakppv_tdt_restartProcess] eNA_PPV_Restart_Invalid!! \n");
			hError =ERR_FAIL;
			goto EXIT_FUNC;

		default :
			HxLOG_Info("[local_cakppv_tdt_restartProcess] default!! \n");
			break;
	}
	pTdtPpvStateInfo->eRestartStatus = eNA_PPV_Restart_None;
}
#endif

	hError = local_cakppv_tdt_clearPurchaseHistory(pPurchaseHistory);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_restartProcess] local_cakppv_tdt_clearPurchaseHistory fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	hError = local_cakppv_tdt_initPackageList(pPackageList);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_restartProcess] local_cakppv_tdt_initPackageList fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

/******************************************************************************************************
 *
 * cat ca system id check
 *
 *****************************************************************************************************/
	pTdtPpvStateInfo->bNagraCaSysId = FALSE;
	CA_NA_PsiGetCatCaSystemIds(&pusCaSysIds, &ulSize);
	if(pusCaSysIds == NULL || ulSize == 0)
	{
		HxLOG_Error("[local_cakppv_tdt_processCat] pusCaSysIds == NULL || ulSize == 0!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	pTdtPpvStateInfo->eSignalingState |= eNA_PPV_SignalingState_CatReceived;
	for(ulIndex = 0; ulIndex < ulSize; ulIndex++)
	{
		if(pusCaSysIds[ulIndex] >= 0x1812 && pusCaSysIds[ulIndex] <= 0x1819)
		{
			bNagraSystemId = TRUE;
			break;
		}
	}
	if(bNagraSystemId)
	{
		pTdtPpvStateInfo->bNagraCaSysId = TRUE;
	}

/******************************************************************************************************
 *
 * nit check
 *
 *****************************************************************************************************/
	if(pTdtPpvStateInfo->pNitList)
	{
		pucNitSection = UTIL_LINKEDLIST_GetContents(pTdtPpvStateInfo->pNitList);
	}
	if(pucNitSection == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_restartProcess] local_cakppv_tdt_parseNit fail!! \n");
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = local_cakppv_tdt_parseNit(	pucNitSection,
										&pTdtPpvStateInfo->ePdtSetMask,
										&pTdtPpvStateInfo->stCPdtInfo,
										&pTdtPpvStateInfo->stHPdtInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_restartProcess] local_cakppv_tdt_parseNit fail!! \n");
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}
	pTdtPpvStateInfo->eSignalingState |= eNA_PPV_SignalingState_NitReceived;

	if(	pTdtPpvStateInfo->ePdtSetMask & eNA_PPV_PdtSet_HPDTPid
		|| pTdtPpvStateInfo->ePdtSetMask & eNA_PPV_PdtSet_CPDTPid
	)
	{
		pTdtPpvStateInfo->bNagraTs = TRUE;
	}

	if(!pTdtPpvStateInfo->bNagraTs)
	{
		pTdtPpvStateInfo->ePeriodType = eNA_PPV_Period_Others;
		HxLOG_Error("[local_cakppv_tdt_restartProcess] (!bNagraTs) !! \n");
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}
	hError = ERR_OK;

EXIT_FUNC :

	LEAVE_CRITICAL_PPVTDT;
	HxLOG_Info("[local_cakppv_tdt_processNit] EXIT \n");

	return hError;
}

#define ____CREDIT_FUNC____
#if						defined(CONFIG_CREDIT_TDT)
static void		local_cakppv_tdt_InitCreditInfo(NA_CREDIT_List_t *pCreditInfo)
{
	if(pCreditInfo == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_InitCreditInfo] pCreditInfo == NULL fail!! \n");
		goto EXIT_FUNC;
	}
	if(pCreditInfo->pCreditList)
	{
		OxCAS_Free(pCreditInfo->pCreditList);
		pCreditInfo->pCreditList = NULL;
	}

	HxSTD_memset(pCreditInfo, 0x00, sizeof(NA_CREDIT_List_t));

EXIT_FUNC :

	return;
}

static HERROR local_cakppv_tdt_requestCredit(void)
{
	HERROR				hError = ERR_FAIL;
	TCaRequestStatus		status;
	TCaRequest			*pRequest = NULL;
	HINT32				nResult = 0;
	NA_CREDIT_List_t		*pCreditInfo = NULL;


	HxLOG_Info("[local_cakppv_tdt_requestCredit] ENTER!!\n");
	ENTER_CRITICAL_PPVTDT;

	pCreditInfo = local_cakppv_tdt_getCreditList();

	local_cakppv_tdt_InitCreditInfo(pCreditInfo);

	status = caRequestCreate(CA_REQUEST_TYPE_CREDITS, &pRequest);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestCredit] caRequestCreate fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	status = caRequestSetAsynchronousResponse(pRequest, local_cakppv_tdt_creditCallback);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestCredit] caRequestSetAsynchronousResponse fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	status = caRequestSend(pRequest);
	if (status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestCredit] caRequestSend fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}


	hError = ERR_OK;

EXIT_FUNC :

	if(hError != ERR_OK)
	{
		if(pRequest)
		{
			status = caRequestDispose(pRequest);
			if(status != CA_REQUEST_NO_ERROR)
			{
				HxLOG_Error("[local_cakppv_tdt_requestCredit] caRequestDispose fail!!\n");
				hError = ERR_FAIL;
			}
			pRequest = NULL;
		}
	}
	LEAVE_CRITICAL_PPVTDT;

	HxLOG_Info("[local_cakppv_tdt_requestCredit] EXIT!!\n");

	return hError;
}



static void local_cakppv_tdt_creditCallback(TCaRequest* pxRequest)
{
	TSignedInt32		nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Info("[local_cakppv_tdt_creditCallback] ENTER\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_CREDIT_CALL_BACK;
	stMsg.hWnd = (TUnsignedInt32)pxRequest;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if(nErrCode)
	{
		HxLOG_Error("[local_cakppv_tdt_creditCallback]  VK_PostMessage fail\n");
	}

	HxLOG_Info("[local_cakppv_tdt_creditCallback] EXIT\n");
}



static HERROR					local_cakppv_tdt_buildCreditList(TCaRequest*  pxRequest)
{
	HERROR					hError = ERR_FAIL;
	TCaRequest				*pRequest = NULL;
	TCaRequestStatus 			reqStatus;
	TCaObjectStatus 			objStatus;
	TCaProcessingStatus 		procStatus;
	TUnsignedInt32 			ulIndex = 0;
	TUnsignedInt32 			ulNumOfCredit = 0;
	NA_PPV_StateInfo_t		*pTdtPpvStateInfo = NULL;
	NA_CREDIT_List_t			*pCreditMgr = NULL;
	TCaCredit				**ppCakCreditList = NULL;
	TPpid					creditId = 0;
	TPpid					providerId = 0;
	TCreditAmount			xAmount = 0;
	TCreditFlags				xFlags = 0;


	HxLOG_Info("[local_cakppv_tdt_buildCreditList] Enter (%x)\n", pxRequest);
	ENTER_CRITICAL_PPVTDT;

	pCreditMgr = local_cakppv_tdt_getCreditList();
	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();



	HxLOG_Info("[local_cakppv_tdt_buildCreditList] pRequest(%x)\n", pRequest);
	reqStatus = caRequestGetProcessingStatus(pRequest, &procStatus);
	if (reqStatus != CA_REQUEST_NO_ERROR || procStatus != CA_PROCESSING_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_buildCreditList] caRequestGetProcessingStatus fail!!\n");
		HxLOG_Error("[local_cakppv_tdt_buildCreditList] reqStatus(%x)procStatus(%x)\n", reqStatus, procStatus);
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	HxLOG_Error("[local_cakppv_tdt_buildCreditList] procStatus(%x)\n", procStatus);

	reqStatus = caRequestGetObjects(pRequest, &ulNumOfCredit, &ppCakCreditList);
	if (reqStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_buildCreditList] caRequestGetObjects fail(%d)!!\n", reqStatus);
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	HxLOG_Info("ulNumOfCredit = %d, reqStatus(%d)\n", ulNumOfCredit, reqStatus);

	if(ulNumOfCredit == 0)
	{
		hError = ERR_OK;
	}
	else if(ulNumOfCredit > 0)
	{
		pCreditMgr->pCreditList = (NA_CREDIT_Info_t *)OxCAS_Malloc(ulNumOfCredit*sizeof(NA_CREDIT_Info_t));
		if(pCreditMgr->pCreditList == NULL)
		{
			HxLOG_Error("[local_cakppv_tdt_buildCreditList] OxCAS_Malloc fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
		HxSTD_memset(pCreditMgr->pCreditList , 0x00, ulNumOfCredit*sizeof(NA_CREDIT_Info_t));

		for(ulIndex = 0; ulIndex < ulNumOfCredit; ulIndex++)
		{
			creditId = 0;
			objStatus = caCreditGetId(ppCakCreditList[ulIndex], &creditId);
			if (objStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("[local_cakppv_tdt_buildCreditList] caPurchaseGetProduct fail!!\n");
				continue;
			}
			pCreditMgr->ulNumOfCredit++;
			HxLOG_Info("[local_cakppv_tdt_buildCreditList][%d] ProductID(%d), creditId(%d)\n", ulIndex, creditId);

			providerId = 0;
			objStatus = caCreditGetProviderId(ppCakCreditList[ulIndex], &providerId);
			if (objStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Info("[local_cakppv_tdt_buildCreditList] caPurchaseGetProduct fail!!\n");
			}
			xAmount = 0;
			objStatus = caCreditGetAmount(ppCakCreditList[ulIndex], &xAmount);
			if (objStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Info("[local_cakppv_tdt_buildCreditList] caPurchaseGetProduct fail!!\n");
			}
			xFlags = 0;
			objStatus = caCreditGetFlags(ppCakCreditList[ulIndex], &xFlags);
			if (objStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Info("[local_cakppv_tdt_buildCreditList] caPurchaseGetProduct fail!!\n");
			}
		}

		pTdtPpvStateInfo->eBuildState |= (eNA_PPV_BuildState_CreditComplete);
	}
	hError = ERR_OK;

EXIT_FUNC :
	if(hError == ERR_OK)
	{
		if(pRequest)
		{
			reqStatus = caRequestDispose(pRequest);
			if(reqStatus != CA_REQUEST_NO_ERROR)
			{
				HxLOG_Error("[local_cakppv_tdt_buildCreditList] caRequestDispose fail!!\n");
				hError = ERR_FAIL;
			}
			pTdtPpvStateInfo->pPurchaseHistoryRequest = NULL;
		}
	}

	LEAVE_CRITICAL_PPVTDT;

	HxLOG_Info("[local_cakppv_tdt_buildCreditList] EXIT\n");

	return hError;
}
#endif




#define ____PRODUCT_LOAD_FUNC____

static HERROR					local_cakppv_tdt_processProductLoad(HUINT16 ulMessage)
{
	HERROR 					hError = ERR_FAIL;
	NA_PPV_StateInfo_t		*pTdtPpvStateInfo = NULL;

	HxLOG_Info("[local_cakppv_tdt_processProductLoad] ENTER \n");

	ENTER_CRITICAL_PPVTDT;
	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();


	switch(ulMessage)
	{
		case CAK_PPV_LOADING_LISTENER :
			pTdtPpvStateInfo->eLoadState |= eNA_PPV_LoadState_ProductLoading;
			break;

		case CAK_PPV_LOADED_LISTENER :
			pTdtPpvStateInfo->eLoadState |= eNA_PPV_LoadState_ProductLoaded;
			break;

		default :
			HxLOG_Info("[local_cakppv_tdt_processProductLoad] default!! \n");
			break;
	}

	hError = ERR_OK;

EXIT_FUNC :
	LEAVE_CRITICAL_PPVTDT;
	HxLOG_Info("[local_cakppv_tdt_processProductLoad] EXIT \n");

	return hError;
}




#define ____NIT_FUNC____

static void				local_cakppv_tdt_waitNitTimerCallback(unsigned long TimerId,  void *pArg)
{
	HINT32				nErrCode;
	NaCasMsg_t 	 		stMsg = {0, };


	HxLOG_Info("[local_cakppv_tdt_waitNitTimerCallback] ENTER \n");

	if(TimerId == 0)
	{
		HxLOG_Error("[local_cakppv_tdt_waitNitTimerCallback] TimerId == 0 \n");
		goto EXIT_FUNC;
	}

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_NIT_TIMER_CALL_BACK;
	stMsg.hWnd = (TUnsignedInt32)TimerId;

	nErrCode = VK_PostMessageTimeout(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if (nErrCode)
	{
		HxLOG_Error("[local_cakppv_tdt_waitNitTimerCallback] VK_PostMessageTimeout fail!! \n");
		goto EXIT_FUNC;
	}

EXIT_FUNC :
	HxLOG_Info("Exit\n");
	return;

}


static HERROR					local_cakppv_tdt_setNitTimer(void)
{
	HERROR 					hError = ERR_FAIL;
	HINT32					nResult = 0;
	NA_PPV_StateInfo_t		*pTdtPpvStateInfo = NULL;


	HxLOG_Info("[local_cakppv_tdt_setNitTimer] ENTER \n");
	ENTER_CRITICAL_PPVTDT;

	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();


	nResult = VK_TIMER_EventAfter(	TIMEOUT_DURATION_NIT,
									local_cakppv_tdt_waitNitTimerCallback,
									NULL,
									0,
									&pTdtPpvStateInfo->ulNitWaitTimer);
	if(nResult != VK_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_setNitTimer] VK_TIMER_EventAfter fail!! \n");
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}

	pTdtPpvStateInfo->eSignalingState |= eNA_PPV_SignalingState_NitWating;

	hError = ERR_OK;

EXIT_FUNC :

	LEAVE_CRITICAL_PPVTDT;
	HxLOG_Info("[local_cakppv_tdt_setNitTimer] EXIT \n");

	return hError;


}



static HERROR				local_cakppv_tdt_processNitTimeout(HUINT32 ulTimerId)
{
	HERROR 				hError = ERR_FAIL;
	NA_PPV_StateInfo_t	*pTdtPpvStateInfo = NULL;

	HxLOG_Info("[local_cakppv_tdt_processNitTimeout] ENTER \n");

	ENTER_CRITICAL_PPVTDT;
	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();
	if(pTdtPpvStateInfo->eSignalingState & eNA_PPV_SignalingState_NitReceived )
	{
		HxLOG_Error("[local_cakppv_tdt_processNitTimeout] !eNA_PPV_SignalingState_NitWating!!\n");
		hError = ERR_OK;
		goto EXIT_FUNC;
	}
	if( pTdtPpvStateInfo->ulNitWaitTimer != ulTimerId)
	{
		HxLOG_Error("[local_cakppv_tdt_processNitTimeout] ulNitWaitTimer != ulTimerId!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	pTdtPpvStateInfo->eSignalingState = eNA_PPV_SignalingState_NitTimeout;

	hError = ERR_OK;

EXIT_FUNC :
	LEAVE_CRITICAL_PPVTDT;
	HxLOG_Info("[local_cakppv_tdt_processNitTimeout] EXIT \n");

	return hError;
}



static HERROR				local_cakppv_tdt_processNit(HUINT8 *pucSection, HUINT16 usOnId)
{
	HERROR 				hError = ERR_OK;
	NA_PPV_StateInfo_t	*pTdtPpvStateInfo = NULL;

	HxLOG_Info("[local_cakppv_tdt_processNit] ENTER \n");

	ENTER_CRITICAL_PPVTDT;
	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();
	if(pTdtPpvStateInfo->pNitList)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTdtPpvStateInfo->pNitList, local_cakppv_tdt_freeSection);
		pTdtPpvStateInfo->pNitList = NULL;
	}
	pTdtPpvStateInfo->eSignalingState |= eNA_PPV_SignalingState_NitReceived;

	pTdtPpvStateInfo->pNitList = UTIL_LINKEDLIST_AppendListItemToPointerList(pTdtPpvStateInfo->pNitList, (void*)pucSection);
	pTdtPpvStateInfo->usOnId = usOnId;

	hError = local_cakppv_tdt_parseNit(	pucSection,
										&pTdtPpvStateInfo->ePdtSetMask,
										&pTdtPpvStateInfo->stCPdtInfo,
										&pTdtPpvStateInfo->stHPdtInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_processNit] local_cakppv_tdt_parseNit fail!! \n");
		pTdtPpvStateInfo->ePeriodType = eNA_PPV_Period_Others;
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}
	if(	pTdtPpvStateInfo->ePdtSetMask & eNA_PPV_PdtSet_HPDTPid
		|| pTdtPpvStateInfo->ePdtSetMask & eNA_PPV_PdtSet_CPDTPid
	)
	{
		pTdtPpvStateInfo->bNagraTs = TRUE;
	}

	if(!pTdtPpvStateInfo->bNagraTs)
	{
		pTdtPpvStateInfo->ePeriodType = eNA_PPV_Period_Others;
		HxLOG_Error("[local_cakppv_tdt_processNit] (!bNagraTs) !! \n");
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = ERR_OK;

EXIT_FUNC :
	LEAVE_CRITICAL_PPVTDT;
	HxLOG_Info("[local_cakppv_tdt_processNit] EXIT \n");

	return hError;
}



static HERROR				local_cakppv_tdt_parseNit(HUINT8 *pucNit, NA_PPV_PdtSet_t *pePdtSetMask, NA_PDT_Info_t *pCPdt, NA_PDT_Info_t *pHPdt)
{
	HERROR 				hError = ERR_OK;
	HINT32				nErrCode;
	HUINT16 				usSumOfDes = 0;
	HUINT16 				usDesSize = 0;
	HUINT16 				nDesLoopSize = 0;
	HUINT16				usSize = 0;
	HUINT8 				*pDesNode = NULL;

	if(pucNit == NULL || pePdtSetMask == FALSE ||  pCPdt == NULL || pHPdt == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_parseNit] param fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	usSize = (TUnsignedInt16)get12bit(pucNit+1)+3;	// pucNit rawdata total usSize
	nDesLoopSize = get12bit( pucNit+8 );

	HxLOG_Info("[local_cakppv_tdt_parseNit] nit size(%x) nDesLoopSize(%d)\n", usSize, nDesLoopSize);
#if 0
	if (pucNit != NULL)
	{
		TUnsignedInt16 j = 0, k = 1;
		HxLOG_Print("\n\n\n -------- NIT RAW DATA ---------(usSize:%d)-\n", usSize);

		while ( j < usSize)
		{
			HxLOG_Print("[");
			for (k = 1; k < 21; k++)
			{
				HxLOG_Print("%02x ",*(pucNit+j));
				j++;
				if (j >= usSize) break;
			}
			HxLOG_Print("]\n");
		}
		HxLOG_Print("----------------- NIT RAW DATA END---------------\n\n\n\n");
	}
#endif

	if (pucNit != NULL)
	{
		usDesSize = 0;
		usSumOfDes = 0;
		pDesNode = pucNit + 10;
		while (usSumOfDes < nDesLoopSize)
		{
			HxLOG_Info("pDesNode [%x], pDesNode+1=usSize [%x], pDesNode+8 [%x]\n", *pDesNode, *(pDesNode+1), *(pDesNode+8));
			if(*pDesNode == TAG_LINKAGE)
			{
				if(*(pDesNode+1) > 7)
				{
					if(*(pDesNode+8) == 0x91)
					{
						pCPdt->usTsId =  *(pDesNode+2) << 8 | *(pDesNode+3);
						pCPdt->usOnId = *(pDesNode+4) << 8 | *(pDesNode+5);
						pCPdt->usSvcId = *(pDesNode+6) << 8 | *(pDesNode+7);
						pCPdt->usPid= (*(pDesNode+9) << 8 | *(pDesNode+10)) >> 3;

						*pePdtSetMask |= eNA_PPV_PdtSet_CPDTPid;
						HxLOG_Info("[local_cakppv_tdt_parseNit] NAGRA TS 0x91\n");
					}
					else if(*(pDesNode+8) == 0x90)
					{
						pHPdt->usTsId = *(pDesNode+2) << 8 | *(pDesNode+3);
						pHPdt->usOnId = *(pDesNode+4) << 8 | *(pDesNode+5);
						pHPdt->usSvcId = *(pDesNode+6) << 8 | *(pDesNode+7);
						pHPdt->usPid = (*(pDesNode+9) << 8 | *(pDesNode+10)) >> 3;

						*pePdtSetMask |= eNA_PPV_PdtSet_HPDTPid;
						HxLOG_Info("[local_cakppv_tdt_parseNit] NAGRA TS 0x90\n");
					}
					else
					{
						HxLOG_Print("*(pDesNode+8) != 0x91 or 0x90 [%x]\n", *(pDesNode+8));
					}
				}
			}

			usDesSize = (*(pDesNode+1) + 2); //Tag + usSize +  length of descriptor
			usSumOfDes += usDesSize;
			pDesNode = pDesNode + usDesSize;
			HxLOG_Info("usDesSize(%d), nDesLoopSize(%d)!!\n", usSumOfDes, nDesLoopSize);
		}
	}

	hError = ERR_OK;

EXIT_FUNC :

	return hError;
}


static HERROR				local_cakppv_tdt_retrievePdt(void)
{
	HERROR 				hError = ERR_OK;
	TCaRequestStatus		status;
	NA_PPV_StateInfo_t	*pTdtPpvStateInfo = NULL;
	NA_PDT_Info_t		*pCPdt = NULL;
	NA_PDT_Info_t		*pHPdt = NULL;
	TPpid				providerId = 0;


	HxLOG_Info("[local_cakppv_tdt_retrievePdt] ENTER!!\n");
	ENTER_CRITICAL_PPVTDT;

	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();

/******************************************************************************************************
 *
 * state info init
 *
 *****************************************************************************************************/
	pTdtPpvStateInfo->eLoadState = 0;
	pTdtPpvStateInfo->eBuildState = 0;
	pTdtPpvStateInfo->eSignalingState &= ~(	eNA_PPV_SignalingState_Fail
										| eNA_PPV_SignalingState_NitTimeout
										| eNA_PPV_SignalingState_PurchaseHistoryTimeout
										| eNA_PPV_SignalingState_PackageListTimeout);


/******************************************************************************************************
 *
 * check card & ts info
 *
 *****************************************************************************************************/
	hError = CA_NA_CakGetProviderId(0, &providerId);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_retrievePdt] CA_NA_CakGetProviderId fail!! \n");
		pTdtPpvStateInfo->eSignalingState = eNA_PPV_SignalingState_Fail;
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	// Spain TDT Spec
	if(	!pTdtPpvStateInfo->bNagraCaSysId
		|| !pTdtPpvStateInfo->bNagraTs)
	{
		HxLOG_Error("[local_cakppv_tdt_retrievePdt] !bNagraCaSysId || !bNagraTs!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	pTdtPpvStateInfo->ePeriodType = eNA_PPV_Period_Tdt;

	if(pTdtPpvStateInfo->ePdtSetMask & eNA_PPV_PdtSet_CPDTPid)
	{
		pCPdt = &pTdtPpvStateInfo->stCPdtInfo;
	}
	if(pTdtPpvStateInfo->ePdtSetMask & eNA_PPV_PdtSet_HPDTPid)
	{
		pHPdt = &pTdtPpvStateInfo->stHPdtInfo;
	}


/******************************************************************************************************
 *
 * filter PDT tabel
 *
 *****************************************************************************************************/
	if(pCPdt != NULL || pHPdt != NULL)
	{
		hError = local_cakppv_tdt_requestPdtStop();
		if(hError != ERR_OK)
		{
			HxLOG_Error("[local_cakppv_tdt_retrievePdt] local_cakppv_tdt_requestPdtStop fail!!\n");
			pTdtPpvStateInfo->eSignalingState = eNA_PPV_SignalingState_Fail;
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
	}

	hError = local_cakppv_tdt_requestPdtStart(pCPdt, pHPdt);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_retrievePdt] local_cakppv_tdt_requestPdtStart fail!!\n");
		pTdtPpvStateInfo->eSignalingState = eNA_PPV_SignalingState_Fail;
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	pTdtPpvStateInfo->eSignalingState |= (eNA_PPV_SignalingState_RequestPackageList | eNA_PPV_SignalingState_RequestPurchaseHistory);
	pTdtPpvStateInfo->ePeriodType = eNA_PPV_Period_Tdt;

	hError = ERR_OK;

EXIT_FUNC :
	LEAVE_CRITICAL_PPVTDT;

	return hError;
}



#define ____TP_FUNC____

static HERROR					local_cakppv_tdt_initPpvInfo(void)
{
	HERROR 					hError = ERR_FAIL;
	TCaRequestStatus			reqStatus;
	NA_PPV_StateInfo_t		*pTdtPpvStateInfo = NULL;
	PPV_PURCHASE_LIST		*pPurchaseHistory = NULL;
	PPV_PACKAGE_LIST		*pPackageList = NULL;

	HxLOG_Info("[local_cakppv_tdt_initPpvInfo] ENTER \n");
	ENTER_CRITICAL_PPVTDT;

	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();


/**************************************************************************************
  *
  * section free
  *
  **************************************************************************************/
	if(pTdtPpvStateInfo->pPmtList)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTdtPpvStateInfo->pPmtList, local_cakppv_tdt_freeSection);
		pTdtPpvStateInfo->pPmtList = NULL;
	}
	if(pTdtPpvStateInfo->pCatList)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTdtPpvStateInfo->pCatList, local_cakppv_tdt_freeSection);
		pTdtPpvStateInfo->pCatList = NULL;
	}
	if(pTdtPpvStateInfo->pNitList)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTdtPpvStateInfo->pNitList, local_cakppv_tdt_freeSection);
		pTdtPpvStateInfo->pNitList = NULL;
	}
	if(pTdtPpvStateInfo->pEitList)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTdtPpvStateInfo->pEitList, local_cakppv_tdt_freeSection);
		pTdtPpvStateInfo->pEitList = NULL;
	}
	if(pTdtPpvStateInfo->pSdtList)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTdtPpvStateInfo->pSdtList, local_cakppv_tdt_freeSection);
		pTdtPpvStateInfo->pSdtList = NULL;
	}
	if(pTdtPpvStateInfo->pTdtList)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTdtPpvStateInfo->pTdtList, local_cakppv_tdt_freeSection);
		pTdtPpvStateInfo->pTdtList = NULL;
	}

/**************************************************************************************
  *
  * request dispose
  *
  **************************************************************************************/
	if(pTdtPpvStateInfo->pPackageListRequest)
	{
		reqStatus = caRequestDispose(pTdtPpvStateInfo->pPackageListRequest);
		if(reqStatus != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_initPpvInfo] caRequestDispose fail!!\n");
			hError = ERR_FAIL;
		}
		pTdtPpvStateInfo->pPackageListRequest = NULL;
	}
	if(pTdtPpvStateInfo->pPurchaseHistoryRequest)
	{
		reqStatus = caRequestDispose(pTdtPpvStateInfo->pPurchaseHistoryRequest);
		if(reqStatus != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_initPpvInfo] caRequestDispose fail!!\n");
			hError = ERR_FAIL;
		}
		pTdtPpvStateInfo->pPurchaseHistoryRequest = NULL;
	}

/**************************************************************************************
  *
  * timer cancel
  *
  **************************************************************************************/
	if(pTdtPpvStateInfo->ulPurchaseHistoryTimer)
	{
		VK_TIMER_Cancel(pTdtPpvStateInfo->ulPurchaseHistoryTimer);
		pTdtPpvStateInfo->ulPurchaseHistoryTimer = 0;
		pTdtPpvStateInfo->ulPurchaseHistoryCounter = 0;
	}
	if(pTdtPpvStateInfo->ulPackageListTimer)
	{
		VK_TIMER_Cancel(pTdtPpvStateInfo->ulPackageListTimer);
		pTdtPpvStateInfo->ulPackageListTimer = 0;
		pTdtPpvStateInfo->ulPackageListCounter= 0;
	}

/**************************************************************************************
  *
  * init
  *
  **************************************************************************************/
	HxSTD_memset(pTdtPpvStateInfo, 0x00, sizeof(NA_PPV_StateInfo_t));

	pPackageList = local_cakppv_tdt_getPackageList();
	hError = local_cakppv_tdt_initPackageList(pPackageList);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_initPpvInfo] local_cakppv_tdt_initPackageList fail!! \n");
		hError = ERR_FAIL;
	}

	pPurchaseHistory = local_cakppv_tdt_getPurchaseHistory();
	hError = local_cakppv_tdt_clearPurchaseHistory(pPurchaseHistory);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_initPpvInfo] local_cakppv_tdt_clearPurchaseHistory fail !!\n");
		hError = ERR_FAIL;
	}

	hError = ERR_OK;

EXIT_FUNC :
	LEAVE_CRITICAL_PPVTDT;

	return hError;

}




static HERROR		local_cakppv_tdt_processCat(HBOOL *pIsTdtSystemIds)
{
	HERROR 				hError = ERR_OK;
	HUINT16				*pusCaSysIds = NULL;
	HUINT32				ulSize = 0;
	HUINT32				ulIndex = 0;
	HBOOL				bNagraSystemId = FALSE;
	NA_PPV_StateInfo_t	*pTdtPpvStateInfo = NULL;

	HxLOG_Info("[local_cakppv_tdt_processCat] ENTER \n");

	ENTER_CRITICAL_PPVTDT;
	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();

	if(pIsTdtSystemIds == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_processCat] param fail!!\n");
		pTdtPpvStateInfo->ePeriodType = eNA_PPV_Period_Others;
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	*pIsTdtSystemIds = FALSE;

	pTdtPpvStateInfo->eSignalingState |= eNA_PPV_SignalingState_CatReceived;

	CA_NA_PsiGetCatCaSystemIds(&pusCaSysIds, &ulSize);
	if(pusCaSysIds == NULL || ulSize == 0)
	{
		HxLOG_Error("[local_cakppv_tdt_processCat] pusCaSysIds == NULL || ulSize == 0!!\n");
		pTdtPpvStateInfo->ePeriodType = eNA_PPV_Period_Others;
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	for(ulIndex = 0; ulIndex < ulSize; ulIndex++)
	{
		if(pusCaSysIds[ulIndex] >= 0x1812 && pusCaSysIds[ulIndex] <= 0x1819)
		{
			bNagraSystemId = TRUE;
			break;
		}
	}

	if(bNagraSystemId)
	{
		pTdtPpvStateInfo->bNagraCaSysId = TRUE;
	}
	else
	{
		pTdtPpvStateInfo->ePeriodType = eNA_PPV_Period_Others;
	}

	*pIsTdtSystemIds = bNagraSystemId;


	hError = ERR_OK;

EXIT_FUNC :

	if(pusCaSysIds)
	{
		OxCAS_Free(pusCaSysIds);
	}

	LEAVE_CRITICAL_PPVTDT;

	return hError;

}



static HERROR				local_cakppv_tdt_requestPdtStart(NA_PDT_Info_t *pCPdtInfo, NA_PDT_Info_t *pHPdtInfo)
{
	HERROR 				hError = ERR_FAIL;
	TCaRequestStatus		status = 0;
	TCaRequest			*pRequest = NULL;
	Handle_t				hAction = 0;


	HxLOG_Info("[local_cakppv_tdt_requestPdtStart]ENTER \n");

	hError = CA_NA_PSI_ifExt_GetLiveAction(&hAction);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPdtStart] CA_NA_PSI_ifExt_GetLiveAction fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	status = caRequestCreate(CA_REQUEST_TYPE_PDT_START, &pRequest);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPdtStart] caRequestCreate fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	if (pCPdtInfo || pHPdtInfo)
	{
		status= caRequestSetTransportSessionId(pRequest, (TUnsignedInt32)hAction);
		if(status != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_requestPdtStart] caRequestSetTransportSessionId fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
	}

	status = CA_REQUEST_ERROR;
	if (pCPdtInfo)
	{
		status = caRequestSetPid(pRequest, pCPdtInfo->usPid);
		if(status != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_requestPdtStart] caRequestSetPid cpdt fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
	}
	if (pHPdtInfo)
	{
		status = caRequestSetPid(pRequest, pHPdtInfo->usPid);
		if(status != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_requestPdtStart] caRequestSetPid hpdt fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
	}

	if (status == CA_REQUEST_NO_ERROR)
	{
		status = caRequestSend(pRequest);
		if(status != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_requestPdtStart] caRequestSend fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
	}
	HxLOG_Info("[local_cakppv_tdt_requestPdtStart]EXIT!!\n");

	hError = ERR_OK;

EXIT_FUNC :
	if(pRequest)
	{
		status = caRequestDispose(pRequest);
		if(status != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_requestPdtStart] caRequestDispose fail!!\n");
		}
	}

	return hError;
}



//항상 synch 모드....
static HERROR				local_cakppv_tdt_requestPdtStop(void)
{
	HERROR				hError = ERR_FAIL;
	TCaRequestStatus		status;
	TCaRequest			*pRequest = NULL;

	HxLOG_Print("[local_cakppv_tdt_requestPdtStop]ENTER \n");

	// PDT request
	status = caRequestCreate(CA_REQUEST_TYPE_PDT_STOP, &pRequest);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPdtStop] caRequestCreate fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	status = caRequestSend(pRequest);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPdtStop] caRequestSend fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	HxLOG_Info("[local_cakppv_tdt_requestPdtStop]EXIT - status(%d)\n", status);

	hError = ERR_OK;

EXIT_FUNC :
	if(pRequest)
	{
		status = caRequestDispose(pRequest);
		if(status != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_requestPdtStop] caRequestDispose fail!!\n");
		}
	}

	return hError;
}


#define ____PACKAGE_FUNC____

static void 					local_cakppv_tdt_waitPackageListTimerCallback(unsigned long nTimerId,  void *pArg)
{
	TSignedInt32				nErrCode;
	NaCasMsg_t 	 			stMsg;

	HxLOG_Info("[local_cakppv_tdt_waitPackageListTimerCallback] ENTER \n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_PACKAGE_TIMER_CALL_BACK;
	stMsg.hWnd = (TUnsignedInt32)nTimerId;

	nErrCode = VK_PostMessageTimeout(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if (nErrCode)
	{
		HxLOG_Error("[local_cakppv_tdt_waitPackageListTimerCallback] VK_PostMessageTimeout fail!! \n");
		goto EXIT_FUNC;
	}

EXIT_FUNC :

	HxLOG_Info("[local_cakppv_tdt_waitPackageListTimerCallback] EXIT \n");
	return;
}




static HERROR					local_cakppv_tdt_checkPackageListTimer(HUINT32 ulTimerId)
{
	HERROR 					hError = ERR_FAIL;
	HINT32					nResult = 0;
	NA_PPV_StateInfo_t		*pTdtPpvStateInfo = NULL;


	HxLOG_Info("[local_cakppv_tdt_checkPackageListTimer] ENTER \n");
	ENTER_CRITICAL_PPVTDT;

	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();
/**************************************************************************************
  *
  * case 1. check timer id
  *
  **************************************************************************************/
	if(pTdtPpvStateInfo->ulPackageListTimer != ulTimerId)
	{
		HxLOG_Error("[local_cakppv_tdt_checkPackageListTimer] ulPackageListTimer != ulTimerId!! \n");
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}

/**************************************************************************************
  *
  * case 2. check time out
  *
  **************************************************************************************/
	if(pTdtPpvStateInfo->ulPackageListCounter >= TIMER_COUNTER_PACKAGELIST)
	{
		HxLOG_Error("[local_cakppv_tdt_checkPackageListTimer] eNA_PPV_SignalingState_PurchaseHistoryTimeout!! \n");
		pTdtPpvStateInfo->eSignalingState |= eNA_PPV_SignalingState_PackageListTimeout;
		pTdtPpvStateInfo->ulPackageListCounter = 0;
		pTdtPpvStateInfo->ulPackageListTimer = 0;
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}

#if 0
/**************************************************************************************
  *
  * case 3. check products loaded
  *
  **************************************************************************************/
	if( !(pTdtPpvStateInfo->eBuildState & eNA_PPV_BuildState_PackageListComplete) )
	{
		nResult = VK_TIMER_EventAfter(	TIMEOUT_DURATION_PACKAGELIST,
										local_cakppv_tdt_waitPackageListTimerCallback,
										NULL,
										0,
										&pTdtPpvStateInfo->ulPackageListTimer);
		if(nResult != VK_OK)
		{
			HxLOG_Error("[local_cakppv_tdt_checkPackageListTimer] VK_TIMER_EventAfter fail!! \n");
		}
		pTdtPpvStateInfo->ulPackageListCounter++;
	}
#endif

/**************************************************************************************
  *
  * case 4. call build package list~
  *
  **************************************************************************************/
	hError = ERR_OK;

EXIT_FUNC :
	LEAVE_CRITICAL_PPVTDT;

	HxLOG_Info("[local_cakppv_tdt_checkPackageListTimer] EXIT \n");

	return hError;


}





static void					local_cakppv_tdt_packageListCallback(TCaRequest* pxRequest)
{
	TSignedInt32				nErrCode;
	NaCasMsg_t 	 			stMsg;

	HxLOG_Info("[local_cakppv_tdt_packageListCallback] ENTER \n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_PACKAGE_LIST_CALL_BACK;
	stMsg.lParam1 = 0;
	stMsg.hWnd = pxRequest;

	nErrCode = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if (nErrCode)
	{
		HxLOG_Error("[local_cakppv_tdt_packageListCallback] VK_PostMessageTimeout fail!! \n");
		goto EXIT_FUNC;
	}

EXIT_FUNC :

	HxLOG_Info("[local_cakppv_tdt_packageListCallback] EXIT \n");
	return;
}


static HBOOL					local_cakppv_tdt_IsAvailableBusinessType(TPpid providerId, TUnsignedInt8 ucBusinessType)
{
	HBOOL					bRet = FALSE;

	switch(providerId)
	{
		case NA_MEDIASET_PROVIDER_ID:
			if (	ucBusinessType == ePACKAGE_SPECIAL
				|| (ucBusinessType >= ePACKAGE_CHILDREN && ucBusinessType <= ePACKAGE_ADULT_MORE)
			)
			{
				bRet = TRUE;
			}
			break;

		default :
			if (ucBusinessType >= ePACKAGE_SPORT && ucBusinessType <= ePACKAGE_OTHERS_MORE)
			{
				bRet = TRUE;
			}
			break;
	}

	return bRet;

}



static HERROR				local_cakppv_tdt_requestPackageList(HBOOL bFirst, HUINT32 ulProviderId)
{
	HERROR				hError = ERR_FAIL;
	TCaRequestStatus		status;
	TCaRequest			*pRequest = NULL;
	HINT32				nResult = 0;
	PPV_PACKAGE_LIST	*pPackageList = NULL;
	NA_PPV_StateInfo_t	*pTdtPpvStateInfo = NULL;

	HxLOG_Info("[local_cakppv_tdt_requestPackageList] ENTER!!\n");
	ENTER_CRITICAL_PPVTDT;

	pPackageList = local_cakppv_tdt_getPackageList();

	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();

	if( !(pTdtPpvStateInfo->eLoadState & eNA_PPV_LoadState_ProductLoaded) )
	{
/**************************************************************************************
  *
  * set timer
  *
  **************************************************************************************/
		HxLOG_Error("[local_cakppv_tdt_requestPackageList] VK_TIMER_EventAfter!! \n");
		if(bFirst)
		{
			pTdtPpvStateInfo->ulPackageListCounter = 0;
			pTdtPpvStateInfo->ulPackageListTimer = 0;
		}
		nResult = VK_TIMER_EventAfter(	TIMEOUT_DURATION_PACKAGELIST,
										local_cakppv_tdt_waitPackageListTimerCallback,
										NULL,
										0,
										&pTdtPpvStateInfo->ulPackageListTimer);
		if(nResult != VK_OK)
		{
			HxLOG_Error("[local_cakppv_tdt_requestPackageList] VK_TIMER_EventAfter fail!! \n");
			hError =ERR_FAIL;
			goto EXIT_FUNC;
		}
		pTdtPpvStateInfo->ulPackageListCounter++;

		goto EXIT_FUNC;
	}


/**************************************************************************************
  *
  * clear old purchase list
  *
  **************************************************************************************/
	hError = local_cakppv_tdt_initPackageList(pPackageList);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPackageList] local_cakppv_tdt_initPackageList fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}


/**************************************************************************************
  *
  * request package list
  *
  **************************************************************************************/
	status = caRequestCreate(CA_REQUEST_TYPE_PRODUCTS, &pRequest);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPackageList] caRequestCreate fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	status = caRequestSetAsynchronousResponse(pRequest, local_cakppv_tdt_packageListCallback);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPackageList] caRequestSetAsynchronousResponse fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	status = caRequestSend(pRequest);
	if (status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPackageList] caRequestSend fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

/**************************************************************************************
  *
  * set timer
  *
  **************************************************************************************/
	pTdtPpvStateInfo->eSignalingState |= eNA_PPV_SignalingState_RequestPackageList;

	pTdtPpvStateInfo->pPackageListRequest = pRequest;

	hError = ERR_OK;

EXIT_FUNC :

	if(hError != ERR_OK)
	{
		if(pRequest)
		{
			status = caRequestDispose(pRequest);
			if(status != CA_REQUEST_NO_ERROR)
			{
				HxLOG_Error("[local_cakppv_tdt_requestPackageList] caRequestDispose fail!!\n");
				hError = ERR_FAIL;
			}
			pRequest = NULL;
		}
	}
	LEAVE_CRITICAL_PPVTDT;

	HxLOG_Info("[local_cakppv_tdt_requestPackageList] EXIT!!\n");

	return hError;
}



static void		local_cakppv_tdt_InitPackageInfo(PACKAGE_INFO *pPackageInfo)
{
	if(pPackageInfo == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_InitPackageInfo] pPackageList == NULL fail!! \n");
		goto EXIT_FUNC;
	}

	HxSTD_memset(pPackageInfo, 0x00, sizeof(PACKAGE_INFO));

EXIT_FUNC :

	return;
}


static HERROR			local_cakppv_tdt_initPackageList(PPV_PACKAGE_LIST* pPackageList)
{
	HERROR					hError = ERR_FAIL;
	TCaRequestStatus			status;
	PPV_PACKAGE_LIST_ITEM	*pTemItem = NULL;
	PPV_PACKAGE_LIST_ITEM	*pPrevItem = NULL;


	HxLOG_Info("[local_cakppv_tdt_initPackageList] ENTER!!\n");

	if(pPackageList == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_initPackageList] pPackageList == NULL fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	HxLOG_Print("[local_cakppv_tdt_initPackageList] xNbItems(%ld)\n", pPackageList->xNbItems);

	if(pPackageList->pList)
	{
		pPrevItem =  pTemItem = pPackageList->pList;
		while (pTemItem)
		{
			if (pTemItem->pServices)
			{
				OxCAS_Free(pTemItem->pServices);
				pTemItem->pServices = NULL;
			}

			local_cakppv_tdt_InitPackageInfo(&pTemItem->PackageInfo);

			pPrevItem = pTemItem;
			pTemItem = pTemItem->pNext;

			if(pPrevItem)
			{
				OxCAS_Free(pPrevItem);
				pPrevItem = NULL;
			}
		}
	}

	pPackageList->pList = NULL;
	pPackageList->xNbItems = 0;

	hError = ERR_OK;

EXIT_FUNC :
	HxLOG_Info("[local_cakppv_tdt_initPackageList] EXIT!!\n");

	return hError;
}


static HERROR						local_cakppv_tdt_buildPackageList(TCaRequest* pxRequest)
{
	HERROR						hError = ERR_FAIL;
	TCaRequestStatus				reqStatus;
	TCaObjectStatus				objStatus = CA_OBJECT_NO_ERROR;
	TCaObjectStatus				svcObjStatus = CA_OBJECT_NO_ERROR;
	TUnsignedInt16				usPackageIndex = 0;
	TUnsignedInt16				usServiceIndex = 0;
	TUnsignedInt32 				ulNumOfPackage = 0;
	TCaProduct					**ppPackageProduct = NULL;
	TCaService					**ppServices = NULL;
	TUnsignedInt16				usIndex = 0;
	PPV_PACKAGE_LIST			*pPackageList = NULL;

	PPV_PACKAGE_LIST_ITEM		*pHolder = NULL;
	PPV_PACKAGE_LIST_ITEM		*pNewPackage = NULL;
	TDvbTriplet					*pNewTripleIds = NULL;
	TPpid 						providerId;
	TChar						*pPackageName;
	TUnsignedInt8					ucBusinessType = 0;
	HBOOL						bAvailableBusiness = FALSE;
	TCaProductId					productId = 0;
	TUnsignedInt32				ulPackageNameLength = 0;
	NA_PPV_StateInfo_t			*pTdtPpvStateInfo = NULL;
	TCaRequest					*pRequest = NULL;
	TSize						nSize = 0;
	HUINT8						*pucDescription = NULL;
	HUINT8						*pucDisclaimer = NULL;
	HUINT8						*pucProviderName = NULL;

	HxLOG_Info("[local_cakppv_tdt_buildPackageList] ENTER!!\n");
	ENTER_CRITICAL_PPVTDT;

	pPackageList = local_cakppv_tdt_getPackageList();

/**************************************************************************************
  *
  * check product loaded
  *
  **************************************************************************************/
	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();
	if( !(pTdtPpvStateInfo->eLoadState & eNA_PPV_LoadState_ProductLoaded)  )
	{
		HxLOG_Error("[local_cakppv_tdt_buildPackageList] (!eNA_PPV_LoadState_ProductLoaded)!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}


/**************************************************************************************
  *
  * get purchase history
  *
  **************************************************************************************/
	if(pxRequest != NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_buildPackageList] (pxRequest != NULL)!!\n");
		if(pxRequest != pTdtPpvStateInfo->pPackageListRequest)
		{
			HxLOG_Error("[local_cakppv_tdt_buildPackageList] (pxRequest != pTdtPpvStateInfo->pPurchaseHistoryRequest)!!\n");
			pTdtPpvStateInfo->pPackageListRequest = pxRequest;
		}
	}
	pRequest = pTdtPpvStateInfo->pPackageListRequest;

	hError = local_cakppv_tdt_initPackageList(pPackageList);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_buildPackageList] local_cakppv_tdt_initPackageList fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	reqStatus = caRequestGetObjects(pRequest, &ulNumOfPackage, &ppPackageProduct);
	if(reqStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_buildPackageList] caRequestGetObjects fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

#if 1
	hError = CA_NA_CakGetProviderId(0, &providerId);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_buildPackageList] CA_NA_CakGetProviderId fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
#endif
	HxLOG_Error("[local_cakppv_tdt_buildPackageList] providerId(%x), ulNumOfPackage(%d)!! \n", providerId, ulNumOfPackage);

	for(usPackageIndex = 0; usPackageIndex < ulNumOfPackage; usPackageIndex++)
	{
		pNewPackage = NULL;
		pNewPackage = (PPV_PACKAGE_LIST_ITEM*)OxCAS_Malloc(sizeof(PPV_PACKAGE_LIST_ITEM));
		if (pNewPackage == NULL)
		{
			HxLOG_Error("[local_cakppv_tdt_buildPackageList] OxCAS_Malloc fail!! \n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
		HxSTD_memset(pNewPackage, 0x00, sizeof(PPV_PACKAGE_LIST_ITEM));

/**************************************************************************************
  *
  * mandatory package product info
  *
  **************************************************************************************/
		objStatus = caProductGetBusinessType(ppPackageProduct[usPackageIndex], &ucBusinessType);
		if (objStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_buildPackageList] caProductGetBusinessType fail!! \n");
			continue;
		}

#if		0
		bAvailableBusiness = FALSE;
		bAvailableBusiness = local_cakppv_tdt_IsAvailableBusinessType(providerId, ucBusinessType);
		if(bAvailableBusiness != TRUE)
		{
			HxLOG_Error("[local_cakppv_tdt_buildPackageList] local_cakppv_tdt_IsAvailableBusinessType fail!! \n");
			continue;
		}

		productId = 0;
		objStatus = caProductGetId(ppPackageProduct[usPackageIndex], &productId);
		if (objStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Error("[local_cakppv_tdt_buildPackageList] caProductGetId fail!! \n");
			continue;
		}
#endif

		pPackageName = NULL;
		ulPackageNameLength = 0;
		objStatus = caProductGetName(ppPackageProduct[usPackageIndex], &ulPackageNameLength, &pPackageName);
		if (objStatus != CA_OBJECT_NO_ERROR || ulPackageNameLength == 0)
		{
			HxLOG_Error("[local_cakppv_tdt_buildPackageList] caProductGetName fail!! \n");
			continue;
		}

/**************************************************************************************
  *
  * set mandatory info
  *
  **************************************************************************************/
		pNewPackage->pProduct = ppPackageProduct[usPackageIndex];
		pNewPackage->pRequest = pPackageList->pRequest;
		pNewPackage->BusinessType = ucBusinessType;
		pNewPackage->ProductID = productId;
		if (ulPackageNameLength > MAX_PACKAGE_NAME_LENGTH)
		{
			ulPackageNameLength = MAX_PACKAGE_NAME_LENGTH;
		}
		pNewPackage->PackageNameLength = ulPackageNameLength;
		MWC_UTIL_DvbStrncpy(pNewPackage->PackageName, pPackageName, ulPackageNameLength);
		pPackageList->xNbItems++;

		if (pPackageName)
		{
			HxLOG_Info("[local_cakppv_tdt_buildPackageList] PackageName(%s)!! \n", pNewPackage->PackageName);
		}
/**************************************************************************************
  *
  * product description
  *
  **************************************************************************************/
		nSize= 0;
		pucDescription = NULL;
		pNewPackage->PackageInfo.aucDescription[0] = 0;
		objStatus = caProductGetDescription(ppPackageProduct[usPackageIndex], &nSize, &pucDescription);
		if (objStatus == CA_REQUEST_NO_ERROR && nSize >0)
		{
			if (nSize > MAX_MSET_PPV_DESCRIPTION_LENGTH)
			{
				nSize = MAX_MSET_PPV_DESCRIPTION_LENGTH;
			}
			pNewPackage->PackageInfo.nDesSize = nSize;
			MWC_UTIL_DvbStrncpy(pNewPackage->PackageInfo.aucDescription, pucDescription, nSize);
			HxLOG_Info("[local_cakppv_tdt_buildPackageList] pucDescription(%s)!! \n", pucDescription);
		}

		nSize= 0;
		pucDisclaimer = NULL;
		pNewPackage->PackageInfo.aucDisclaimer[0] = 0;
		objStatus = caProductGetDisclaimer(ppPackageProduct[usPackageIndex], &nSize, &pucDisclaimer);
		if (objStatus == CA_REQUEST_NO_ERROR && pucDisclaimer != NULL)
		{
			if (nSize > MAX_MSET_PPV_DISCLAIMER_LENGTH)
			{
				nSize = MAX_MSET_PPV_DISCLAIMER_LENGTH;
			}
			pNewPackage->PackageInfo.nDisclaimerSize = nSize;
			MWC_UTIL_DvbStrncpy(pNewPackage->PackageInfo.aucDisclaimer, pucDescription, nSize);
			HxLOG_Info("[local_cakppv_tdt_buildPackageList] pucDisclaimer(%s)!! \n", pucDisclaimer);
		}

/**************************************************************************************
  *
  * optional info
  *
  **************************************************************************************/

		objStatus = caProductGetCost(ppPackageProduct[usPackageIndex], &pNewPackage->Cost);
		if (objStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Info("[local_cakppv_tdt_buildPackageList] caProductGetCost fail!! \n");
		}

		objStatus = caProductGetPurchaseStartTime(ppPackageProduct[usPackageIndex], &pNewPackage->PurchaseStartTime);
		if (objStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Info("[local_cakppv_tdt_buildPackageList] caProductGetPurchaseStartTime fail!! \n");
		}

		objStatus = caProductGetPurchaseEndTime(ppPackageProduct[usPackageIndex], &pNewPackage->PurchaseEndTime);
		if (objStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Info("[local_cakppv_tdt_buildPackageList] caProductGetPurchaseEndTime fail!! \n");
		}

		objStatus = caProductGetFlags (ppPackageProduct[usPackageIndex], &pNewPackage->PurchasableFlag);
		if (objStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Info("[local_cakppv_tdt_buildPackageList] caProductGetFlags fail!! \n");
		}

/**************************************************************************************
  *
  * product service info
  *
  **************************************************************************************/
		ppServices = NULL;
		pNewPackage->NumOfServices = 0;
		svcObjStatus = caProductGetServices(ppPackageProduct[usPackageIndex], &pNewPackage->NumOfServices, &ppServices);
		if (svcObjStatus == CA_OBJECT_NO_ERROR && ppServices != NULL)
		{
			pNewTripleIds = NULL;
			pNewTripleIds = OxCAS_Malloc(pNewPackage->NumOfServices *sizeof(TDvbTriplet));
			if (pNewTripleIds == NULL)
			{
				HxLOG_Error("[local_cakppv_tdt_buildPackageList] OxCAS_Malloc fail!! \n");
				OxCAS_Free(pNewPackage);
				pNewPackage = NULL;
				continue;
			}
			HxSTD_memset(pNewTripleIds, 0x00, pNewPackage->NumOfServices *sizeof(TDvbTriplet));

			for (usServiceIndex = 0; usServiceIndex < pNewPackage->NumOfServices; usServiceIndex++)
			{
				objStatus = caServiceGetLocator(ppServices[usServiceIndex], &pNewTripleIds[usServiceIndex]);
				if (objStatus != CA_OBJECT_NO_ERROR)
				{
					HxLOG_Info("[local_cakppv_tdt_buildPackageList] caServiceGetLocator fail!! \n");
				}

				nSize = 0;
				pucProviderName = NULL;
				objStatus = caServiceGetProviderName(ppServices[usServiceIndex], &nSize, &pucProviderName);
				if (objStatus != CA_OBJECT_NO_ERROR)
				{
					HxLOG_Info("[local_cakppv_tdt_buildPackageList] caServiceGetProviderName fail!!\n");
				}
				else
				{
					HxLOG_Info("[local_cakppv_tdt_buildPackageList] pucProviderName(%s)!!\n", pucProviderName);
				}
			}
			pNewPackage->pServices = pNewTripleIds;
		}

		if (pHolder == NULL)
		{
			pPackageList->pList = pHolder =pNewPackage;
		}
		else
		{
			pHolder->pNext = pNewPackage;
			pHolder = pNewPackage;
		}
	}

	pTdtPpvStateInfo->eBuildState |= (eNA_PPV_BuildState_PackageListComplete);

/**************************************************************************************
  *
  * cancel timer
  *
  **************************************************************************************/
	if(pTdtPpvStateInfo->ulPackageListTimer !=0 )
	{
		VK_TIMER_Cancel(pTdtPpvStateInfo->ulPackageListTimer);
		pTdtPpvStateInfo->ulPackageListTimer = 0;
		pTdtPpvStateInfo->ulPackageListCounter = 0;
	}

	hError = ERR_OK;

EXIT_FUNC :

	if(hError == ERR_OK)
	{
		if(pRequest)
		{
			reqStatus = caRequestDispose(pRequest);
			if(reqStatus != CA_REQUEST_NO_ERROR)
			{
				HxLOG_Error("[local_cakppv_tdt_buildPackageList] caRequestDispose fail!!\n");
				hError = ERR_FAIL;
			}
			pTdtPpvStateInfo->pPackageListRequest = NULL;
		}
	}
	LEAVE_CRITICAL_PPVTDT;

	HxLOG_Info("[local_cakppv_tdt_buildPackageList] EXIT!!\n");

	return hError;
}






#define ____PURCHASE_FUNC____
static void				local_cakppv_tdt_waitPurchaseHistoryTimerCallback(unsigned long TimerId,  void *pArg)
{
	HINT32				nErrCode;
	NaCasMsg_t 	 		stMsg = {0, };


	HxLOG_Info("[local_cakppv_tdt_waitPurchaseHistoryTimerCallback] ENTER \n");

	if(TimerId == 0)
	{
		HxLOG_Error("[local_cakppv_tdt_waitPurchaseHistoryTimerCallback] TimerId == 0 \n");
		goto EXIT_FUNC;
	}

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_PURCHASEHISTORY_TIMER_CALL_BACK;
	stMsg.hWnd = (TUnsignedInt32)TimerId;

	nErrCode = VK_PostMessageTimeout(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if (nErrCode)
	{
		HxLOG_Error("[local_cakppv_tdt_waitPurchaseHistoryTimerCallback] VK_PostMessageTimeout fail!! \n");
		goto EXIT_FUNC;
	}

EXIT_FUNC :

	HxLOG_Info("Exit\n");
	return;

}


static HERROR					local_cakppv_tdt_checkPurchaseHistoryTimer(HUINT32 ulTimerId)
{
	HERROR 					hError = ERR_FAIL;
	HINT32					nResult = 0;
	NA_PPV_StateInfo_t		*pTdtPpvStateInfo = NULL;


	HxLOG_Info("[local_cakppv_tdt_checkPurchaseHistoryTimer] ENTER \n");
	ENTER_CRITICAL_PPVTDT;

	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();

/**************************************************************************************
  *
  * case 1. check timer id
  *
  **************************************************************************************/
	if(pTdtPpvStateInfo->ulPurchaseHistoryTimer != ulTimerId)
	{
		HxLOG_Error("[local_cakppv_tdt_checkPurchaseHistoryTimer] ulPurchaseHistoryTimer != ulTimerId!! \n");
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}

/**************************************************************************************
  *
  * case 2. check time out
  *
  **************************************************************************************/
	if(pTdtPpvStateInfo->ulPurchaseHistoryCounter >= TIMER_COUNTER_PURCHASEHISTORY)
	{
		HxLOG_Error("[local_cakppv_tdt_checkPurchaseHistoryTimer] eNA_PPV_SignalingState_PurchaseHistoryTimeout!! \n");
		pTdtPpvStateInfo->eSignalingState |= eNA_PPV_SignalingState_PurchaseHistoryTimeout;
		pTdtPpvStateInfo->ulPurchaseHistoryTimer = 0;
		pTdtPpvStateInfo->ulPurchaseHistoryCounter = 0;
		hError =ERR_FAIL;
		goto EXIT_FUNC;
	}


#if 0
/**************************************************************************************
  *
  * case 3. check products loaded
  *
  **************************************************************************************/
	if( !(pTdtPpvStateInfo->eBuildState & eNA_PPV_BuildState_PurchaseHistoryComplete) )
	{
		HxLOG_Info("[local_cakppv_tdt_checkPurchaseHistoryTimer] VK_TIMER_EventAfter \n");

		nResult = VK_TIMER_EventAfter(	TIMEOUT_DURATION_PURCHASEHISTORY,
										local_cakppv_tdt_waitPurchaseHistoryTimerCallback,
										NULL,
										0,
										&pTdtPpvStateInfo->ulPurchaseHistoryTimer);
		if(nResult != VK_OK)
		{
			HxLOG_Error("[local_cakppv_tdt_checkPurchaseHistoryTimer] VK_TIMER_EventAfter fail!! \n");
		}
		pTdtPpvStateInfo->ulPurchaseHistoryCounter++;
	}
#endif

/**************************************************************************************
  *
  * case 4. call build purchase history~
  *
  **************************************************************************************/
	hError = ERR_OK;

EXIT_FUNC :
	LEAVE_CRITICAL_PPVTDT;

	HxLOG_Info("[local_cakppv_tdt_checkPurchaseHistoryTimer] EXIT \n");

	return hError;


}



static HERROR		local_cakppv_tdt_clearPurchaseHistory(PPV_PURCHASE_LIST* pPurchaseManager)
{
	HERROR						hError = ERR_FAIL;

	HxLOG_Info("[local_cakppv_tdt_clearPurchaseHistory] ENTER\n");

	if(pPurchaseManager == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_clearPurchaseHistory] param fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	if(pPurchaseManager->pList)
	{
		OxCAS_Free(pPurchaseManager->pList);
		pPurchaseManager->pList = NULL;
	}

	HxSTD_memset(pPurchaseManager, 0x00, sizeof(PPV_PURCHASE_LIST));

	hError = ERR_OK;

EXIT_FUNC :
	HxLOG_Info("[local_cakppv_tdt_clearPurchaseHistory] EXIT\n");

	return hError;
}


static HERROR 			local_cakppv_tdt_requestPurchaseHistory(HBOOL bFirst, TUnsignedInt32 ulProviderId)
{
	HERROR				hError = ERR_FAIL;
	HINT32				nResult = 0;
	TCaRequestStatus 		status;
	TCaRequest			*pRequest = NULL;
	TEventStatus 			errMsg = CA_EVENT_SUCCESS;
	PPV_PURCHASE_LIST	*pPurchaseHistory = NULL;
	NA_PPV_StateInfo_t	*pTdtPpvStateInfo = NULL;

	HxLOG_Info("[local_cakppv_tdt_requestPurchaseHistory] Enter - ulProviderId = 0x%x\n", ulProviderId);
	ENTER_CRITICAL_PPVTDT;

	pPurchaseHistory = local_cakppv_tdt_getPurchaseHistory();

	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();

	if( !(pTdtPpvStateInfo->eLoadState & eNA_PPV_LoadState_ProductLoaded) )
	{
/**************************************************************************************
  *
  * set timer
  *
  **************************************************************************************/
		HxLOG_Error("[local_cakppv_tdt_requestPurchaseHistory] VK_TIMER_EventAfter!! \n");
		if(bFirst)
		{
			pTdtPpvStateInfo->ulPurchaseHistoryTimer = 0;
			pTdtPpvStateInfo->ulPurchaseHistoryCounter = 0;
		}
		nResult = VK_TIMER_EventAfter(	TIMEOUT_DURATION_PURCHASEHISTORY,
										local_cakppv_tdt_waitPurchaseHistoryTimerCallback,
										NULL,
										0,
										&pTdtPpvStateInfo->ulPurchaseHistoryTimer);
		if(nResult != VK_OK)
		{
			HxLOG_Error("[local_cakppv_tdt_requestPurchaseHistory] VK_TIMER_EventAfter fail!! \n");
			hError =ERR_FAIL;
			goto EXIT_FUNC;
		}
		pTdtPpvStateInfo->ulPurchaseHistoryCounter++;

		goto EXIT_FUNC;
	}


/**************************************************************************************
  *
  * clear old purchase list
  *
  **************************************************************************************/
	hError = local_cakppv_tdt_clearPurchaseHistory(pPurchaseHistory);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPurchaseHistory] local_cakppv_tdt_clearPurchaseHistory fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	pPurchaseHistory->providerID = ulProviderId;

/**************************************************************************************
  *
  * request purchase history
  *
  **************************************************************************************/
	status = caRequestCreate(CA_REQUEST_TYPE_PURCHASE_HISTORY, &pRequest);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPurchaseHistory] caRequestCreate fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	status = caRequestSetAsynchronousResponse(pRequest, local_cakppv_tdt_purchaseHistoryCallback);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPurchaseHistory] caRequestSetAsynchronousResponse fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	status = caRequestSend(pRequest);
	if(status != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_requestPurchaseHistory] caRequestSend fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = ERR_OK;

	pTdtPpvStateInfo->eSignalingState |= eNA_PPV_SignalingState_RequestPurchaseHistory;

EXIT_FUNC :

	if(hError != ERR_OK)
	{
		if(pRequest)
		{
			status = caRequestDispose(pRequest);
			if(status != CA_REQUEST_NO_ERROR)
			{
				HxLOG_Error("[local_cakppv_tdt_requestPurchaseHistory] caRequestDispose fail!!\n");
				hError = ERR_FAIL;
			}
		}
	}

	LEAVE_CRITICAL_PPVTDT;

	HxLOG_Info("[local_cakppv_tdt_requestPurchaseHistory] EXIT\n");

	return hError;
}



static void local_cakppv_tdt_purchaseHistoryCallback(TCaRequest* pxRequest)
{
	TSignedInt32		nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Info("[local_cakppv_tdt_purchaseHistoryCallback] ENTER\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PURCHASE_HISTORY_CALL_BACK;
	stMsg.hWnd = (TUnsignedInt32)pxRequest;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if(nErrCode)
	{
		HxLOG_Error("[local_cakppv_tdt_purchaseHistoryCallback]  VK_PostMessage fail\n");
	}

	HxLOG_Info("[local_cakppv_tdt_purchaseHistoryCallback] EXIT\n");
}




static HERROR					local_cakppv_tdt_buildPurchaseHistory(TCaRequest*  pxRequest)
{
	HERROR					hError = ERR_FAIL;
	TCaRequest				*pRequest = NULL;
	TCaRequestStatus 			reqStatus;
	TCaObjectStatus 			objStatus;
	TCaPurchase 				**ppPurchase = NULL;
	TCaProcessingStatus 		procStatus;
	TUnsignedInt16 			usFirstIndex = 0;
	TUnsignedInt16 			usSecondIndex = 0;
	PPV_PURCHASE_LIST		*pPurchaseManager = NULL;
	TUnsignedInt32 			ulIndex = 0;
	TUnsignedInt32 			ulNumOfCredit = 0;
	TUnsignedInt8				*pName = NULL;
	TCaProduct				*pProduct = NULL;
	TSize		 			length;
	NA_PPV_StateInfo_t		*pTdtPpvStateInfo = NULL;
	HUINT32					ulNumOfServiec = 0;
	TCaService				**ppServices = NULL;
	HUINT32					ulSvcIndex = 0;


	HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory] Enter (%x)\n", pxRequest);
	ENTER_CRITICAL_PPVTDT;

	pPurchaseManager = local_cakppv_tdt_getPurchaseHistory();

/**************************************************************************************
  *
  * check product loaded
  *
  **************************************************************************************/
	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();

/**************************************************************************************
  *
  * get purchase history
  *
  **************************************************************************************/
	if(pxRequest != NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] (pxRequest != NULL)!!\n");
		if(pxRequest != pTdtPpvStateInfo->pPurchaseHistoryRequest)
		{
			HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] (pxRequest != pTdtPpvStateInfo->pPurchaseHistoryRequest)!!\n");
			pTdtPpvStateInfo->pPurchaseHistoryRequest = pxRequest;
		}
	}
	pRequest = pTdtPpvStateInfo->pPurchaseHistoryRequest;

	HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory] pRequest(%x)\n", pRequest);
#if 1
	reqStatus = caRequestGetProcessingStatus(pRequest, &procStatus);
	if (reqStatus != CA_REQUEST_NO_ERROR || procStatus != CA_PROCESSING_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] caRequestGetProcessingStatus fail!!\n");
		HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] reqStatus(%x)procStatus(%x)\n", reqStatus, procStatus);
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] procStatus(%x)\n", procStatus);
#endif
	reqStatus = caRequestGetObjects(pRequest, &ulNumOfCredit, &ppPurchase);
	if (reqStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] caRequestGetObjects fail(%d)!!\n", reqStatus);
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	HxLOG_Info("purchase number = %d, reqStatus(%d)\n", ulNumOfCredit, reqStatus);

	if(ulNumOfCredit == 0)
	{
		hError = ERR_OK;
	}
	else if(ulNumOfCredit > 0)
	{
		pPurchaseManager->pList = (PPV_PURCHASE_LIST_ITEM *)OxCAS_Malloc(ulNumOfCredit*sizeof(PPV_PURCHASE_LIST_ITEM));
		if(pPurchaseManager->pList == NULL)
		{
			HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] OxCAS_Malloc fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
		HxSTD_memset(pPurchaseManager->pList, 0x00, ulNumOfCredit*sizeof(PPV_PURCHASE_LIST_ITEM));

		pPurchaseManager->xNbItems = ulNumOfCredit;

		for(ulIndex = 0; ulIndex < ulNumOfCredit; ulIndex++)
		{
			pProduct = NULL;
			objStatus = caPurchaseGetProduct(ppPurchase[ulIndex], &pProduct);
			if (objStatus != CA_OBJECT_NO_ERROR || pProduct == NULL)
			{
				HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] caPurchaseGetProduct fail!!\n");
				hError = ERR_FAIL;
				goto EXIT_FUNC;
			}
			HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory] pProduct(%x)!!\n", pProduct);

			objStatus = caProductGetId(pProduct, &(pPurchaseManager->pList[ulIndex].ProductID));
			if (objStatus != CA_OBJECT_NO_ERROR)
			{
				pPurchaseManager->pList[ulIndex].ProductID = 0;
			}
			HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory][%d] ProductID(%d), objStatus(%d)\n", ulIndex, pPurchaseManager->pList[ulIndex].ProductID, objStatus);

#if			1
			ppServices = NULL;
			ulNumOfServiec = 0;
			objStatus = caProductGetServices(pProduct, (TUnsignedInt32*)&ulNumOfServiec, &ppServices);
			if (objStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] caProductGetServices fail!!\n");
			}
			if(ppServices)
			{
				TSize		nSize = 0;
				HUINT8		*pucProviderName = NULL;

				HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory] Enter Service!!\n");
				for(ulSvcIndex = 0; ulSvcIndex < ulNumOfServiec; ulSvcIndex++)
				{
					nSize = 0;
					pucProviderName = NULL;
					objStatus = caServiceGetProviderName(ppServices[ulSvcIndex], &nSize, &pucProviderName);
					if (objStatus != CA_OBJECT_NO_ERROR)
					{
						HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] caServiceGetProviderName fail!!\n");
						pPurchaseManager->providerName[0] = 0;
					}
					else
					{
						HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory] pucProviderName(%s)!!\n", pucProviderName);
						MWC_UTIL_DvbStrncpy(pPurchaseManager->providerName, pucProviderName, MAX_PPV_PURCHASE_LIST_NAME_LENGTH);
					}
				}
			}
#endif

			// purchase history도 product name을 사용
			pName = NULL;
			length = 0;
			objStatus = caProductGetName(pProduct, &length, &pName);
			if( objStatus== CA_OBJECT_NO_ERROR && pName != NULL)
			{
				pPurchaseManager->pList[ulIndex].EvtNameLength = length;
				//caProductGetName CA_OBJECT_NO_ERROR && purchase_list.pList[ulIndex].EvtNameLength > 0
				if (pPurchaseManager->pList[ulIndex].EvtNameLength  <= MAX_PPV_PURCHASE_LIST_NAME_LENGTH)
				{
					MWC_UTIL_DvbStrncpy(pPurchaseManager->pList[ulIndex].EvtName, pName, pPurchaseManager->pList[ulIndex].EvtNameLength);
				}
				else
				{
					pPurchaseManager->pList[ulIndex].EvtNameLength = MAX_PPV_PURCHASE_LIST_NAME_LENGTH;
					MWC_UTIL_DvbStrncpy(pPurchaseManager->pList[ulIndex].EvtName, pName, MAX_PPV_PURCHASE_LIST_NAME_LENGTH);
				}
				HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory] objStatus(%d) pName(%s)\n", objStatus, pName);
			}
			else
			{
				/**************************************************************
				 *
				 * Conditional Access Kernel Abertis Implementation Guidelines
				 * 2.3 purchase history
				 *
				 ***************************************************************/
				if(pTdtPpvStateInfo->ePeriodType & eNA_PPV_Period_Tdt)
				{
					pPurchaseManager->pList[ulIndex].EvtNameLength = MAX_PPV_PURCHASE_LIST_NAME_LENGTH;
					sprintf(pPurchaseManager->pList[ulIndex].EvtName, "Package %d", ulIndex);
					HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory] (%s)\n", pPurchaseManager->pList[ulIndex].EvtName);
					HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory] caProductGetName fail!! objStatus(%d) length(%d)\n", objStatus, length);
				}
				else
				{
					pTdtPpvStateInfo->ePeriodType = eNA_PPV_Period_Others;
				}
			}
			//error 가 나는 대로 그냥 전달.
			reqStatus = caPurchaseGetExpirationDate(ppPurchase[ulIndex], &(pPurchaseManager->pList[ulIndex].ExpirationDate));
			if (reqStatus != CA_REQUEST_NO_ERROR)
			{
				HxSTD_memset(&(pPurchaseManager->pList[ulIndex].ExpirationDate), 0x00, sizeof(TCalendarTime));
			}
			reqStatus = caPurchaseGetDate(ppPurchase[ulIndex], &(pPurchaseManager->pList[ulIndex].PurchaseTime));
			if (reqStatus != CA_REQUEST_NO_ERROR)
			{
				HxSTD_memset(&(pPurchaseManager->pList[ulIndex].PurchaseTime), 0x00, sizeof(TCalendarTime));
			}
			reqStatus = caPurchaseGetCost(ppPurchase[ulIndex], &(pPurchaseManager->pList[ulIndex].Cost));
			if (reqStatus != CA_REQUEST_NO_ERROR)
			{
				HxSTD_memset(&(pPurchaseManager->pList[ulIndex].Cost), 0x00, sizeof(TCreditAmount));
			}
			reqStatus = caPurchaseGetFlags(ppPurchase[ulIndex], &(pPurchaseManager->pList[ulIndex].PurchaseFlag));
			if (reqStatus != CA_REQUEST_NO_ERROR)
			{
				HxSTD_memset(&(pPurchaseManager->pList[ulIndex].PurchaseFlag), 0x00, sizeof(TPurchaseFlags));
			}
		}

		pTdtPpvStateInfo->eBuildState |= (eNA_PPV_BuildState_PurchaseHistoryComplete);

/**************************************************************************************
  *
  * cancel timer
  *
  **************************************************************************************/
		if(pTdtPpvStateInfo->ulPurchaseHistoryTimer != 0)
		{
			VK_TIMER_Cancel(pTdtPpvStateInfo->ulPurchaseHistoryTimer);
			pTdtPpvStateInfo->ulPurchaseHistoryTimer = 0;
			pTdtPpvStateInfo->ulPurchaseHistoryCounter = 0;
		}
	}
	hError = ERR_OK;

EXIT_FUNC :
	if(hError == ERR_OK)
	{
		if(pRequest)
		{
			reqStatus = caRequestDispose(pRequest);
			if(reqStatus != CA_REQUEST_NO_ERROR)
			{
				HxLOG_Error("[local_cakppv_tdt_buildPurchaseHistory] caRequestDispose fail!!\n");
				hError = ERR_FAIL;
			}
			pTdtPpvStateInfo->pPurchaseHistoryRequest = NULL;
		}
	}


	LEAVE_CRITICAL_PPVTDT;

	HxLOG_Info("[local_cakppv_tdt_buildPurchaseHistory] EXIT\n");

	return hError;
}


#define ____SECTIONLIST_CALLBACK____
static void				local_cakppv_tdt_freeSection(void *pvContents)
{
	HUINT8				*pucSection = (HUINT8*)pvContents;

	if(pucSection == NULL)
	{
		HxLOG_Error("[CA_NA_PpvStartup] pucSection == NULL!!\n");
		goto EXIT_FUNC;
	}
	OxCAS_Free(pucSection);

EXIT_FUNC :

	return;
}



#define ____LOCAL_AP_INTERFACE____
static HERROR				local_cakppv_tdt_getPPVStateInfo(NA_PPV_State_t *peState, HBOOL *pbNagraCaSys)
{
	HERROR				hError = ERR_FAIL;
	NA_PPV_StateInfo_t	*pTdtPpvStateInfo = NULL;
	NA_PPV_State_t		eState = eNA_PPV_State_Waiting;

	HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] Enter!!\n");

	ENTER_CRITICAL_PPVTDT;
	*pbNagraCaSys = FALSE;

	pTdtPpvStateInfo = local_cakppv_tdt_getPpvStateInfo();

	HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] eSignalingState(%x)!!\n", pTdtPpvStateInfo->eSignalingState);
	if(pTdtPpvStateInfo->eSignalingState == 0)
	{
		HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] 1!!\n");
		eState = eNA_PPV_State_Waiting;
	}
	else if(	pTdtPpvStateInfo->eSignalingState & eNA_PPV_SignalingState_Fail
			|| pTdtPpvStateInfo->eSignalingState & eNA_PPV_SignalingState_NitTimeout
			|| pTdtPpvStateInfo->eSignalingState & eNA_PPV_SignalingState_PurchaseHistoryTimeout
			|| pTdtPpvStateInfo->eSignalingState & eNA_PPV_SignalingState_PackageListTimeout )
	{
		HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] 2 (%x)!!\n", pTdtPpvStateInfo->eSignalingState);
		eState = eNA_PPV_State_Fail;
	}
	else
	{
		if(pTdtPpvStateInfo->eSignalingState & eNA_PPV_SignalingState_NitWating)
		{
			HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] 3!!\n");
			eState = eNA_PPV_State_Waiting;
		}
		if(	pTdtPpvStateInfo->eSignalingState & eNA_PPV_SignalingState_RequestPackageList
			|| pTdtPpvStateInfo->eSignalingState & eNA_PPV_SignalingState_RequestPurchaseHistory)
		{
			HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] 4!!\n");
			eState = eNA_PPV_State_Waiting;
		}

		if(pTdtPpvStateInfo->ePeriodType & eNA_PPV_Period_Others)
		{
			HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] 5!!\n");
			eState = eNA_PPV_State_OutOfArea;
		}

		if(	pTdtPpvStateInfo->eLoadState & eNA_PPV_LoadState_ProductLoaded)
		{
			eState = 0;
			if(	pTdtPpvStateInfo->eBuildState & eNA_PPV_BuildState_PurchaseHistoryComplete)
			{
				HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] 6!!\n");
				eState |= eNA_PPV_State_PurchaseHistoryComplete;
			}
			if(	pTdtPpvStateInfo->eBuildState & eNA_PPV_BuildState_PackageListComplete)
			{
				HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] 7!!\n");
				eState |= eNA_PPV_State_PackageListComplete;
			}
		}
		else if(pTdtPpvStateInfo->eLoadState & eNA_PPV_LoadState_ProductLoading)
		{
			HxLOG_Info("[local_cakppv_tdt_getPPVStateInfo] 8!!\n");
			eState = eNA_PPV_State_Waiting;
		}
	}

	*peState = eState;
	*pbNagraCaSys = pTdtPpvStateInfo->bNagraCaSysId;

	hError = ERR_OK;

EXIT_FUNC :
	LEAVE_CRITICAL_PPVTDT;

	return hError;
}

static HERROR				local_cakppv_tdt_getProductList(HUINT32 *pulNumOfProducts, PPV_PACKAGE_LIST_ITEM **ppPackageList)
{
	HERROR					hError = ERR_FAIL;
	PPV_PACKAGE_LIST		*pPackageList = NULL;
	PPV_PACKAGE_LIST_ITEM	*pPackageItems = NULL;
	PPV_PACKAGE_LIST_ITEM	*pTempItem = NULL;
	HUINT32					ulIndex = 0;


	HxLOG_Info("[local_cakppv_tdt_getProductList] ENTER!!\n");
	ENTER_CRITICAL_PPVTDT;


	if( pulNumOfProducts == NULL || ppPackageList == NULL )
	{
		HxLOG_Error("[local_cakppv_tdt_getProductList] param fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	*pulNumOfProducts = 0;
	*ppPackageList = NULL;

	pPackageList = local_cakppv_tdt_getPackageList();
	if( !pPackageList->pList || pPackageList->xNbItems == 0)
	{
		HxLOG_Error("[local_cakppv_tdt_getProductList] local_cakppv_tdt_getPackageList fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	pPackageItems = (PPV_PACKAGE_LIST_ITEM*)OxCAS_Malloc(sizeof(PPV_PACKAGE_LIST_ITEM)*pPackageList->xNbItems);
	if(pPackageItems == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_getProductList] OxCAS_Malloc fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	HxSTD_memset(pPackageItems, 0x00, sizeof(PPV_PACKAGE_LIST_ITEM)*pPackageList->xNbItems);

	pTempItem = pPackageList->pList;
	for(ulIndex = 0; ulIndex < pPackageList->xNbItems && pTempItem != NULL; ulIndex++)
	{
		HxSTD_memcpy(&pPackageItems[ulIndex], pTempItem, sizeof(PPV_PACKAGE_LIST_ITEM));
		pTempItem = pTempItem->pNext;
	}

	*pulNumOfProducts = (HUINT32)pPackageList->xNbItems;
	*ppPackageList = pPackageItems;

	hError = ERR_OK;

EXIT_FUNC :
	LEAVE_CRITICAL_PPVTDT;

	return hError;
}



static HERROR				local_cakppv_tdt_getPurchaseHistoryList(HUINT32 *pulNumOfPurchase, PPV_PURCHASE_LIST_ITEM **ppPurchaseList)
{
	HERROR					hError = ERR_FAIL;
	PPV_PURCHASE_LIST		*pPurchaseHistory = NULL;
	PPV_PURCHASE_LIST_ITEM	*pPurchaseItems = NULL;

	HxLOG_Info("[local_cakppv_tdt_getPurchaseHistoryList] ENTER!!\n");

	ENTER_CRITICAL_PPVTDT;

	if( pulNumOfPurchase == NULL || ppPurchaseList == NULL )
	{
		HxLOG_Error("[local_cakppv_tdt_getPurchaseHistoryList] param fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	*pulNumOfPurchase = 0;
	*ppPurchaseList = NULL;

	pPurchaseHistory = local_cakppv_tdt_getPurchaseHistory();
	if( !pPurchaseHistory->pList || pPurchaseHistory->xNbItems == 0)
	{
		HxLOG_Error("[local_cakppv_tdt_getPurchaseHistoryList] local_cakppv_tdt_getPurchaseHistory fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	pPurchaseItems = (PPV_PURCHASE_LIST_ITEM*)OxCAS_Malloc(sizeof(PPV_PURCHASE_LIST_ITEM)*pPurchaseHistory->xNbItems);
	if(pPurchaseItems == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_getPurchaseHistoryList] OxCAS_Malloc fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	HxSTD_memset(pPurchaseItems, 0x00, sizeof(PPV_PURCHASE_LIST_ITEM)*pPurchaseHistory->xNbItems);

	HxSTD_memcpy(pPurchaseItems, pPurchaseHistory->pList, sizeof(PPV_PURCHASE_LIST_ITEM)*pPurchaseHistory->xNbItems);

	*pulNumOfPurchase = (HUINT32)pPurchaseHistory->xNbItems;
	*ppPurchaseList = pPurchaseItems;

	hError = ERR_OK;

EXIT_FUNC :

	LEAVE_CRITICAL_PPVTDT;

	return hError;
}



static HERROR				local_cakppv_tdt_getProviderName(HUINT32 *pulSize,HUINT8 **ppucProviderName)
{
	HERROR					hError = ERR_FAIL;
	PPV_PURCHASE_LIST		*pPurchaseHistory = NULL;
	HUINT32					ulLen = 0;

	HxLOG_Info("[local_cakppv_tdt_getProviderName] ENTER!!\n");
	ENTER_CRITICAL_PPVTDT;

	if(pulSize == NULL ||  ppucProviderName == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_getProviderName] param fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	*ppucProviderName = NULL;
	*pulSize = 0;


	pPurchaseHistory = local_cakppv_tdt_getPurchaseHistory();
	if(!pPurchaseHistory->pList || pPurchaseHistory->xNbItems == 0)
	{
		HxLOG_Error("[local_cakppv_tdt_getProviderName] local_cakppv_tdt_getPurchaseHistory fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	ulLen = svc_cas_MgrUtilStrLen(pPurchaseHistory->providerName);
	if(ulLen == 0)
	{
		HxLOG_Error("[local_cakppv_tdt_getProviderName] ulLen == 0 !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	*ppucProviderName = (HUINT8*)OxCAS_Malloc(sizeof(HUINT8)*(ulLen+1));
	if(*ppucProviderName == NULL)
	{
		HxLOG_Error("[local_cakppv_tdt_getProviderName] OxCAS_Malloc fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	HxSTD_memset(*ppucProviderName, 0x00, sizeof(HUINT8)*(ulLen+1));

	MWC_UTIL_DvbStrcpy(*ppucProviderName, pPurchaseHistory->providerName);
	*pulSize = ulLen;

	hError = ERR_OK;

EXIT_FUNC :

	LEAVE_CRITICAL_PPVTDT;

	return hError;
}


static HERROR				local_cakppv_tdt_releaseProductList(PPV_PACKAGE_LIST_ITEM *pPackageList)
{
	HERROR					hError = ERR_FAIL;

	HxLOG_Info("[local_cakppv_tdt_releaseProductList] ENTER!!\n");

	if(pPackageList == NULL )
	{
		HxLOG_Error("[local_cakppv_tdt_releaseProductList] param fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	OxCAS_Free(pPackageList);

	hError = ERR_OK;

EXIT_FUNC :

	return hError;
}


static HERROR				local_cakppv_tdt_releasePurchaseHistoryList(PPV_PURCHASE_LIST_ITEM *pPurchaseList)
{
	HERROR					hError = ERR_FAIL;

	HxLOG_Info("[local_cakppv_tdt_releasePurchaseHistoryList] ENTER!!\n");

	if(pPurchaseList == NULL )
	{
		HxLOG_Error("[local_cakppv_tdt_releasePurchaseHistoryList] param fail !!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	OxCAS_Free(pPurchaseList);


	hError = ERR_OK;

EXIT_FUNC :

	return hError;
}



#define ____TDT_PPV_GLOVAL_FUNC____
TUnsignedInt32	CA_NA_GetTdtPpvMsgQueueId(void)
{
	return s_ulTdtPpvQueueId;
}


HERROR					CA_NA_PpvNotifyTPChanged(TBoolean	bTpChanged)
{
/** nagar type */
/** humax type */
	HERROR 				hError = ERR_OK;
	NaCasMsg_t			stMsg = {0, };

	if(!bTpChanged)
	{
		HxLOG_Info("[CA_NA_PpvNotifyTPChanged] !bTpChanged!!\n");
		hError = ERR_OK;
		goto EXIT_FUNC;
	}

	HxLOG_Info("[CA_NA_PpvNotifyTPChanged] ENTER!!\n");

	stMsg.wMsg = NA_SERVICE_ID;
	stMsg.lParam2 = CAK_SERVICE_CHANGE_TP;

	hError = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if (hError != ERR_OK)
	{
		HxLOG_Error("[CA_NA_PpvNotifyTPChanged] VK_PostMessage fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = ERR_OK;

EXIT_FUNC :

	return hError;
}



void						CA_NA_CakReceivedNit(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucNit)
{
/** nagar type */
/** humax type */
	HERROR 				hError = ERR_OK;
	NaCasMsg_t			stMsg = {0, };
	HUINT16	 			usSize = 0;
	DbSvc_TsInfo_t			stTsInfo;
	HUINT8				*pucNitSection = NULL;

	HxLOG_Info("[CA_NA_CakReceivedNit] ENTER!!\n");

	if(pucNit == NULL)
	{
		HxLOG_Error("CA_NA_CakReceivedNit ERROR for given usUniqueId, local_cakppv_tdt_SetNITArrivedFlag(TRUE)!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = DB_SVC_GetTsInfoByUniqueId(usUniqueId, &stTsInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[CA_NA_CakReceivedNit] DB_SVC_GetTsInfoByUniqueId fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	usSize = (TUnsignedInt16)get12bit(pucNit+1)+3;	// pucNit rawdata total usSize

	pucNitSection = (TUnsignedInt8*)OxCAS_Malloc(usSize);
	if(pucNitSection == NULL)
	{
		HxLOG_Error("[CA_NA_CakReceivedNit] OxCAS_Malloc fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	HxSTD_memcpy(pucNitSection, pucNit, usSize);

	stMsg.wMsg = NA_FILTER_ID;
	stMsg.lParam2 = CAK_FILTER_NIT_ARRIVAL;
	stMsg.lParam3 = stTsInfo.usOnId;
	stMsg.hWnd = (TUnsignedInt8*)pucNitSection;

	hError = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	if(hError != ERR_OK)
	{
		HxLOG_Error("[CA_NA_CakReceivedNit] VK_PostMessage fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	hError = ERR_OK;

EXIT_FUNC :
	if(pucNit != NULL)
	{
		OxCAS_Free(pucNit);
		pucNit = NULL;
	}

	return;
}

void				CA_NA_CakReceivedEit(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucEitP, TUnsignedInt8 *pucEitF)
{
	return;
}

void				CA_NA_CakReceivedTDT(void)
{
	return;
}



HERROR			CA_NA_PpvStartup(void)
{
	HERROR		hError = ERR_FAIL;

	hError = VK_CreateMessageQueue(	256,
									&s_ulTdtPpvQueueId,
									NA_PPV_MSG_Q_NAME,
									sizeof(NaCasMsg_t));
	if (hError != ERR_OK)
	{
		HxLOG_Error("[CA_NA_PpvStartup] VK_PostMessage fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = VK_TASK_Create(	local_cakppv_tdt_Task,
							NA_CAK_TASK_PRIORITY,
							NA_CAK_TASK_STACK_SIZE,
							NA_PPV_TASK_NAME, NULL,
							&s_ulPpvTaskId,
							0);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[CA_NA_PpvStartup] VK_PostMessage fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = VK_TASK_Start(s_ulPpvTaskId);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[CA_NA_PpvStartup] VK_PostMessage fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
#if 1
	hError = VK_SEM_Create(&s_ulPpvDataSemId, "s_ulPpvDataSemId", VK_SUSPENDTYPE_FIFO);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[CA_NA_PpvStartup] VK_PostMessage fail!!\n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
#endif

	hError = ERR_OK;

EXIT_FUNC :

	return hError;
}



TCaStatus			CA_NA_PpvRequestCardCredit(void)
{
	NaCasMsg_t 		stMsg = {0, };
	TSignedInt32		nErrCode = 0;

	HxLOG_Info("[CA_NA_PpvRequestCardCredit] ENTER!!\n");

	 stMsg.wMsg = NA_CAK_ID;
	 stMsg.lParam2 = CAK_CREDIT_NOTIFICATION;

	 nErrCode = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	 if(nErrCode != ERR_OK)
	{
		HxLOG_Error("[CA_NA_PpvRequestCardCredit] VK_PostMessage fail!!\n");
		return CA_ERROR;
	}

	 return CA_NO_ERROR;
}



TCaStatus			CA_NA_PpvProductLoadingListener(void)
{
	NaCasMsg_t 		stMsg = {0, };
	TSignedInt32		nErrCode = 0;

	HxLOG_Info("[CA_NA_PpvProductLoadingListener] ENTER!!\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_LOADING_LISTENER;

	nErrCode = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	 if(nErrCode != ERR_OK)
	{
		HxLOG_Error("[CA_NA_PpvProductLoadingListener] VK_PostMessage fail!!\n");
		return CA_ERROR;
	}

	 return CA_NO_ERROR;
}

TCaStatus			CA_NA_PpvProductLoadedListener(void)
{
	NaCasMsg_t 		stMsg = {0, };
	TSignedInt32		nErrCode = 0;

	HxLOG_Info("[CA_NA_PpvProductLoadedListener] ENTER!!\n");

	 stMsg.wMsg = NA_CAK_ID;
	 stMsg.lParam2 = CAK_PPV_LOADED_LISTENER;

	 nErrCode = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	 if (nErrCode != ERR_OK)
	 {
		HxLOG_Error("[CA_NA_PpvProductLoadedListener] VK_PostMessage fail!!\n");
		return CA_ERROR;
	 }
	 return CA_NO_ERROR;
}


TCaStatus			CA_NA_PpvPurchaseHistoryListener(void)
{
	NaCasMsg_t 		stMsg = {0, };
	TSignedInt32		nErrCode = 0;

	HxLOG_Info("[CA_NA_PpvPurchaseHistoryListener] ENTER!!\n");

	 stMsg.wMsg = NA_CAK_ID;
	 stMsg.lParam2 = CAK_PPV_PURCHASEHISTORY_LISTENER;

	 nErrCode = VK_PostMessage(s_ulTdtPpvQueueId, (void *)&stMsg, sizeof(NaCasMsg_t));
	 if (nErrCode != ERR_OK)
	 {
		HxLOG_Error("[CA_NA_PpvPurchaseHistoryListener] VK_PostMessage fail!!\n");
		return CA_ERROR;
	 }
	 return CA_NO_ERROR;
}



#define ____GLOBAL_AP_INTERFACE____
HERROR				CA_NA_GetPPVStateInfo(NA_PPV_State_t *peState, HBOOL *pbNagraCaSys)
{
	return local_cakppv_tdt_getPPVStateInfo(peState, pbNagraCaSys);
}

HERROR				CA_NA_GetPackageList(HUINT32 *pulNumOfProducts, PPV_PACKAGE_LIST_ITEM **ppPackageList)
{
	return local_cakppv_tdt_getProductList(pulNumOfProducts, ppPackageList);
}

HERROR				CA_NA_GetPurchaseHistoryList(HUINT32 *pulNumOfPurchase, PPV_PURCHASE_LIST_ITEM **ppPurchaseList)
{
	return local_cakppv_tdt_getPurchaseHistoryList(pulNumOfPurchase, ppPurchaseList);
}

HERROR				CA_NA_GetProviderName(HUINT32 *pulSize,HUINT8 **ppucProviderName)
{
	return local_cakppv_tdt_getProviderName(pulSize, ppucProviderName);
}

HERROR				CA_NA_ReleaseProductList(PPV_PACKAGE_LIST_ITEM *pPackageList)
{
	return local_cakppv_tdt_releaseProductList(pPackageList);
}

HERROR				CA_NA_ReleasePurchaseHistoryList(PPV_PURCHASE_LIST_ITEM *pPurchaseList)
{
	return local_cakppv_tdt_releasePurchaseHistoryList(pPurchaseList);
}

HERROR				CA_NA_ReleaseProviderName(HUINT8 *pucProviderName)
{
	return ERR_OK;
//	return local_cakppv_tdt_releaseProviderName(pPurchaseList);
}




//#define CONFIG_NA_PPV_DUMMY_FUNC
#if 0//defined(CONFIG_NA_PPV_DUMMY_FUNC)
HERROR			CA_NA_PpvRegisterPackageListNotifyCallback(TPackageInfoNotifyCallback callback)
{
	return ERR_OK;
}



HERROR			CA_NA_PpvRegisterScCreditCallback(TCardCreditCallback callback)
{

	return ERR_OK;
}


HERROR			CA_NA_PpvRegisterProductsLoadingCallback(TCaListenerArrivedNotifyCallback callback)
{
	return ERR_OK;
}



HERROR			CA_NA_PpvRegisterTSCheckNotifyCallback(TTSCheckNotifyCallback callback)
{
	return ERR_OK;
}

HERROR			CA_NA_PpvRegisterEventInfoNotifyCallback(TRequestEventNotifyCallback callback)
{
	return ERR_OK;
}



HERROR			CA_NA_PpvRegisterPurchaseHistoryNotifyCallback(TPurchaseHistoryNotifyCallback callback)
{

	return ERR_OK;
}


HERROR				CA_NA_PpvGetScCredit(TCreditAmount  *pCredit)
{
	return ERR_OK;
}

void					CA_NA_PpvSetScCredit(TCreditAmount  pCredit)
{
	return;
}

TCaStatus 			CA_NA_PpvRequestCardCredit(void)
{
	 return CA_NO_ERROR;
}

HERROR			CA_NA_PpvRegisterRechargeListNotifyCallback(TRechargeListNotifyCallback callback)
{
	return ERR_OK;
}


HERROR			CA_NA_PpvRegisterOppvPurchaseListNotifyCallback(TOPPVPurchaseListNotifyCallback callback)
{
	return ERR_OK;
}


HERROR			CA_NA_PpvRegisterRechargeAlarmNotifyCallback(TRechargeNotifyCallback callback)
{
	return ERR_OK;
}

HERROR			CA_NA_PpvRegisterRequestPurchaseEventNotifyCallback(TPurchaseEventNotifyCallback callback)
{
	return ERR_OK;
}

HERROR			CA_NA_PpvRegisterRequestPurchasePackageNotifyCallback(TPurchasePackageNotifyCallback callback)
{
	return ERR_OK;
}


TCaStatus			CA_NA_PpvRequestTsCheck(Handle_t hSvcHandle)
{
	 return CA_NO_ERROR;
}


HERROR				CA_NA_PpvRequestEventInfo(void)
{
	return ERR_OK;
}


HERROR				CA_NA_PpvRequestRechargeList(void)
{
	return ERR_OK;
}


HERROR				CA_NA_PpvRequestNewRecharge(void)
{
	return ERR_OK;
}

HERROR				CA_NA_PpvRequestNewPurchase(TCaExportation* 	pxExportation)
{
	return ERR_OK;
}


HERROR				CA_NA_PpvRequestEventPurchase(TPurchasableEventType xStatus, TCaRequest* pRequest, TCaEvent *pEvent, TCaProduct *pProduct)
{
	return ERR_OK;
}


HERROR				CA_NA_PpvRequestPackagePurchase(TCaProduct* pProduct)
{
	return ERR_OK;

}

HERROR					CA_NA_PpvRequestProductPurchasable(TCaRequest* pRequest, TCaProduct*  pProduct)
{
	return ERR_OK;

}

HERROR					CA_NA_PpvGetProductGetDescription(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer)
{
	return ERR_OK;
}

HERROR					CA_NA_PpvGetProductGetDisclaimer(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer)
{
	return ERR_OK;
}

HERROR					CA_NA_PpvGetPackageEventInfo(TCaRequest* pRequest, TCaProduct* pPackage, PACKAGE_INFO **pPackageInfo)
{
	return ERR_OK;
}
#endif


#endif
