//**************************************************************************************************/
#define ________CA_CAK_SVC_Private_Include_____________________________________________________
/**************************************************************************************************/
#include <stdarg.h>
#include "vkernel.h"
#include "na_glob.h"

#include "util.h"
//#include <pal_pipe.h>
#include "_svc_cas_mgr_common.h"
#include <_svc_cas_sub_svc_api.h>

#include <db_svc.h>
#include "namgr_main.h"


/**************************************************************************************************/
#define ________CA_CAK_SVC_Golbal_Value________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_SVC_Private_Define_______________________________________________________
/**************************************************************************************************/
#if (CONFIG_FOR_DAL_TEST_DEF == 1)
#define CONFIG_S_DSMCC						0
#else
#define CONFIG_S_DSMCC						1
#endif

#define NORMAL_PLAY_MODE					0
#define PAUSE_PLAY_MODE 					1

#define ASSERT_VALID_HANDLE(x)				!(x)

#define BUF_SIZE_1K							(1024)
#define QUEUE_SIZE							sizeof(NaCasMsg_t)

#define WAIT_FOR_SEM(x)					VK_SEM_Get(x);
#define RELEASE_SEM(x)						VK_SEM_Release(x);

#define VK_CreateMessageQueue(a, b)			VK_MSG_Create(a, QUEUE_SIZE, NA_PSI_MSG_Q_NAME, b, VK_SUSPENDTYPE_PRIORITY)
#define VK_PostMessage(a, b) 					VK_MSG_Send(a, b, QUEUE_SIZE)
#define VK_GetMessage(a, b)					VK_MSG_Receive(a, b, QUEUE_SIZE)
#define VK_WaitGetMessage(a, b, c)			VK_MSG_ReceiveTimeout(a, b, QUEUE_SIZE, c)

#if defined(GET_ACTION_ID)
#undef GET_ACTION_ID
#define GET_ACTION_ID(x)					(svc_cas_SubSvcRmGetActionId(x))
#endif

#define CHECK_SAME_HANDLE(x, y)				(x == y)
#define CHECK_SAME_ACTION_ID(x, y)			(svc_cas_SubSvcRmGetActionId(x) == svc_cas_SubSvcRmGetActionId(y))

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
// DLK_CAK_DevGuide_1.1.3 (작업일자 기준) - PARENTAL CONTROL chapter 참조
#define DESCRAMBLE_CA_PARENTAL_CONTROL_THRESHOLD	15
#define DESCRAMBLE_CA_PARENTAL_CONTROL_BITMAP		0
#endif

/**************************************************************************************************/
#define ________CA_CAK_SVC_Private_Type_Define_________________________________________________
/**************************************************************************************************/
typedef enum PSI_CA_STREAM_TYPE_ENUM
{
	eClearService = 0,
	eScrambleService,
	eUnknownService,
	eCaStreamType_Max
} PSI_CA_STREAM_TYPE_t;

typedef struct PSI_CAT_INFO_STRUCT
{
	TUnsignedInt32			hAction;
	TBoolean				bIsAvailable;
	TUnsignedInt16			usUniqueId;
	TCaRequest				*pxEmmFliteringRequest;
} PSI_CAT_INFO_t;

typedef struct PSI_PMT_INFO_STRUCT
{
	TUnsignedInt8				ucNumOfElem;
	EsType_t					eEsType[MAX_NUM_OF_SERVICE_PID];
	TMpegStreamType 			ucStreamType[MAX_NUM_OF_SERVICE_PID];
	TUnsignedInt16			usEsPid[MAX_NUM_OF_SERVICE_PID];
	TBoolean					bScrambleFlag[MAX_NUM_OF_SERVICE_PID];
	TMpegStreamType 			eStreamTypeDetail[MAX_NUM_OF_SERVICE_PID];
} PSI_PMT_INFO_t;

typedef struct PSI_CAS_SERVICE_INFO_STRUCT
{
	Handle_t					hAction;
	Handle_t					hSvcHandle;
	SvcCas_ActionType_e			eActType;

	TBoolean					bIsAvailable;
	TUnsignedInt8				ucPlayMode;
	TBoolean					bIsSvcStarted;

	TTransportSessionId			xTsId;
	TCaSourceId					xSourceId;

	TCaRequest				*pxProgDesRequest;
} PSI_CA_SERVICE_INFO_t;


// TODO: add ca_psi7.c
typedef enum tagCaInstState
{
	eInstStartState = 0,
	eInstPauseState,
	eInstStopState
} CaInstState_t;

typedef struct tagEsPidList
{
	TUnsignedInt16 		usVideoPid;
	TUnsignedInt16		ausAudioPid[MAX_NUM_OF_AUDIO_TRACK];
	TUnsignedInt16 		usTeletextPid;
	TUnsignedInt16 		usSubtitlePid;
} CaEsPidList_t;

typedef struct tagCaPlayInst
{
	Handle_t			hAction;
	Handle_t			hSvc;
	SvcCas_ActionType_e	eActType;

	TUnsignedInt16		usSvcId;
	TUnsignedInt16		usUniqueId;
	CaEsPidList_t		stEsPidList;

	TBoolean			bGotPmt;
	TBoolean			bConnect;
	TUnsignedInt8		*pucPmt;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	PSI_CA_MAC_DESC_t	stCaMacDesc[MAX_NUM_OF_CA_MAC_DESC];		// Merlin Access Criteria Descriptor
#endif

	TCaAccess			eCaAccess;
	CaInstState_t		eInstState;
} CaPlayInst_t, *pCaPlayInst_t;

#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
typedef struct tagCaEmmInfo
{
	Handle_t			hAction;
	Handle_t			hSymAction;

	TBoolean			bSetEmm;
	TUnsignedInt16		usUniqueId;
} CaEmmInfo_t;
#endif

typedef struct tagCaNaInst
{
	Handle_t			hCasInst;
	TUnsignedInt32		ulSem;

#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	CaEmmInfo_t 		stCaEmmInfo[MAX_TUNER_NO];
#endif
	CaPlayInst_t		stCaPlayInstList[MAX_NO_SESSION];
	CaPlayInst_t		stCaLastLiveState;
} CaNaInst_t;


/**************************************************************************************************/
#define ________CA_CAK_SVC_Externel_Functions_Prototype_________________________________________
/**************************************************************************************************/
extern void CA_NA_PsiStartService(TUnsignedInt16 usSvcID, TUnsignedInt32 hAction, TUnsignedInt32 hSvc, SvcCas_ActionType_e eActType);
extern void CA_NA_PsiStopService(TUnsignedInt32	hAction, TUnsignedInt16 usSvcID);
extern void CA_NA_PsiReceivedPmt(TUnsignedInt32 hAction, TUnsignedInt8 *pucPmt);
extern void CA_NA_PsiStopEmm(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId);
extern void CA_NA_PsiSetService(TUnsignedInt8 ucElemType, TUnsignedInt16 usPid, TUnsignedInt32 hAction, TUnsignedInt8 ucStreamType);
extern void CA_NA_PsiRecievedCat(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucCat);
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
extern void CA_NA_PsiSetDsc(Handle_t hAction, CaEsPidList_t *pstEsPid);
#endif


/**************************************************************************************************/
#define ________CA_CAK_SVC_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static TUnsignedInt32 			s_ulPsiTaskId;
static TUnsignedInt32 			s_ulPsiSemId;
static TUnsignedInt32 			s_ulPsiQueueId;

static PSI_PMT_INFO_t			s_stPmtInfoList[MAX_NO_SESSION];
static PSI_PMT_INFO_t			*s_pstTestPmtInfo;

static PSI_CA_SERVICE_INFO_t 	s_stCaSvcInfoList[MAX_NO_SESSION];
static PSI_CA_SERVICE_INFO_t 	*s_pstTestCaSvcInfo;

static PSI_CAT_INFO_t			s_stCatInfoList[MAX_TUNER_NO];
static PSI_CAT_INFO_t 			*s_pstTestCatInfo;

static TUnsignedInt8			s_ucCatSection[BUF_SIZE_1K];
static TUnsignedInt8			s_ucPmtSection[BUF_SIZE_1K * 2];

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static PSI_CA_PMT_INFO_t		stPsiCaPmtInfo[MAX_NO_SESSION];
#endif

// TODO: add ca_psi7.c
static CaNaInst_t				s_stNaCaInst;


/**************************************************************************************************/
#define ________CA_CAK_SVC_Private_Static_Prototype______________________________________________
/**************************************************************************************************/
static void 					_naMgr_cakSvc_Task(void* arg);

static void  					_naMgr_cakSvc_DisposeRequest(TCaRequest* pRequest);
static TCaRequestStatus 		_naMgr_cakSvc_PgmDescCallback(TCaRequest* pxRequest, TCaExportation* pxExportation);

static void 					_naMgr_cakSvc_CatReceived(NaCasMsg_t *pstMsg);
static void 					_naMgr_cakSvc_PmtReceived(NaCasMsg_t *pstMsg);

static void 					_naMgr_cakSvc_StartService(NaCasMsg_t *pstMsg);
static void 					_naMgr_cakSvc_StopService(NaCasMsg_t *pstMsg);
static void 					_naMgr_cakSvc_StopEmm(NaCasMsg_t *pstMsg);
static void 					_naMgr_cakSvc_SetPidService(NaCasMsg_t *pstMsg);
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static void						_naMgr_cakSvc_SetDsc(NaCasMsg_t *pstMsg);
#endif

static HERROR 				_naMgr_cakSvc_CheckValidSvcInfoIdx(TUnsignedInt8 ucIndex);
static HERROR 				_naMgr_cakSvc_GetSvcInfoIdx(TUnsignedInt32 hAction, TUnsignedInt8 *pucIndex);
static HERROR 				_naMgr_cakSvc_FindSvcInfoIdxByAction(TUnsignedInt32 hAction, TUnsignedInt8 *pucIndex);
static PSI_PMT_INFO_t* 		_naMgr_cakSvc_GetPmtInfoPtrByIdx(TUnsignedInt8 ucIndex);
static PSI_CA_SERVICE_INFO_t* _naMgr_cakSvc_GetCasSvcInfoPtrByIdx(TUnsignedInt8 ucIndex);
static HERROR					_naMgr_cakSvc_MakeCaSvcInfoAvailableByIdx(TUnsignedInt8 ucIndex);

static PSI_CAT_INFO_t* 		_naMgr_cakSvc_FindCatInfoByUniqueId(TUnsignedInt16 usUniqueId);
static PSI_CAT_INFO_t* 		_naMgr_cakSvc_MakeCatInfoAvailable(void);

static HERROR 				_naMgr_cakSvc_InitGlobalInfo(void);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static SvcCas_DescPointList_t* 	_naMgr_cakSvc_GetDescPoint(TUnsignedInt8 *pucRawDesc, TUnsignedInt16 usRawDescLen, TUnsignedInt8 ucDescTag);
static void 				_naMgr_cakSvc_FreeDescPoint(SvcCas_DescPointList_t *pDescList);
static HERROR 				_naMgr_cakSvc_ParsePmt(Handle_t hAction, TUnsignedInt8 *pucPmtRaw);
static HERROR 				_naMgr_cakSvc_GetPmtInfo(Handle_t hAction, PSI_CA_PMT_INFO_t **ppstPmtInfo);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
static void					_naMgr_cakSvc_SetParentalControl(NaCasMsg_t *pstMsg);
#endif

static HERROR _naMgr_cakSvc_GetTable(Handle_t hAction, pCaPlayInst_t *ppstCaPlayInst);

/**************************************************************************************************/
#define ________CA_CAK_SVC_Private_Static_Prototype_Body_________________________________________
/**************************************************************************************************/
static void _naMgr_cakSvc_Task(void *arg)
{
	NaCasMsg_t stMsg;

	while(1)
	{
		if (VK_GetMessage(s_ulPsiQueueId, (void *)&stMsg) != ERR_OK)
		{
			continue;
		}

		switch(stMsg.wMsg)
		{
			case NA_CAK_ID:
				switch(stMsg.lParam2)
				{
					default:
						break;
				}
				break;

			case NA_FILTER_ID:
				switch(stMsg.lParam2)
				{
					case FILTER_PSI_CAT_ARRIVAL:
						_naMgr_cakSvc_CatReceived( &stMsg );
						break;

					case FILTER_PSI_PMT_ARRIVAL:
						_naMgr_cakSvc_PmtReceived( &stMsg );
						break;

					default:
						break;
				}
				break;

			case NA_SERVICE_ID:
				switch(stMsg.lParam2)
				{
					case SERVICE_PSI_START_SERVICE:
						_naMgr_cakSvc_StartService( &stMsg );
						break;

					case SERVICE_PSI_STOP_SERVICE:
						_naMgr_cakSvc_StopService( &stMsg );
						break;

					case SERVICE_PSI_STOP_EMM:
						_naMgr_cakSvc_StopEmm( &stMsg );
						break;

					case SERVICE_PSI_SET_SERVICE:
						_naMgr_cakSvc_SetPidService( &stMsg );
						break;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
					case SERVICE_PSI_SET_DSC:
#if !defined(CONFIG_MW_CAS_NAGRA_OP_SES)						
						_naMgr_cakSvc_SetDsc(&stMsg);
#endif
						break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
					case SERVICE_PSI_SET_PARENTAL_CONTROL:
						_naMgr_cakSvc_SetParentalControl(&stMsg);
						break;
#endif
					default:
						break;
				}
				break;

			default:
				break;
		}
	}
}


static void  _naMgr_cakSvc_DisposeRequest(TCaRequest* pRequest)
{
	if (pRequest)
	{
		TCaRequestStatus eObjStatus;

		HxLOG_Print("-> Dispose request = 0x%x\n", (TUnsignedInt32)pRequest);
		eObjStatus = caRequestDispose(pRequest);
		pRequest = NULL;
		HxLOG_Print("-> After caRequestDispose, objStatus = %d\n", eObjStatus);
	}
	else
	{
		HxLOG_Print("-> Dispose Request is NULL!!\n");
	}
}


static TCaRequestStatus _naMgr_cakSvc_PgmDescCallback(TCaRequest* pxRequest, TCaExportation* pxExportation)
{
#if (0)
	TCaObjectStatus 		eObjStatus;
	TUnsignedInt32 		i, ulNumOfPgm;
	TCaProgram			**ppCaProgram;
	TTransportSessionId	ulTsSessionId;
	TProgramNumber		usPgmNumber;
	TCaAccess			eCaAccess;
#endif
	if (pxExportation != NULL)
	{
#if (0)
		eObjStatus = caExportationGetObjects(pxExportation, &ulNumOfPgm, &ppCaProgram);
		if (eObjStatus == CA_OBJECT_NO_ERROR)
		{
			HxLOG_Print("Num of programs:%d\n", ulNumOfPgm);
			for (i=0; i<ulNumOfPgm; i++)
			{
				if (ppCaProgram[i] != NULL)
				{
					eObjStatus =  caProgramGetTransportSessionId(ppCaProgram[i], &ulTsSessionId);
					if (eObjStatus != CA_OBJECT_NO_ERROR)
					{
						HxLOG_Error("error in caProgramGetTransportSessionId.. (0x%x)\n", eObjStatus);
						continue;
					}

					eObjStatus = caProgramGetNumber(ppCaProgram[i], &usPgmNumber);
					if (eObjStatus != CA_OBJECT_NO_ERROR)
					{
						HxLOG_Error("error in caProgramGetNumber.. (0x%x)\n", eObjStatus);
						continue;
					}

					eObjStatus = caProgramGetAccess(ppCaProgram[i], &eCaAccess);
					if (eObjStatus != CA_OBJECT_NO_ERROR)
					{
						HxLOG_Error("error in caProgramGetAccess.. (0x%x)\n", eObjStatus);
						continue;
					}

					HxLOG_Print("TsSession(hAction):0x%x, Access:0x%x PgmNumber:0x%x\n",
											(unsigned int)ulTsSessionId, (unsigned int)eCaAccess, (unsigned int)usPgmNumber);

				}
			}
		}
#endif
		caExportationDispose(pxExportation);
	}

	return CA_REQUEST_NO_ERROR;
}

static void _naMgr_cakSvc_CatReceived(NaCasMsg_t *pstMsg)
{
	TUnsignedInt32 		hAction;
	TUnsignedInt16		usUniqueId;
	TUnsignedInt8 		*pucCat = NULL;
	TUnsignedInt16		usSizeOfSection = 0;
	TUnsignedInt16		usDesTotalLen = 0;

	TCaRequestStatus		eStatus = 0;
	TCaRequest			*pRequest = NULL;
	TCaProcessingStatus	eProcStatus = 0;
	TTransportSessionId	xTransportSessionId = 0;

	if (pstMsg == NULL)
	{
		HxLOG_Error("pstMsg is NULL\n");
		return;
	}

	hAction = pstMsg->hWnd;
	pucCat = (TUnsignedInt8*)pstMsg->lParam1;
	usUniqueId = pstMsg->lParam3;

	if (pucCat == NULL)
	{
		HxLOG_Error("pucCat is NULL, Action:0x%x\n", (unsigned int)hAction);
		return;
	}

	HxLOG_Print("Action:0x%x, usUniqueId:0x%x, pucCat:0x%x\n", (unsigned int)hAction, (unsigned int)usUniqueId, (unsigned int)pucCat);

	s_pstTestCatInfo = _naMgr_cakSvc_FindCatInfoByUniqueId(usUniqueId);
	if (s_pstTestCatInfo == NULL)
	{
		s_pstTestCatInfo = _naMgr_cakSvc_MakeCatInfoAvailable();
	}

	if (s_pstTestCatInfo == NULL)
	{
		HxLOG_Error("error in _naMgr_cakSvc_MakeCatInfoAvailable....\n");
		return;
	}

	HxSTD_memset(s_ucCatSection, 0x00, BUF_SIZE_1K);
	usSizeOfSection = get12bit(pucCat + 1) + 3;
	if (usSizeOfSection > BUF_SIZE_1K)
	{
		HxLOG_Error("Over of section size:%d\n", usSizeOfSection);
		OxCAS_Free(pucCat);
		return;
	}

	if ((usSizeOfSection - 3) > 9)
	{
		usDesTotalLen = ((usSizeOfSection - ( 5 + 4 )) - 3);
	}

 	HxLOG_Print("Section Size = %d, usDesTotalLen = %d\n", usSizeOfSection, usDesTotalLen);
	HxSTD_memcpy(s_ucCatSection, pucCat, usSizeOfSection);
	OxCAS_Free(pucCat);

	if (usDesTotalLen <= 0)
	{
		HxLOG_Error("CA_Descriptor size is zero\n");
		return;
	}


	// 1-Creat Program Descrambling Request
	pRequest = s_pstTestCatInfo->pxEmmFliteringRequest;
	if (pRequest == NULL)
	{
		HxLOG_Print("Before Created EMM Processing Request....\n");
		eStatus = caRequestCreate(CA_REQUEST_TYPE_EMM_FILTERING, &pRequest);
		if (eStatus != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("caRequestCreate(CA_REQUEST_TYPE_EMM_FILTERING) : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
			return;
		}

		// 2-Set TransportSessionId
		xTransportSessionId = hAction;
		eStatus = caRequestSetTransportSessionId(pRequest, xTransportSessionId);
		if (eStatus != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("caRequestSetTransportSessionId : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
			caRequestDispose(pRequest);
			pRequest = NULL;
			return;
		}
		HxLOG_Print("After Created EMM Processing Request.... pRequest(0x%x), xTransportSessionId(0x%x)\n", pRequest, xTransportSessionId);
	}

	// 3-Set CA descriptor and its length
	eStatus = caRequestSetDescriptors(pRequest, (TSize)usDesTotalLen, s_ucCatSection+8);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("caRequestSetDescriptors : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
		caRequestDispose(pRequest);
		pRequest = NULL;
		return;
	}

	eStatus = caRequestSend(pRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("caRequestSend : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
		caRequestDispose(pRequest);
		pRequest = NULL;
		return;
	}

	eStatus = caRequestGetProcessingStatus(pRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("caRequestSend : err(%d), hAction:0x%X, eProcStatus:0x%x\n", eStatus, (TUnsignedInt32)hAction, eProcStatus);
		caRequestDispose(pRequest);
		pRequest = NULL;
		return;
	}

	s_pstTestCatInfo->bIsAvailable = TRUE;
	s_pstTestCatInfo->hAction = hAction;
	s_pstTestCatInfo->usUniqueId = usUniqueId;
	s_pstTestCatInfo->pxEmmFliteringRequest = pRequest;
}


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static SvcCas_DescPointList_t* _naMgr_cakSvc_GetDescPoint(TUnsignedInt8 *pucRawDesc, TUnsignedInt16 usRawDescLen, TUnsignedInt8 ucDescTag)
{
	TUnsignedInt32	i;
	TUnsignedInt32 	ulLen;

	TUnsignedInt8		ucTag;
	TUnsignedInt8	*	pucDescRaw;

	SvcCas_DescPointList_t *pDescListStart;
	SvcCas_DescPointList_t *pDescList;
	SvcCas_DescPointList_t *pPrevDescList;

	if ( (pucRawDesc == NULL) || (usRawDescLen == 0) )
	{
		return NULL;
	}

	pDescListStart =NULL;
	pPrevDescList = NULL;
	i = 0;

	while (1)
	{
		pucDescRaw = &pucRawDesc[i];
		ucTag = pucRawDesc[i++];
		ulLen = pucRawDesc[i++];

		if (ucTag == ucDescTag)
		{
			pDescList = (SvcCas_DescPointList_t *)OxCAS_Malloc(sizeof(SvcCas_DescPointList_t));
			if (pDescList == NULL)
			{
				_naMgr_cakSvc_FreeDescPoint(pDescListStart);
				HxLOG_Error("error in OxCAS_Malloc\n");
				return NULL;
			}

			HxSTD_memset(pDescList, 0, sizeof(SvcCas_DescPointList_t));
			pDescList->ucTag = ucDescTag;
			pDescList->pucRaw = pucDescRaw;

			if (pPrevDescList == NULL)
				pDescListStart = pDescList;
			else
				pPrevDescList->pNext = pDescList;

			pPrevDescList = pDescList;
		}

		i += ulLen;

		if (i == usRawDescLen)
		{
			break;
		}

		if (i > usRawDescLen)
		{
			if (pDescListStart != NULL)
			{
				_naMgr_cakSvc_FreeDescPoint(pDescListStart);
			}

			HxLOG_Error("corrupt ES Info Raw data\n");
			return NULL;
		}
	}

	return pDescListStart;
}


static void _naMgr_cakSvc_FreeDescPoint(SvcCas_DescPointList_t *pDescList)
{
	SvcCas_DescPointList_t		*pCurr;
	SvcCas_DescPointList_t		*pNext;

	if (pDescList == NULL)
	{
		HxLOG_Error("pDescList is NULL\n");
		return;
	}

	pCurr = pDescList;
	while (1)
	{
		if (pCurr->pucRaw == NULL)
		{
			HxLOG_Error("pCurr->pucRaw is NULL\n");
			return;
		}

		pNext = pCurr->pNext;
		if(pCurr != NULL)
			OxCAS_Free(pCurr);

		if (pNext == NULL)
			return;

		pCurr = pNext;
	}
}


static HERROR _naMgr_cakSvc_ParsePmt(Handle_t hAction, TUnsignedInt8 *pucPmtRaw)
{
	TUnsignedInt8	i, j;
	TUnsignedInt16	usLen;
	TUnsignedInt16	usPmtLen;
	TUnsignedInt8		*pucPmtEsRaw;
	TUnsignedInt16	usEsRawLen;
	TUnsignedInt16	usProgInfoLen;
	TUnsignedInt16	usEsInfoLen;
	TUnsignedInt8		*pucCaDescRaw;
	TUnsignedInt32	ulActionId;

	SvcCas_DescPointList_t *pucCaDescList;
	PSI_CA_PMT_INFO_t *pstPmtInfo = NULL;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	TUnsignedInt8		ucDescRawLen;
#endif

	ulActionId = GET_ACTION_ID(hAction);
	if (ulActionId >= MAX_NO_SESSION)
	{
		HxLOG_Error("Wrong action (0x%x)\n", hAction);
		return ERR_FAIL;
	}

	pstPmtInfo = &stPsiCaPmtInfo[ulActionId];
	if ( (pucPmtRaw == NULL) || (pstPmtInfo == NULL) )
	{
		HxLOG_Error("Targets are NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstPmtInfo, 0x00, sizeof(PSI_CA_PMT_INFO_t));

	pstPmtInfo->usServiceId = ((TUnsignedInt16)pucPmtRaw[3] << 8) & 0xff00;
	pstPmtInfo->usServiceId |= (TUnsignedInt16)pucPmtRaw[4];

	pstPmtInfo->ucVerNum = (pucPmtRaw[5] >> 1) & 0x1f;
	pstPmtInfo->bCurNextInd = pucPmtRaw[5] & 0x01;

	pstPmtInfo->usPcrPid = ((TUnsignedInt16)pucPmtRaw[8] << 8) & 0x1f00;
	pstPmtInfo->usPcrPid |= (TUnsignedInt16)pucPmtRaw[9];

	usPmtLen = (TUnsignedInt16)get12bit(pucPmtRaw + 1) + 3;
	pstPmtInfo->usPmtLen =(TUnsignedInt16)get12bit(pucPmtRaw + 1);
	pstPmtInfo->ulCrc32 = (TUnsignedInt32)get32bit(pucPmtRaw + usPmtLen -4);

	usProgInfoLen = ((TUnsignedInt16)pucPmtRaw[10] << 8) & 0x0f00;
	usProgInfoLen |= (TUnsignedInt16)pucPmtRaw[11];

	pstPmtInfo->ucNumOfCADesc = 0;
	pstPmtInfo->ucaFlag = 0x00;
	HxSTD_memset(pstPmtInfo->stGlobCaDesc, 0x00, (sizeof(PSI_CA_DES_t) * MAX_NUM_OF_CA_PID));

	if (usProgInfoLen > 0)
	{
		pucCaDescList = _naMgr_cakSvc_GetDescPoint(&pucPmtRaw[12], usProgInfoLen, TAG_CA);
		if (pucCaDescList != NULL)
		{
			SvcCas_DescPointList_t *pList = NULL;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			TUnsignedInt8 ucCntFoundNacd = 0;
#endif
			pList = pucCaDescList;
			for (i=0; i<MAX_NUM_OF_CA_PID; i++)
			{
				pucCaDescRaw = pList->pucRaw;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
				ucDescRawLen = pucCaDescRaw[1];
#endif
				pstPmtInfo->stGlobCaDesc[i].usCaSysId = ((TUnsignedInt16)pucCaDescRaw[2] << 8) & 0xff00;
				pstPmtInfo->stGlobCaDesc[i].usCaSysId |= (TUnsignedInt16)pucCaDescRaw[3];
				pstPmtInfo->stGlobCaDesc[i].usCaPid = ((TUnsignedInt16)pucCaDescRaw[4] << 8) & 0x1f00;
				pstPmtInfo->stGlobCaDesc[i].usCaPid |= (TUnsignedInt16)pucCaDescRaw[5];

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
				ucDescRawLen -= 4;
				if ( (ucCntFoundNacd <5) && (ucDescRawLen > 0) )
				{
					TUnsignedInt8	ucDesTag, ucDesLen;
					TUnsignedInt8	*pucDescRawPtr = pucCaDescRaw+6;

					while (ucDescRawLen > 0)
					{
						ucDesTag = *pucDescRawPtr;
						ucDesLen	 = *(pucDescRawPtr+1);

						if (ucDesTag == TAG_MACD_CA)
						{
							pstPmtInfo->stCaMacDesc[ucCntFoundNacd].pucRawData = (TUnsignedInt8*)OxCAS_Malloc((ucDesLen+2));
							if (pstPmtInfo->stCaMacDesc[ucCntFoundNacd].pucRawData != NULL)
							{
								pstPmtInfo->stCaMacDesc[ucCntFoundNacd].usEcmCaSysId = pstPmtInfo->stGlobCaDesc[i].usCaSysId;
								pstPmtInfo->stCaMacDesc[ucCntFoundNacd].ucTag = TAG_MACD_CA;
								pstPmtInfo->stCaMacDesc[ucCntFoundNacd].ucLength = (ucDesLen+2);
								VK_memcpy(pstPmtInfo->stCaMacDesc[ucCntFoundNacd].pucRawData, pucDescRawPtr, pstPmtInfo->stCaMacDesc[ucCntFoundNacd].ucLength);
								ucCntFoundNacd++;
							}
						}

						pucDescRawPtr += (ucDesLen+2);
						ucDescRawLen -= (ucDesLen+2);
					}
				}
#endif
				if (pList->pNext == NULL)
					break;
				pList = pList->pNext;
			}

			_naMgr_cakSvc_FreeDescPoint(pucCaDescList);
			pstPmtInfo->ucNumOfCADesc = (i + 1);
			pstPmtInfo->ucaFlag = 0x0f;
		}
	}

	pucPmtEsRaw = &pucPmtRaw[usProgInfoLen+12];
	usEsRawLen = usPmtLen - (usProgInfoLen+16);
	usLen = 0;
	for (i=0; i<MAX_NUM_OF_SERVICE_PID; i++)
	{
		pstPmtInfo->stElemInfo[i].ucStreamType = pucPmtEsRaw[usLen++];
		pstPmtInfo->stElemInfo[i].usElemPid = ((TUnsignedInt16)pucPmtEsRaw[usLen++] << 8) & 0x1f00;
		pstPmtInfo->stElemInfo[i].usElemPid |= (TUnsignedInt16)pucPmtEsRaw[usLen++];
		usEsInfoLen = ((TUnsignedInt16)pucPmtEsRaw[usLen++] << 8) & 0x0f00;
		usEsInfoLen |= (TUnsignedInt16)pucPmtEsRaw[usLen++];

		switch(pstPmtInfo->stElemInfo[i].ucStreamType)
		{
		case ISO_VIDEO_1:
		case ISO_VIDEO_2:
		case ISO_VIDEO_4:
			pstPmtInfo->stElemInfo[i].eEsType = eEsType_Video;
			break;

		case ISO_AUDIO_1:
		case ISO_AUDIO_2:
		case ISO_AUDIO_4:
			pstPmtInfo->stElemInfo[i].eEsType = eEsType_Audio;
			break;

		case ISO_PRIVDATA_2:
			pucCaDescList = _naMgr_cakSvc_GetDescPoint(&pucPmtEsRaw[usLen], usEsInfoLen, TAG_DOLBY_AC3);
			if (pucCaDescList != NULL )
			{
				pstPmtInfo->stElemInfo[i].eEsType = eEsType_Audio;
				_naMgr_cakSvc_FreeDescPoint(pucCaDescList);
				break;
			}
#if defined(SUPPORT_DOLBY_AC3P)
			pucCaDescList = _naMgr_cakSvc_GetDescPoint(&pucPmtEsRaw[usLen], usEsInfoLen, TAG_ENHANCED_AC3);
			if (pucCaDescList != NULL )
			{
				pstPmtInfo->stElemInfo[i].eEsType = eEsType_Audio;
				_naMgr_cakSvc_FreeDescPoint(pucCaDescList);
				break;
			}
#endif
		case ISO_PRIVDATA_1:
			pucCaDescList = _naMgr_cakSvc_GetDescPoint(&pucPmtEsRaw[usLen], usEsInfoLen, TAG_SUBTITLING);
			if (pucCaDescList != NULL )
			{
				pstPmtInfo->stElemInfo[i].eEsType = eEsType_Subtitle;
				_naMgr_cakSvc_FreeDescPoint(pucCaDescList);
				break;
			}
			else
			{
				pucCaDescList = _naMgr_cakSvc_GetDescPoint(&pucPmtEsRaw[usLen], usEsInfoLen, TAG_TELETEXT);
				if (pucCaDescList != NULL )
				{
					pstPmtInfo->stElemInfo[i].eEsType = eEsType_Teletext;
					_naMgr_cakSvc_FreeDescPoint(pucCaDescList);
					break;
				}
			}
			pstPmtInfo->stElemInfo[i].eEsType = eEsType_Unknown;
			break;

		case ISO_DSMCC_U_N:
			pstPmtInfo->stElemInfo[i].ucStreamType = ISO_DSMCC;
			pstPmtInfo->stElemInfo[i].eEsType = eEsType_Data;
			break;

		default:
			pstPmtInfo->stElemInfo[i].eEsType = eEsType_Unknown;
			break;
		}

		pstPmtInfo->stElemInfo[i].ucNumOfCADesc = 0;
		pstPmtInfo->stElemInfo[i].ucaFlag = 0x00;
		HxSTD_memset(pstPmtInfo->stElemInfo[i].stElemCaDesc, 0x00, (sizeof(PSI_CA_DES_t) * MAX_NUM_OF_CA_PID));

		if (usEsInfoLen > 0)
		{
			pucCaDescList = _naMgr_cakSvc_GetDescPoint(&pucPmtEsRaw[usLen], usEsInfoLen, TAG_CA);

			if (pucCaDescList != NULL)
			{
				SvcCas_DescPointList_t 	*pList;
				pList = pucCaDescList;
				for (j=0; j<MAX_NUM_OF_CA_PID; j++)
				{
					pucCaDescRaw = pList->pucRaw;
					pstPmtInfo->stElemInfo[i].stElemCaDesc[j].usCaSysId = ((TUnsignedInt16)pucCaDescRaw[2] << 8) & 0xff00;
					pstPmtInfo->stElemInfo[i].stElemCaDesc[j].usCaSysId |= (TUnsignedInt16)pucCaDescRaw[3];
					pstPmtInfo->stElemInfo[i].stElemCaDesc[j].usCaPid = ((TUnsignedInt16)pucCaDescRaw[4] << 8) & 0x1f00;
					pstPmtInfo->stElemInfo[i].stElemCaDesc[j].usCaPid |= (TUnsignedInt16)pucCaDescRaw[5];

					if (pList->pNext == NULL)
						break;
					pList = pList->pNext;
				}

				_naMgr_cakSvc_FreeDescPoint(pucCaDescList);
				pstPmtInfo->stElemInfo[i].ucNumOfCADesc = j + 1;
				pstPmtInfo->stElemInfo[i].ucaFlag = 0x0f;
			}
		}

		usLen += usEsInfoLen;

		if (usLen == usEsRawLen)
		{
			pstPmtInfo->ucNumOfElem= i + 1;
			break;
		}

		if (usLen > usEsRawLen)
		{
			HxLOG_Error("Corrupted PMT data....\n");
			HxSTD_memset(pstPmtInfo, 0x00, sizeof(PSI_CA_PMT_INFO_t));
			pstPmtInfo = NULL;
			return ERR_FAIL;
		}
	}
	pstPmtInfo->bUsed = TRUE;
	return ERR_OK;
}

static HERROR _naMgr_cakSvc_GetPmtInfo(Handle_t hAction, PSI_CA_PMT_INFO_t **ppstPmtInfo)
{
	TUnsignedInt32 ulActionId;

	if (ppstPmtInfo == NULL)
	{
		HxLOG_Error("Targets are NULL\n");
		return ERR_FAIL;
	}

	ulActionId = GET_ACTION_ID(hAction);
	if (ulActionId >= MAX_NO_SESSION)
	{
		HxLOG_Error("Wrong action (0x%x)\n", hAction);
		return ERR_FAIL;
	}

	*ppstPmtInfo = &stPsiCaPmtInfo[ulActionId];
	return ERR_OK;
}


static HERROR _naMgr_cakSvc_IsHdPlusService(Handle_t hAction)
{
	TUnsignedInt8 i;
	pCaPlayInst_t pstCaPlayInst;

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable(0x%x)\n", hAction);
		return ERR_FAIL;
	}
	for (i=0; i<5; i++)
	{
		if (pstCaPlayInst->stCaMacDesc[i].ucTag == TAG_MACD_CA)
		{
			HxLOG_Print("This Action (0x%x) has MAC_CA_DESC\n", hAction);
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}


static HERROR _naMgr_cakSvc_GetSizeOfMacCaDesc(Handle_t hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucLen)
{
	TUnsignedInt8 i;
	pCaPlayInst_t pstCaPlayInst;

	if (pucLen == NULL)
		return ERR_FAIL;

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable(0x%x)\n", hAction);
		return ERR_FAIL;
	}

	for (i=0; i<5; i++)
	{
		if ( (pstCaPlayInst->stCaMacDesc[i].ucTag == TAG_MACD_CA)
			&& (pstCaPlayInst->stCaMacDesc[i].usEcmCaSysId == usScCaSysId) )
		{
			HxLOG_Print("This Action (0x%x) has MAC_CA_DESC\n", hAction);
			*pucLen = pstCaPlayInst->stCaMacDesc[i].ucLength;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}


static HERROR _naMgr_cakSvc_GetContentOfMacCaDesc(Handle_t hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucDesc)
{
	TUnsignedInt8 i;
	pCaPlayInst_t pstCaPlayInst;

	if (pucDesc == NULL)
		return ERR_FAIL;

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable(0x%x)\n", hAction);
		return ERR_FAIL;
	}

	for (i=0; i<5; i++)
	{
		if ( (pstCaPlayInst->stCaMacDesc[i].ucTag == TAG_MACD_CA)
			&& (pstCaPlayInst->stCaMacDesc[i].usEcmCaSysId == usScCaSysId) )
		{
			HxLOG_Print("This Action (0x%x) has MAC_CA_DESC\n", hAction);
			VK_memcpy(pucDesc, pstCaPlayInst->stCaMacDesc[i].pucRawData, pstCaPlayInst->stCaMacDesc[i].ucLength);
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}


static HERROR _naMgr_cakSvc_GetActionsSameService(Handle_t hAction, Handle_t *phActions, TUnsignedInt8 *pucNum)
{
	pCaPlayInst_t 	pstCaPlayInst;

	TUnsignedInt8	i;
	TUnsignedInt8	ucCnt = 0;
	TUnsignedInt16 	usUniqueId, usSvcId;

	if ( (phActions == NULL) || (pucNum == NULL) )
		return ERR_FAIL;

	*pucNum = 0;
	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable(0x%x)\n", hAction);
		return ERR_FAIL;
	}

	usUniqueId = pstCaPlayInst->usUniqueId;
	usSvcId = pstCaPlayInst->usSvcId;

	WAIT_FOR_SEM(s_stNaCaInst.ulSem)
	for (i=0; i<MAX_NO_SESSION; i++)
	{
		if ( (!CHECK_SAME_HANDLE(s_stNaCaInst.stCaPlayInstList[i].hAction, hAction))
			&& (CHECK_SAME_HANDLE(s_stNaCaInst.stCaPlayInstList[i].eInstState, eInstStartState)) )
		{
			if ( (CHECK_SAME_HANDLE(s_stNaCaInst.stCaPlayInstList[i].usUniqueId, usUniqueId))
				&& (CHECK_SAME_HANDLE(s_stNaCaInst.stCaPlayInstList[i].usSvcId, usSvcId)) )
			{
				phActions[ucCnt++] = s_stNaCaInst.stCaPlayInstList[i].hAction;
			}
		}
	}

	if (ucCnt > 0)
	{
		*pucNum = ucCnt;
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return ERR_OK;
	}

	RELEASE_SEM(s_stNaCaInst.ulSem)
	return ERR_FAIL;
}

#endif


static void _naMgr_cakSvc_PmtReceived(NaCasMsg_t *pstMsg)
{
	TUnsignedInt8 		i, j;
	TUnsignedInt32 		hAction;
	TUnsignedInt8 		*pucPmt;
	TUnsignedInt8			ucIndex;
	TUnsignedInt32 		ulErrCode;
	TUnsignedInt8			ucNumOfElem = 0;
	TUnsignedInt16		usSizeOfSection = 0;

	SvcCas_PmtInfo_t 		*pstPmtInfo;
	TUnsignedInt32 		ulSecStatus;

	PSI_CA_PMT_INFO_t 	*pstCaPmtInfo = NULL;

	TCaRequest 			*pxRequest = NULL;
	TCaRequestStatus 		eStatus = 0;
	TCaProcessingStatus	eProcStatus = 0;
	TUnsignedInt32		ulNumOfPid = 0;
	TPid               		axPidArray[CAS_MAX_NUM_OF_ES];
#if org
	PSI_CA_PMT_INFO_t 	stCaPmtInfo;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	HUINT16  *pusPids = NULL;
#endif

	if (pstMsg == NULL)
	{
		HxLOG_Error("pstMsg is NULL\n");
		return;
	}

	hAction = pstMsg->hWnd;
	pucPmt = (TUnsignedInt8*)pstMsg->lParam1;

	ulErrCode = _naMgr_cakSvc_FindSvcInfoIdxByAction(hAction, &ucIndex);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("_naMgr_cakSvc_FindSvcInfoIdxByAction() error(%x)\n", (unsigned int)ulErrCode);
		return;
	}

	s_pstTestCaSvcInfo = _naMgr_cakSvc_GetCasSvcInfoPtrByIdx(ucIndex);
	if (s_pstTestCaSvcInfo == NULL)
	{
		HxLOG_Error("_naMgr_cakSvc_GetCasSvcInfoPtrByIdx return NULL\n");
		return;
	}

	// CAK library의 descramble을 중지시킨다.
	if (pucPmt == NULL)
	{
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		HxLOG_Error("pucPmt is NULL, Action:0x%x\n", (unsigned int)hAction);
#else
		_naMgr_cakSvc_DisposeRequest(s_pstTestCaSvcInfo->pxProgDesRequest);
		s_pstTestCaSvcInfo->pxProgDesRequest = NULL;
		s_pstTestCaSvcInfo->bIsSvcStarted = FALSE;
#endif
		return;
	}

	HxLOG_Print("Action:0x%x, pucPmt:0x%x\n", (unsigned int)hAction, (unsigned int)pucPmt);

	ulNumOfPid = 0;
	HxSTD_memset(axPidArray, 0x00, (sizeof(TPid) * CAS_MAX_NUM_OF_ES));
	HxSTD_memset(s_ucPmtSection, 0x00, (BUF_SIZE_1K * 2));

	usSizeOfSection = get12bit(pucPmt + 1) + 3;
	if (usSizeOfSection > (BUF_SIZE_1K*2))
	{
		HxLOG_Error("Over of section size:%d\n", usSizeOfSection);
		return;
	}

	HxLOG_Print("Section Size = %d\n", usSizeOfSection);
	HxSTD_memcpy(s_ucPmtSection, pucPmt, usSizeOfSection);

	s_pstTestPmtInfo = _naMgr_cakSvc_GetPmtInfoPtrByIdx(ucIndex);
	if (s_pstTestPmtInfo == NULL)
	{
		HxLOG_Error("_naMgr_cakSvc_GetPmtInfoPtrByIdx return NULL\n");
		return;
	}

	pxRequest = s_pstTestCaSvcInfo->pxProgDesRequest;
	if (pxRequest == NULL)
	{
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		HxLOG_Error("Program Desc request is NULL\n");
		return;
#else
		// 1-Creat Program Descrambling Request
		eStatus = caRequestCreate(CA_REQUEST_TYPE_PROGRAM_DESCRAMBLING, &pxRequest);
		if (eStatus != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("caRequestCreate(CA_REQUEST_TYPE_PROGRAM_DESCRAMBLING) : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
			return;
		}

		// 2-Set TransportSessionId
		eStatus = caRequestSetTransportSessionId(pxRequest, s_pstTestCaSvcInfo->xTsId);
		if (eStatus != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("caRequestSetTransportSessionId : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
			caRequestDispose(pxRequest);
			return;
		}

		// 3-Set SourceId(= Original Network Id)
		eStatus = caRequestSetSourceId(pxRequest, s_pstTestCaSvcInfo->xSourceId);
		if (eStatus != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("caRequestSetSourceId : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
			caRequestDispose(pxRequest);
			return;
		}

		// 3-Set Exportation Callback
		eStatus = caRequestSetExportationCallback(pxRequest, (TCaRequestExportationCallback)_naMgr_cakSvc_PgmDescCallback);
		if (eStatus != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Error("caRequestSetExportationCallback err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
			caRequestDispose(pxRequest);
			return;
		}

		s_pstTestCaSvcInfo->pxProgDesRequest = pxRequest;
#endif
	}

	eStatus = caRequestSetPmtSection(pxRequest, (TSize)usSizeOfSection, s_ucPmtSection);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("caRequestSetPmtSection err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
		return;
	}

	/* Init the pmt global var */
#if org
	pstCaPmtInfo = &stCaPmtInfo;
#else
	pstCaPmtInfo = (PSI_CA_PMT_INFO_t*)OxCAS_Malloc(sizeof(PSI_CA_PMT_INFO_t));
	if (pstCaPmtInfo == NULL)
	{
		HxLOG_Error("pstCaPmtInfo mem alloc error  \n");
		return;
	}
#endif

	HxSTD_memset(pstCaPmtInfo, 0x00, sizeof(PSI_CA_PMT_INFO_t));
	HxSTD_memset(s_pstTestPmtInfo, 0x00, sizeof(PSI_PMT_INFO_t));

 	ulErrCode = svc_cas_MgrSubGetPmtSectionState(hAction, (unsigned int*)&ulSecStatus);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrSubGetPmtSectionState return error(%x), Action(0x%x)\n", (unsigned int)ulErrCode, (unsigned int)hAction);
#if !org
		OxCAS_Free(pstCaPmtInfo);
#endif
		return;
	}

	if (ulSecStatus != TRUE)
	{
		HxLOG_Error("PMT Section status (%d)\n", (int)ulSecStatus);
#if !org
		OxCAS_Free(pstCaPmtInfo);
#endif
		return;
	}

	if ((pstPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction)) == NULL)
	{
		HxLOG_Error("error in svc_cas_MgrSubGetPmtInfo\n");
#if !org
		OxCAS_Free(pstCaPmtInfo);
#endif
		return;
	}

	pstCaPmtInfo->usServiceId = pstPmtInfo->usServiceId;
	pstCaPmtInfo->ulCrc32 = pstPmtInfo->ulCrc;
	pstCaPmtInfo->usPcrPid = pstPmtInfo->usPcrPid;
	pstCaPmtInfo->ucVerNum = pstPmtInfo->ucVerNum;
	pstCaPmtInfo->bCurNextInd = pstPmtInfo->bCurNextInd;

	pstCaPmtInfo->ucaFlag = pstPmtInfo->bProgramCaFlag;
	pstCaPmtInfo->ucNumOfCADesc = (TUnsignedInt8)pstPmtInfo->ulNumOfProgramCaDescriptor;
	for (i=0; i<pstCaPmtInfo->ucNumOfCADesc && i<MAX_NUM_OF_CA_PID; i++)
	{
		pstCaPmtInfo->stGlobCaDesc[i].usCaPid = pstPmtInfo->stProgramCaDescriptor[i].usCaPid;
		pstCaPmtInfo->stGlobCaDesc[i].usCaSysId = pstPmtInfo->stProgramCaDescriptor[i].usCaSysId;
	}

	pstCaPmtInfo->ucNumOfElem = (TUnsignedInt8)pstPmtInfo->ulNumOfElement;
	for (i=0; i<pstCaPmtInfo->ucNumOfElem && i<MAX_NUM_OF_SERVICE_PID; i++)
	{
		pstCaPmtInfo->stElemInfo[i].ucStreamType = pstPmtInfo->stElementInfo[i].ucStreamType;
		pstCaPmtInfo->stElemInfo[i].eEsType = pstPmtInfo->stElementInfo[i].eEsType;
		pstCaPmtInfo->stElemInfo[i].bIsCurrent = pstPmtInfo->stElementInfo[i].bIsCurrent;
		pstCaPmtInfo->stElemInfo[i].usElemPid = pstPmtInfo->stElementInfo[i].usPid;

		pstCaPmtInfo->stElemInfo[i].ucaFlag = pstPmtInfo->stElementInfo[i].bElementCaFlag;
		pstCaPmtInfo->stElemInfo[i].ucNumOfCADesc = (TUnsignedInt8)pstPmtInfo->stElementInfo[i].ulNumOfCaDescriptor;
		for (j=0; j<pstCaPmtInfo->stElemInfo[i].ucNumOfCADesc && j<MAX_NUM_OF_CA_PID; j++)
		{
			pstCaPmtInfo->stElemInfo[i].stElemCaDesc[j].usCaPid = pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].usCaPid;
			pstCaPmtInfo->stElemInfo[i].stElemCaDesc[j].usCaSysId = pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].usCaSysId;
		}
	}

	ucNumOfElem = ((pstCaPmtInfo->ucNumOfElem > MAX_NUM_OF_SERVICE_PID) ? MAX_NUM_OF_SERVICE_PID : pstCaPmtInfo->ucNumOfElem);
	pstCaPmtInfo->ucNumOfElem = s_pstTestPmtInfo->ucNumOfElem = ucNumOfElem;

	HxLOG_Print("Num of elem in PMT [%d]\n", ucNumOfElem);
	for (i=0; i<s_pstTestPmtInfo->ucNumOfElem; i++)
	{
		s_pstTestPmtInfo->bScrambleFlag[i] = FALSE;
	}

	if (pstCaPmtInfo->ucaFlag != FALSE)
	{
		for (i=0; i<s_pstTestPmtInfo->ucNumOfElem; i++)
		{
			s_pstTestPmtInfo->bScrambleFlag[i] = TRUE;
		}
	}


	// Inserted된 SC의 ECM CA_SYSTEM_ID는 고려하지 않고 scrabmle된 PID는 모두 설정 하도록 한다.
	for (i=0; i<ucNumOfElem; i++)
	{
		if (pstCaPmtInfo->stElemInfo[i].eEsType == eEsType_Video)
			s_pstTestPmtInfo->ucStreamType[i] = MPEG_TYPE_VIDEO;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)// UHD Video type		
		else if (pstCaPmtInfo->stElemInfo[i].ucStreamType == ISO_VIDEO_5)
			s_pstTestPmtInfo->ucStreamType[i] = MPEG_TYPE_VIDEO;
#endif		
		else if (pstCaPmtInfo->stElemInfo[i].eEsType == eEsType_Audio)
			s_pstTestPmtInfo->ucStreamType[i] = MPEG_TYPE_AUDIO;
		else if ( (pstCaPmtInfo->stElemInfo[i].eEsType == eEsType_Teletext)
				|| (pstCaPmtInfo->stElemInfo[i].eEsType == eEsType_Subtitle) )
			s_pstTestPmtInfo->ucStreamType[i] = MPEG_TYPE_PRIVATE_DATA;
#if (CONFIG_S_DSMCC == 1)
		else if (pstCaPmtInfo->stElemInfo[i].eEsType == eEsType_Data)
			s_pstTestPmtInfo->ucStreamType[i] = MPEG_TYPE_DSMCC;
#endif
		else
			s_pstTestPmtInfo->ucStreamType[i] = MPEG_TYPE_AUXILIARY;

		s_pstTestPmtInfo->usEsPid[i] = pstCaPmtInfo->stElemInfo[i].usElemPid;
		s_pstTestPmtInfo->eEsType[i] = pstCaPmtInfo->stElemInfo[i].eEsType;
		s_pstTestPmtInfo->eStreamTypeDetail[i] = pstCaPmtInfo->stElemInfo[i].ucStreamType;

		if (pstCaPmtInfo->stElemInfo[i].ucaFlag != FALSE)
		{
			if (s_pstTestPmtInfo->bScrambleFlag[i] != TRUE)
			{
				s_pstTestPmtInfo->bScrambleFlag[i] = TRUE;
			}
		}
	}

#if !org
	OxCAS_Free(pstCaPmtInfo);
#endif
	// Elementary stream PID를 setting 한다.
	for (i=0; i<s_pstTestPmtInfo->ucNumOfElem && i<MAX_NO_STREAM; i++)
	{
		if ( (s_pstTestPmtInfo->bScrambleFlag[i] == TRUE)
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) && defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
			&& (s_pstTestPmtInfo->ucStreamType[i] != MPEG_TYPE_DSMCC)
			&& (s_pstTestPmtInfo->ucStreamType[i] != MPEG_TYPE_PRIVATE_DATA)
			&& (s_pstTestPmtInfo->ucStreamType[i] != MPEG_TYPE_MHEG)
#endif
			&& (s_pstTestPmtInfo->ucStreamType[i] != MPEG_TYPE_AUXILIARY) )

		{
			axPidArray[ulNumOfPid] = s_pstTestPmtInfo->usEsPid[i];
			ulNumOfPid++;
		}
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
/*	PMT가 먼저 CAK으로 전달이 되면 Open Session -> SetMeta -> SetKey 동작이 CAK에 의해 일어난다.
	이 때 DSC가 준비되어 있지 않으면 처음 ECM(CW)의 SetKey 시점에서 ERROR 발생이 된다.
	만일에, ECM 주기를 길게 가져간다면, START 부분이 ECM 주기만큼 AV가 안나올 수 있다. (Rec/Playback 역시 동일) */
#if defined(CAS_ZAP_LOG)
	{
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SVC[%s:%d] -----  Received PMT ----  hAction(0x%08x) ----------\n", __FUNCTION__, __LINE__, hAction);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif

	if(ulNumOfPid != 0)
	{
		pusPids = (HUINT16 *)OxCAS_Calloc(ulNumOfPid * sizeof(HUINT16));
		if(pusPids != NULL)
			HxSTD_memcpy(pusPids, axPidArray, ulNumOfPid * sizeof(HUINT16));
		else
			HxLOG_Error("OxCAS_Calloc() error. hAction(0x%x)\n", hAction);
	}

	if(caSessionMgr_AssociatePids(hAction, ulNumOfPid, pusPids) != ERR_OK)
	{
		HxLOG_Error("caSessionMgr_AssociatePids() error. hAction(0x%x)\n", hAction);
		if(pusPids != NULL) OxCAS_Free(pusPids);
	}
#endif

	eStatus = caRequestSetElemStreamPids(pxRequest, ulNumOfPid, axPidArray);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("caRequestSetElemStreamPids ERR (%d), hAct:0x%X \n", eStatus, (TUnsignedInt32)hAction);
		return;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("caRequestSend ERR!!(%d), hAct:0x%X \n", eStatus, (TUnsignedInt32)hAction);
		return;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("caRequestSend : err(%d), hAction:0x%X, eProcStatus:0x%x\n", eStatus, (TUnsignedInt32)hAction, eProcStatus);
		return;
	}

	s_pstTestCaSvcInfo->bIsSvcStarted = TRUE;
}


static void _naMgr_cakSvc_StartService(NaCasMsg_t *pstMsg)
{
	TUnsignedInt32 		hAction;
	TUnsignedInt8			ucIndex;
	TUnsignedInt32		ulErrCode;

	TCaRequest			*pRequest = NULL;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	TCaRequestStatus		eStatus = 0;
	TTransportSessionId	xTransportSessionId = 0;
	TCaSourceId			xSourceId = 0;
#endif
	SvcCas_DbSvcInfo_t 	stSvcInfo;

	if (pstMsg == NULL)
	{
		HxLOG_Error("pstMsg is NULL\n");
		return;
	}

	hAction = pstMsg->hWnd;
	HxLOG_Print("Action:0x%x\n", (unsigned int)hAction);

	ulErrCode = _naMgr_cakSvc_GetSvcInfoIdx(hAction, &ucIndex);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetSvcInfoIdx\n");
		return;
	}

	ulErrCode = _naMgr_cakSvc_MakeCaSvcInfoAvailableByIdx(ucIndex);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_MakeCaSvcInfoAvailableByIdx(%d)\n", (unsigned int)ulErrCode);
		return;
	}

	s_pstTestCaSvcInfo = _naMgr_cakSvc_GetCasSvcInfoPtrByIdx(ucIndex);
	if (s_pstTestCaSvcInfo == NULL)
	{
		HxLOG_Print("_naMgr_cakSvc_GetCasSvcInfoPtrByIdx return NULL\n");
		return;
	}

	// Get Service Info by Service Handle
	HxSTD_memset(&stSvcInfo, 0, sizeof(SvcCas_DbSvcInfo_t));
	ulErrCode = svc_cas_SubSvcDbGetSvcInfoByServiceHandle((Handle_t)pstMsg->info, &stSvcInfo);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_SubSvcDbGetSvcInfoByServiceHandle : err(0x%x), hSvc:0x%X\n", ulErrCode, s_pstTestCaSvcInfo->hSvcHandle);
		return;
	}

#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SVC[%s:%d] ----- (%d) ----- Start Service hAction(0x%08x) ----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif

// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)	// Multi-CAS 시나리오를 만족하기 위해 PMT 받은 후에 descramble request한다.
	// 1-Creat Program Descrambling Request
	eStatus = caRequestCreate(CA_REQUEST_TYPE_PROGRAM_DESCRAMBLING, &pRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("caRequestCreate(CA_REQUEST_TYPE_PROGRAM_DESCRAMBLING) : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
		return;
	}

	// 2-Set TransportSessionId
	xTransportSessionId = hAction;
	HxLOG_Print("Action(0x%X), TransportSessionId(0x%X)\n", hAction, xTransportSessionId);
	eStatus = caRequestSetTransportSessionId(pRequest, xTransportSessionId);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("caRequestSetTransportSessionId : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
		caRequestDispose(pRequest);
		pRequest = NULL;
		return;
	}

	// 3-Set SourceId(= Original Network Id)
	xSourceId = (TCaSourceId) stSvcInfo.usOnId;
	HxLOG_Print("Action(0x%X), SourceId(0x%X)\n", hAction, xSourceId);
	eStatus = caRequestSetSourceId(pRequest, xSourceId);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("caRequestSetSourceId : err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)xSourceId);
		caRequestDispose(pRequest);
		pRequest = NULL;
		return;
	}

	// 3-Set Exportation Callback
	eStatus = caRequestSetExportationCallback(pRequest, (TCaRequestExportationCallback)_naMgr_cakSvc_PgmDescCallback);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("caRequestSetAsynchronousResponse err(%d), hAction:0x%X\n", eStatus, (TUnsignedInt32)hAction);
		caRequestDispose(pRequest);
		pRequest = NULL;
		return;
	}
#endif

	s_pstTestCaSvcInfo->hAction 		= hAction;
	s_pstTestCaSvcInfo->eActType 		= (SvcCas_ActionType_e)pstMsg->lParam1;
	s_pstTestCaSvcInfo->hSvcHandle 		= pstMsg->info;
	s_pstTestCaSvcInfo->ucPlayMode 		= NORMAL_PLAY_MODE;
	s_pstTestCaSvcInfo->xTsId			= (TTransportSessionId)hAction;
	s_pstTestCaSvcInfo->xSourceId 		= (TCaSourceId)stSvcInfo.usOnId;
	s_pstTestCaSvcInfo->pxProgDesRequest = pRequest;

#if defined(CONFIG_SUPPORT_NOVA_APP)
	caSessionMgr_SendDrmHandleToNova(hAction, s_pstTestCaSvcInfo->hSvcHandle);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	if(caSessionMgr_AddTransportSessionId(hAction) != ERR_OK)
	{
		HxLOG_Error("caSessionMgr_AddTransportSessionId() error\n");
		return;
	}
#endif
}

static void _naMgr_cakSvc_StopService(NaCasMsg_t *pstMsg)
{
	TUnsignedInt32 	hAction;
	TUnsignedInt8		ucIndex;
	TUnsignedInt32	ulErrCode;

	if (pstMsg == NULL)
	{
		HxLOG_Error("pstMsg is NULL\n");
		return;
	}

	hAction = pstMsg->hWnd;
	HxLOG_Print("Action:0x%x\n", (unsigned int)hAction);

	ulErrCode = _naMgr_cakSvc_FindSvcInfoIdxByAction(hAction, &ucIndex);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_FindSvcInfoIdxByAction (%x)\n", (unsigned int)ulErrCode);
		return;
	}

	s_pstTestCaSvcInfo = _naMgr_cakSvc_GetCasSvcInfoPtrByIdx(ucIndex);
	if (s_pstTestCaSvcInfo == NULL)
	{
		HxLOG_Error("_naMgr_cakSvc_GetCasSvcInfoPtrByIdx return NULL\n");
		return;
	}

	s_pstTestPmtInfo = _naMgr_cakSvc_GetPmtInfoPtrByIdx(ucIndex);
	if (s_pstTestPmtInfo == NULL)
	{
		HxLOG_Error("_naMgr_cakSvc_GetPmtInfoPtrByIdx return NULL\n");
		return;
	}
#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SVC[%s:%d] ----- (%d) ----- Stop Service hAction(0x%08x) ----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif

	_naMgr_cakSvc_DisposeRequest(s_pstTestCaSvcInfo->pxProgDesRequest);

	HxSTD_memset(s_pstTestCaSvcInfo, 0x00, sizeof(PSI_CA_SERVICE_INFO_t));
	HxSTD_memset(s_pstTestPmtInfo, 0x00, sizeof(PSI_PMT_INFO_t));

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	if(caSessionMgr_RemoveTransportSessionId(hAction) != ERR_OK)
	{
		HxLOG_Error("caSessionMgr_RemoveTransportSessionId() error\n");
		return;
	}
#endif
}

static void _naMgr_cakSvc_StopEmm(NaCasMsg_t *pstMsg)
{
	TUnsignedInt32 	hAction;
	TUnsignedInt16	usUniqueId;

	if (pstMsg == NULL)
	{
		HxLOG_Error("pstMsg is NULL\n");
		return;
	}

	hAction = pstMsg->hWnd;
	usUniqueId = pstMsg->lParam3;
	HxLOG_Print("Action:0x%x, usUniqueId:0x%x\n", (unsigned int)hAction, (unsigned int)usUniqueId);

	s_pstTestCatInfo = _naMgr_cakSvc_FindCatInfoByUniqueId(usUniqueId);
	if (s_pstTestCatInfo == NULL)
	{
		HxLOG_Error("error in _naMgr_cakSvc_FindCatInfoByUniqueId....\n");
		return;
	}

	_naMgr_cakSvc_DisposeRequest(s_pstTestCatInfo->pxEmmFliteringRequest);
	HxSTD_memset(s_pstTestCatInfo, 0x00, sizeof(PSI_CAT_INFO_t));
}

static void _naMgr_cakSvc_SetPidService(NaCasMsg_t *pstMsg)
{
	TUnsignedInt32 	hAction;
	EsType_t			eEsType;
	TUnsignedInt8		ucStreamType;
	TUnsignedInt16	usEsPid;

	if (pstMsg == NULL)
	{
		HxLOG_Error("pstMsg is NULL\n");
		return;
	}

	hAction 			= pstMsg->hWnd;
	eEsType 			= pstMsg->lParam1;
	ucStreamType 	= pstMsg->info;
	usEsPid			= pstMsg->lParam3;

	HxLOG_Print("Action:0x%x, ES_Type:0x%x, Stream_Type:0x%x, ES_Pid:0x%x\n",
							(unsigned int)hAction, (unsigned int)eEsType, (unsigned int)ucStreamType, (unsigned int)usEsPid);
	return;
}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static void _naMgr_cakSvc_SetDsc(NaCasMsg_t *pstMsg)
{
	TUnsignedInt32 	hAction;
	CaEsPidList_t	*pstEsPid;
	HUINT32			ulNumOfPid = 0;
	HUINT16			*pusPids = NULL;
	HUINT16			ausPids[sizeof(CaEsPidList_t)];
	int i;

	if (pstMsg == NULL)
	{
		HxLOG_Error("pstMsg is NULL\n");
		return;
	}

	hAction		= pstMsg->hWnd;
	pstEsPid	= (CaEsPidList_t*)pstMsg->lParam3;

	if(pstEsPid->usVideoPid != NULL_PID)	ausPids[ulNumOfPid++] = pstEsPid->usVideoPid;
	if(pstEsPid->usTeletextPid != NULL_PID)	ausPids[ulNumOfPid++] = pstEsPid->usTeletextPid;
	if(pstEsPid->usSubtitlePid != NULL_PID)	ausPids[ulNumOfPid++] = pstEsPid->usSubtitlePid;
	for(i = 0 ; i < MAX_NUM_OF_AUDIO_TRACK ; i++)
		if(pstEsPid->ausAudioPid[i] != NULL_PID)	ausPids[ulNumOfPid++] = pstEsPid->ausAudioPid[i];

	pusPids = (HUINT16 *)OxCAS_Calloc(ulNumOfPid * sizeof(HUINT16));
	if(pusPids == NULL)
	{
		HxLOG_Error("OxCAS_Calloc() error. hAction(0x%x)\n", hAction);
		return;
	}

	HxSTD_memcpy(pusPids, ausPids, ulNumOfPid * sizeof(HUINT16));

#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SVC[%s:%d] ----- (%d) ----- Set Dsc hAction(0x%08x), ulNumOfPid(%d) ----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction, ulNumOfPid);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif


	if(caSessionMgr_AssociatePids(hAction, ulNumOfPid, pusPids) != ERR_OK)
	{
		HxLOG_Error("caSessionMgr_AssociatePids() error. hAction(0x%x)\n", hAction);
		OxCAS_Free(pusPids);
	}
}
#endif

static HERROR _naMgr_cakSvc_CheckValidSvcInfoIdx(TUnsignedInt8 ucIndex)
{
	if ( (ucIndex < 0) || (ucIndex >= MAX_NO_SESSION) )
	{
		HxLOG_Error("index is overflower (0x%x)\n", ucIndex);
		return ERR_FAIL;
	}

	if (s_stCaSvcInfoList[ucIndex].bIsAvailable == TRUE)
	{
		HxLOG_Error("already used...\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR _naMgr_cakSvc_GetSvcInfoIdx(TUnsignedInt32 hAction, TUnsignedInt8 *pucIndex)
{
	TUnsignedInt32 ulActID;
	TUnsignedInt32 ulErrCode;

	if (pucIndex == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	if (ASSERT_VALID_HANDLE(hAction))
	{
		HxLOG_Error("error in ASSERT_VALID_HANDLE\n");
		return ERR_FAIL;
	}

	ulActID = GET_ACTION_ID(hAction);

	ulErrCode = _naMgr_cakSvc_CheckValidSvcInfoIdx((TUnsignedInt8)ulActID);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_CheckValidSvcInfoIdx(%d)\n", (unsigned int)ulErrCode);
		return ERR_FAIL;
	}

	*pucIndex = (TUnsignedInt8)ulActID;
	return ERR_OK;
}


static HERROR _naMgr_cakSvc_FindSvcInfoIdxByAction(TUnsignedInt32 hAction, TUnsignedInt8 *pucIndex)
{
	TUnsignedInt8 i;

	if (pucIndex == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	if (ASSERT_VALID_HANDLE(hAction))
	{
		HxLOG_Error("error in ASSERT_VALID_HANDLE\n");
		return ERR_FAIL;
	}

	for (i=0; i<MAX_NO_SESSION; i++)
	{
		if ( (s_stCaSvcInfoList[i].bIsAvailable == TRUE)
			&& ((s_stCaSvcInfoList[i].hAction == hAction)
				|| (CHECK_SAME_ACTION_ID(s_stCaSvcInfoList[i].hAction, hAction))) )
		{
			*pucIndex = i;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}


static PSI_PMT_INFO_t* _naMgr_cakSvc_GetPmtInfoPtrByIdx(TUnsignedInt8 ucIndex)
{
	if ( /*(ucIndex < 0) ||*/ (ucIndex >= MAX_NO_SESSION) )
	{
		HxLOG_Error("over of index (0x%x)\n", ucIndex);
		return NULL;
	}

	if (s_stCaSvcInfoList[ucIndex].bIsAvailable != TRUE)
	{
		HxLOG_Error("unavailble pmt list\n");
		return NULL;
	}
	return &s_stPmtInfoList[ucIndex];
}


static PSI_CA_SERVICE_INFO_t* _naMgr_cakSvc_GetCasSvcInfoPtrByIdx(TUnsignedInt8 ucIndex)
{
	if ( /*(ucIndex < 0) ||*/ (ucIndex >= MAX_NO_SESSION) )
	{
		HxLOG_Error("over of index (0x%x)\n", ucIndex);
		return NULL;
	}

	if (s_stCaSvcInfoList[ucIndex].bIsAvailable != TRUE)
	{
		HxLOG_Error("unavailble pmt list\n");
		return NULL;
	}

	return &s_stCaSvcInfoList[ucIndex];
}


static HERROR _naMgr_cakSvc_MakeCaSvcInfoAvailableByIdx(TUnsignedInt8 ucIndex)
{
	if (s_stCaSvcInfoList[ucIndex].bIsAvailable == TRUE)
	{
		HxLOG_Error("Already Available\n");
		return ERR_FAIL;
	}

	s_stCaSvcInfoList[ucIndex].bIsAvailable = TRUE;
	s_stCaSvcInfoList[ucIndex].hAction = NULL_INDEX;
	s_stCaSvcInfoList[ucIndex].bIsSvcStarted = FALSE;
	return ERR_OK;
}


static PSI_CAT_INFO_t* _naMgr_cakSvc_FindCatInfoByUniqueId(TUnsignedInt16 usUniqueId)
{
	TUnsignedInt8 i;

	for (i=0; i<MAX_TUNER_NO; i++)
	{
		if ( (s_stCatInfoList[i].bIsAvailable == TRUE)
			&& (s_stCatInfoList[i].usUniqueId == usUniqueId) )
		{
			return &s_stCatInfoList[i];
		}
	}
	return NULL;
}


static PSI_CAT_INFO_t* _naMgr_cakSvc_MakeCatInfoAvailable(void)
{
	TUnsignedInt8 i;

	for (i=0; i<MAX_TUNER_NO; i++)
	{
		if (s_stCatInfoList[i].bIsAvailable == FALSE)
		{
			/*	여기서 TRUE로 잡아둔 경우 CAT가 처리되는 도중에 CAK에서 ERROR가 발생한다면,
				UniqueID가 Update되지 않기에 CH 전환시 SLOT FULL이 발생을 한다.
				CAT 처리가 정상일 때 TRUE로 변경(현재 수정된 방법) 또는  ERROR가 발생하면 FLAG를 FALSE로 바꿔야 한다. */
			//s_stCatInfoList[i].bIsAvailable = TRUE;
			return &s_stCatInfoList[i];
		}
	}
	return NULL;
}


static HERROR _naMgr_cakSvc_InitGlobalInfo(void)
{
	// cat info
	HxSTD_memset(&s_stCatInfoList, 0x00, (sizeof(PSI_CAT_INFO_t) * MAX_TUNER_NO));

	// pmt info
	HxSTD_memset(s_stPmtInfoList, 0x00, (sizeof(PSI_PMT_INFO_t) * MAX_NO_SESSION));

	// cas service
	HxSTD_memset(s_stCaSvcInfoList, 0x00, (sizeof(PSI_CA_SERVICE_INFO_t) * MAX_NO_SESSION));

	return ERR_OK;
}


// TODO: add ca_psi7.c
static HERROR _naMgr_cakSvc_GetTable(Handle_t hAction, pCaPlayInst_t *ppstCaPlayInst)
{
	TUnsignedInt8 i;

	if (ppstCaPlayInst == NULL)
		return ERR_FAIL;

	WAIT_FOR_SEM(s_stNaCaInst.ulSem)
	for (i=0; i<MAX_NO_SESSION; i++)
	{
		if (CHECK_SAME_HANDLE(s_stNaCaInst.stCaPlayInstList[i].hAction, hAction))
			break;

		if (CHECK_SAME_ACTION_ID(s_stNaCaInst.stCaPlayInstList[i].hAction, hAction))
			break;
	}

	if (i < MAX_NO_SESSION)
	{
		*ppstCaPlayInst = &s_stNaCaInst.stCaPlayInstList[i];
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return ERR_OK;
	}

	RELEASE_SEM(s_stNaCaInst.ulSem)
	return ERR_FAIL;
}

static HERROR _naMgr_cakSvc_GetAvailbleTable(pCaPlayInst_t *ppstCaPlayInst)
{
	TUnsignedInt8 i;

	if (ppstCaPlayInst == NULL)
		return ERR_FAIL;

	WAIT_FOR_SEM(s_stNaCaInst.ulSem)
	for (i=0; i<MAX_NO_SESSION; i++)
	{
		if (CHECK_SAME_HANDLE(s_stNaCaInst.stCaPlayInstList[i].eInstState, eInstStopState))
			break;
	}

	if (i < MAX_NO_SESSION)
	{
		*ppstCaPlayInst = &s_stNaCaInst.stCaPlayInstList[i];
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return ERR_OK;
	}

	RELEASE_SEM(s_stNaCaInst.ulSem)
	return ERR_FAIL;
}

static HERROR _naMgr_cakSvc_GetInstanceID(Handle_t *phCasHandle)
{
	if (phCasHandle == NULL)
		return ERR_FAIL;

	HxLOG_Print("Cas Instance Id(0x%x)\n", s_stNaCaInst.hCasInst);
	*phCasHandle = s_stNaCaInst.hCasInst;
	return ERR_OK;
}

static HERROR _naMgr_cakSvc_SetStatus(Handle_t hAction, CaInstState_t eInstState)
{
	pCaPlayInst_t pstCaPlayInst;

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable(0x%x, 0x%x)\n", hAction, eInstState);
		return ERR_FAIL;
	}

	pstCaPlayInst->eInstState = eInstState;
	if (eInstState == eInstStopState)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
{
		TUnsignedInt8 i;

		for (i=0; i<MAX_NUM_OF_CA_MAC_DESC; i++)
		{
			if (pstCaPlayInst->stCaMacDesc[i].pucRawData != NULL)
			{
				OxCAS_Free(pstCaPlayInst->stCaMacDesc[i].pucRawData);
			}
			HxSTD_memset(&pstCaPlayInst->stCaMacDesc[i], 0x00, sizeof(PSI_CA_MAC_DESC_t));
		}
}
#endif
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
		if(pstCaPlayInst->eActType == eCAS_ActionType_Live)
		{
			HxSTD_memcpy(&s_stNaCaInst.stCaLastLiveState, pstCaPlayInst, sizeof(CaPlayInst_t));
		}
#endif
		if(pstCaPlayInst->pucPmt != NULL)
		{
			OxCAS_Free(pstCaPlayInst->pucPmt);
			pstCaPlayInst->pucPmt = NULL;
		}
		pstCaPlayInst->hAction = HANDLE_NULL;
		pstCaPlayInst->hSvc = HANDLE_NULL;
	}
	return ERR_OK;
}

#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
static HERROR _naMgr_cakSvc_FindEmmInfo(TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucIndex)
{
	TUnsignedInt8 ucIndex;

	if (pucIndex == NULL)
		return ERR_FAIL;

	for (ucIndex=0; ucIndex<MAX_TUNER_NO; ucIndex++)
	{
		if ( (s_stNaCaInst.stCaEmmInfo[ucIndex].bSetEmm == TRUE) && (s_stNaCaInst.stCaEmmInfo[ucIndex].usUniqueId == usUniqueId) )
		{
			HxLOG_Print("[%s:%d] found the slot(%d) with the UniqueID(0x%x)\n", __FUNCTION__, __HxLINE__, ucIndex, usUniqueId);
			*pucIndex = ucIndex;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


static HERROR _naMgr_cakSvc_CheckEmmInfo(TUnsignedInt16 usUniqueId, Handle_t hCurrAction, Handle_t *phSymAction)
{
	TUnsignedInt8 ucIndex;
	Handle_t hTempAction = HANDLE_NULL;

	if (phSymAction == NULL)
		return ERR_FAIL;

	WAIT_FOR_SEM(s_stNaCaInst.ulSem)
	HxLOG_Print("Unique_Id (0x%x), Curr_Action (0x%x)\n", usUniqueId, hCurrAction);

	if (_naMgr_cakSvc_FindEmmInfo(usUniqueId, &ucIndex) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_FindEmmInfo.\n");
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return ERR_FAIL;
	}

	if (s_stNaCaInst.stCaEmmInfo[ucIndex].bSetEmm == TRUE)
	{
		TUnsignedInt8 i;
		TUnsignedInt8 ucCnt = 0;

		for (i=0; i<MAX_NO_SESSION; i++)
		{
			if ( (CHECK_SAME_HANDLE(s_stNaCaInst.stCaPlayInstList[i].eInstState, eInstStartState))
				&& (CHECK_SAME_HANDLE(s_stNaCaInst.stCaPlayInstList[i].usUniqueId, usUniqueId)) )
			{
				if (!CHECK_SAME_HANDLE(s_stNaCaInst.stCaPlayInstList[i].hAction, hCurrAction))
				{
					ucCnt++;
					hTempAction = s_stNaCaInst.stCaPlayInstList[i].hAction;
				}
			}
		}

		HxLOG_Print("The Count is (%d)\n", ucCnt);
		if (ucCnt == 0)
		{
			*phSymAction = s_stNaCaInst.stCaEmmInfo[ucIndex].hSymAction;
			RELEASE_SEM(s_stNaCaInst.ulSem)
			return ERR_OK;
		}
	}
	HxLOG_Print("Replaced Action (0x%x)\n", hTempAction);
	s_stNaCaInst.stCaEmmInfo[ucIndex].hAction = hTempAction;
	RELEASE_SEM(s_stNaCaInst.ulSem)
	return ERR_FAIL;
}


static HERROR _naMgr_cakSvc_GetEmmInfo(TUnsignedInt16 usUniqueId, Handle_t *phAction, Handle_t *phSymAction)
{
	TUnsignedInt8 ucIndex;

	if (phAction == NULL)
		return ERR_FAIL;

	WAIT_FOR_SEM(s_stNaCaInst.ulSem)

	if (_naMgr_cakSvc_FindEmmInfo(usUniqueId, &ucIndex) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_FindEmmInfo.\n");
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return ERR_FAIL;
	}

	*phAction = s_stNaCaInst.stCaEmmInfo[ucIndex].hAction;
	*phSymAction = s_stNaCaInst.stCaEmmInfo[ucIndex].hSymAction;
	RELEASE_SEM(s_stNaCaInst.ulSem)
	return ERR_OK;
}


static HERROR _naMgr_cakSvc_SetEmmInfo(Handle_t hAction, TUnsignedInt16 usUniqueId, Handle_t *phSymAction)
{
	TUnsignedInt8 ucIndex;

	if (phSymAction == NULL)
		return ERR_FAIL;

	WAIT_FOR_SEM(s_stNaCaInst.ulSem)

	if (_naMgr_cakSvc_FindEmmInfo(usUniqueId, &ucIndex) == ERR_OK)
	{
		HxLOG_Print("[%s:%d] Found registed emm filtering slot. : Index(%d), usUniqueId(0x%x)\n", __FUNCTION__, __HxLINE__, ucIndex, usUniqueId);
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return ERR_FAIL;
	}

	for (ucIndex=0; ucIndex<MAX_TUNER_NO; ucIndex++)
	{
		if (s_stNaCaInst.stCaEmmInfo[ucIndex].bSetEmm == FALSE)
			break;
	}

	if (ucIndex >= MAX_TUNER_NO)
	{
		HxLOG_Error("Not found the slot for emm filtering.\n");
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return ERR_FAIL;
	}

	HxLOG_Print("Index (%d) hAction (0x%x) Unique_Id (0x%x)\n", ucIndex, hAction, usUniqueId);

	s_stNaCaInst.stCaEmmInfo[ucIndex].bSetEmm = TRUE;
	s_stNaCaInst.stCaEmmInfo[ucIndex].hAction = hAction;
	s_stNaCaInst.stCaEmmInfo[ucIndex].hSymAction = ((EMM_HANDLE_ID << 24) | usUniqueId);
	s_stNaCaInst.stCaEmmInfo[ucIndex].usUniqueId = usUniqueId;
	*phSymAction = s_stNaCaInst.stCaEmmInfo[ucIndex].hSymAction;
	RELEASE_SEM(s_stNaCaInst.ulSem)
	return ERR_OK;
}


static HERROR _naMgr_cakSvc_UnsetEmmInfo(TUnsignedInt16 usUniqueId)
{
	TUnsignedInt8 ucIndex;

	WAIT_FOR_SEM(s_stNaCaInst.ulSem)
	if (_naMgr_cakSvc_FindEmmInfo(usUniqueId, &ucIndex) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_FindEmmInfo.\n");
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return ERR_FAIL;
	}

	HxSTD_memset(&s_stNaCaInst.stCaEmmInfo[ucIndex], 0x00, sizeof(CaEmmInfo_t));
	RELEASE_SEM(s_stNaCaInst.ulSem)
	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
static void _naMgr_cakSvc_SetParentalControl(NaCasMsg_t *pstMsg)
{
	HERROR		ulErrCode;
	Handle_t	hAction;
	HUINT8		ucIndex;
	PSI_CA_SERVICE_INFO_t	*pstSvcInfo;

	TCaRequest					*pxRequest;
	TCaRequestStatus			eStatus;
	TCaProcessingStatus			eProcStatus;

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	if (pstMsg == NULL)
	{
		HxLOG_Error("[%s:%d] pstMsg is NULL\n", __FUNCTION__, __HxLINE__);
		return;
	}

	hAction		= pstMsg->hWnd;

	ulErrCode = _naMgr_cakSvc_FindSvcInfoIdxByAction(hAction, &ucIndex);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("[%s:%d] _naMgr_cakSvc_FindSvcInfoIdxByAction() error\n", __FUNCTION__, __HxLINE__);
		return;
	}

	pstSvcInfo = _naMgr_cakSvc_GetCasSvcInfoPtrByIdx(ucIndex);
	if (pstSvcInfo == NULL)
	{
		HxLOG_Error("[%s:%d] _naMgr_cakSvc_GetCasSvcInfoPtrByIdx return NULL\n", __FUNCTION__, __HxLINE__);
		return;
	}

	pxRequest = pstSvcInfo->pxProgDesRequest;
	if(pxRequest == NULL)
	{
		HxLOG_Error("[%s:%d] pxRequest is NULL\n", __FUNCTION__, __HxLINE__);
		return;
	}

	eStatus = caRequestSetParentalControl(pxRequest, DESCRAMBLE_CA_PARENTAL_CONTROL_THRESHOLD, DESCRAMBLE_CA_PARENTAL_CONTROL_BITMAP);
	if(eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[%s:%d] error in caRequestSetParentalControl (0x%x)\n", __FUNCTION__, __HxLINE__, eStatus);
		return;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("[%s:%d] error in caRequestSend (0x%x), hAction(0x%x)\n", __FUNCTION__, __HxLINE__, eStatus, hAction);
		return;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("[%s:%d] error in caRequestGetProcessingStatus (0x%x, 0x%x), hAction(0x%x)\n", __FUNCTION__, __HxLINE__, eStatus, eProcStatus, hAction);
		return;
	}

	HxLOG_Print("[%s:%d] Done...\n", __FUNCTION__, __HxLINE__);
}
#endif

static HERROR _naMgr_cakSvc_GetLiveAction(Handle_t *phAction)
{
	TUnsignedInt8 i;

	if (phAction == NULL)
		return ERR_FAIL;

	WAIT_FOR_SEM(s_stNaCaInst.ulSem)

	for (i=0; i<MAX_NO_SESSION; i++)
	{
		if (s_stNaCaInst.stCaPlayInstList[i].eInstState == eInstStartState)
		{
			if ( (s_stNaCaInst.stCaPlayInstList[i].eActType == eCAS_ActionType_Live)
				|| (s_stNaCaInst.stCaPlayInstList[i].eActType == eCAS_ActionType_Playback) )
			{
				break;
			}
		}
	}

	if (i < MAX_NO_SESSION)
	{
		*phAction = s_stNaCaInst.stCaPlayInstList[i].hAction;
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return ERR_OK;
	}

	RELEASE_SEM(s_stNaCaInst.ulSem)
	return ERR_FAIL;
}


static void _naMgr_cakSvc_cbStartSession(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)
{
	DbSvc_TsInfo_t 		stTsInfo;
	DbSvc_Info_t		stSvcInfo;
	pCaPlayInst_t 	pstCaPlayInst;
	TUnsignedInt8 i;

#if (0)//CONFIG_FOR_DAL_TEST_DEF == 1)
	if ( (eActType == eCAS_ActionType_Tsr_Rec) || (eActType == eCAS_ActionType_Recording) )
		return;
#endif
	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
	HxSTD_memset(&pstCaPlayInst, 0x00, sizeof(pCaPlayInst_t));

	HxLOG_Print("----------------------------------------------------------------------------------------\n");
	HxLOG_Print("[%s:%d] (Action Handle=0x%x)(Svc Handle=0x%x)(Action Type =0x%x)\n", __FUNCTION__, __HxLINE__, hAction, hSvc, eActType);
	HxLOG_Print("----------------------------------------------------------------------------------------\n");

#if defined(CONFIG_MW_CAS_NAGRA_TIVU)
	MW_CA_NA_SetTivuService(FALSE);
#endif

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) == ERR_OK)
	{
		HxLOG_Error("error already started session(0x%x)\n", hAction);
		return;
	}

	if (_naMgr_cakSvc_GetAvailbleTable(&pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetAvailbleTable\n");
		return;
	}

	pstCaPlayInst->hAction 	= hAction;
	pstCaPlayInst->hSvc 	= hSvc;
	pstCaPlayInst->eActType	= eActType;

	pstCaPlayInst->bGotPmt	= FALSE;
	pstCaPlayInst->bConnect	= FALSE;

	pstCaPlayInst->stEsPidList.usVideoPid = NULL_PID;

	for (i=0; i<MAX_NUM_OF_AUDIO_TRACK; i++)
		pstCaPlayInst->stEsPidList.ausAudioPid[i] = NULL_PID;

	pstCaPlayInst->stEsPidList.usTeletextPid = NULL_PID;
	pstCaPlayInst->stEsPidList.usSubtitlePid = NULL_PID;

	pstCaPlayInst->usUniqueId 	= 0;
	pstCaPlayInst->usSvcId 		= 0;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	HxSTD_memset(&pstCaPlayInst->stCaMacDesc, 0x00, (sizeof(PSI_CA_MAC_DESC_t) * MAX_NUM_OF_CA_MAC_DESC));
#endif

	if ( (eActType != eCAS_ActionType_Playback)
		&& (eActType != eCAS_ActionType_Tsr_Play) )
	{
		if (DB_SVC_GetTsInfoByServiceHandle(hSvc, &stTsInfo) != ERR_OK)
		{
			HxLOG_Error("error in DB_SVC_GetTsInfoByServiceHandle\n");
			return;
		}

		HxLOG_Print("Ts_UniqueId(0x%x)\n", stTsInfo.usUniqueId);
		pstCaPlayInst->usUniqueId = stTsInfo.usUniqueId;

		if (DB_SVC_GetServiceInfo(hSvc, &stSvcInfo) != ERR_OK)
		{
			HxLOG_Error("error in DB_SVC_GetServiceInfo\n");
			return;
		}

		HxLOG_Print("Ts_index(0x%x), Triple_ID(0x%x, 0x%x, 0x%x)\n",  DbSvc_GetTsIdx(&stSvcInfo), DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo));
		pstCaPlayInst->usSvcId = DbSvc_GetSvcId(&stSvcInfo);
	}

	NAMGR_SetSessionStatus(hAction, TRUE);
	_naMgr_cakSvc_SetStatus(hAction, eInstStartState);

#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SVC[%s:%d] ----- (%d) ----- Start Session hAction(0x%x)]	[%d]	[%s]	[%d MHz] ----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction, stSvcInfo.usLcn, stSvcInfo.szSvcName, (stTsInfo.stTuningParam.cab.ulFreq / 1000));
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif

#if (CONFIG_SCRAMBLE_PLAYBACK == 0)
	if ( (eActType == eCAS_ActionType_Playback)
		|| (eActType == eCAS_ActionType_Tsr_Play) )
	{
		return;
	}
#endif

	CA_NA_PsiStartService(DbSvc_GetSvcId(&stSvcInfo), hAction, hSvc, eActType);
}

static void _naMgr_cakSvc_cbStopSession(Handle_t hAction, Handle_t hNextSvc)
{
	pCaPlayInst_t pstCaPlayInst;


#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SVC[%s:%d] ----- (%d) ----- Stop Session hAction(0x%08x), Next Svc Handle=(0x%08x) ----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction, hNextSvc);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif

	HxLOG_Print("----------------------------------------------------------------------------------------\n");
	HxLOG_Print("[%s:%d] (Action Handle=0x%x, Next Svc Handle=0x%x)\n", __FUNCTION__, __HxLINE__, hAction, hNextSvc);
	HxLOG_Print("----------------------------------------------------------------------------------------\n");

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable\n");
		return;
	}

	/*	SERVICE START로 받은 ActionHandle값과 SERVICE STOP시 전달되는 ActionHandle값이 차이가 있다.
		ActionId값이 같을 경우 해당 ActionHandle값으로 대체해서 넘겨주도록 하자.
		근본적인 수정이 완료되면 삭제..! */
	if(hAction != pstCaPlayInst->hAction)
	{
		HxLOG_Warning("ActionHandle => REQ (0x%x), INST (0x%x).\n", hAction, pstCaPlayInst->hAction);
		if(GET_ACTION_ID(hAction) != GET_ACTION_ID(pstCaPlayInst->hAction))
		{
			HxLOG_Error("Invalid action handle.\n");
			return;
		}

		hAction = pstCaPlayInst->hAction;
	}

#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	// TP 전환 혹은 service 완전 stop시 EMM filtering stop
	if (hNextSvc == HANDLE_NULL)	// 1.service 완전 stop (menu 진입, standby 진입 등)
	{
		Handle_t hSymAction;
		if (_naMgr_cakSvc_CheckEmmInfo(pstCaPlayInst->usUniqueId, hAction, &hSymAction) == ERR_OK)
		{
			HxLOG_Print("Stop EMM by All Service Stop... in this TP(0x%x)\n", pstCaPlayInst->usUniqueId);
			_naMgr_cakSvc_UnsetEmmInfo(pstCaPlayInst->usUniqueId);
			CA_NA_PsiStopEmm(hSymAction,pstCaPlayInst->usUniqueId);
		}
	}
	else	// 2. TP 전환시
	{
		DbSvc_TsInfo_t stTsInfo;
		if (DB_SVC_GetTsInfoByServiceHandle(hNextSvc, &stTsInfo) == ERR_OK)
		{
			if (pstCaPlayInst->usUniqueId != stTsInfo.usUniqueId)
			{
				Handle_t hSymAction = HANDLE_NULL;

				HxLOG_Print("Ts_UniqueId(0x%x, 0x%x)\n", pstCaPlayInst->usUniqueId, stTsInfo.usUniqueId);
				if (_naMgr_cakSvc_CheckEmmInfo(pstCaPlayInst->usUniqueId, hAction, &hSymAction) == ERR_OK)
				{
					HxLOG_Print("Stop EMM by TP Change...\n");
					_naMgr_cakSvc_UnsetEmmInfo(pstCaPlayInst->usUniqueId);
					CA_NA_PsiStopEmm(hSymAction, pstCaPlayInst->usUniqueId);
				}
			}
		}
	}
#else
	// TP 전환 혹은 service 완전 stop시 EMM filtering stop
	// 1.service 완전 stop (menu 진입, standby 진입 등)
	if (hNextSvc == HANDLE_NULL)
	{
		CA_NA_PsiStopEmm(hAction, pstCaPlayInst->usUniqueId);
	}
	else
	{
	// 2. TP 전환시
		DbSvc_TsInfo_t stTsInfo;
		if (DB_SVC_GetTsInfoByServiceHandle(hNextSvc, &stTsInfo) == ERR_OK)
		{
			if (pstCaPlayInst->usUniqueId != stTsInfo.usUniqueId)
			{
				HxLOG_Print("Ts_UniqueId(0x%x, 0x%x)\n", pstCaPlayInst->usUniqueId, stTsInfo.usUniqueId);
				CA_NA_PsiStopEmm(hAction, pstCaPlayInst->usUniqueId);
			}
		}
	}
#endif

	CA_NA_PsiStopService(hAction, pstCaPlayInst->usSvcId);

	NAMGR_SetSessionStatus(hAction, FALSE);
	_naMgr_cakSvc_SetStatus(hAction, eInstStopState);
}

static void _naMgr_cakSvc_cbSetDsc(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	TUnsignedInt16 	usVideoPid = NULL_PID;
	TUnsignedInt16	ausAudioPid[MAX_NUM_OF_AUDIO_TRACK];
	TUnsignedInt16 	usTeletextPid = NULL_PID;
	TUnsignedInt16 	usSubtitlePid = NULL_PID;
	TUnsignedInt8	ucStreamType, i;
#if (CONFIG_MW_CAS_NAGRA_PREDSC_TEST == 1)
	TUnsignedInt8	ucIdx;
#endif
	pCaPlayInst_t 	pstCaPlayInst;
#if defined(CONFIG_MW_CAS_NAGRA_SKYD) || defined(CONFIG_MW_CAS_NAGRA_KDG) || defined(CONFIG_MW_CAS_NAGRA_RCS)
	DbSvc_Info_t 		stSvcInfo;
#endif


#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SVC[%s:%d] ----- (%d) ----- Set Dsc hAction(0x%08x) ----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif

	HxLOG_Print("----------------------------------------------------------------------------------------\n");
	HxLOG_Print("[%s:%d] (Action Handle=0x%x)\n", __FUNCTION__, __HxLINE__, hAction);
	HxLOG_Print("----------------------------------------------------------------------------------------\n");

	if (pstCaDscInfo == NULL)
	{
		HxLOG_Error("pstCaDscInfo is null\n");
		return;
	}

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable\n");
		return;
	}

	for(i = 0 ; i < MAX_NUM_OF_AUDIO_TRACK ; i++)
		ausAudioPid[i] = NULL_PID;

#if (CONFIG_MW_CAS_NAGRA_PREDSC_TEST == 1)
#if (CONFIG_S_MULTI_TS_CAS == 1)
	if (pstCaPlayInst->eActType == eCAS_ActionType_Live || pstCaPlayInst->eActType == eCAS_ActionType_Recording || pstCaPlayInst->eActType == eCAS_ActionType_Tsr_Rec)
#else
	if (pstCaPlayInst->eActType == eCAS_ActionType_Live)
#endif
	{
		TUnsignedInt16 usVideoPid = 0, usAudioPid = 0;

		for (ucIdx=0; ucIdx < pstCaDscInfo->ulPidNum; ucIdx++)
		{
			if ( (pstCaDscInfo->stPidInfo[ucIdx].usPid != NULL_PID) && (pstCaDscInfo->stPidInfo[ucIdx].esType == eEsType_Video) )
			{
				usVideoPid = pstCaDscInfo->stPidInfo[ucIdx].usPid;
			}

			if ( (pstCaDscInfo->stPidInfo[ucIdx].usPid != NULL_PID) && (pstCaDscInfo->stPidInfo[ucIdx].esType == eEsType_Audio) )
			{
				usAudioPid = pstCaDscInfo->stPidInfo[ucIdx].usPid;
			}
		}
#if (CONFIG_S_MULTI_TS_CAS == 1)
		CA_NA_PreDsc_SetPidForPreDescramble(GET_ACTION_ID(hAction), usVideoPid, usAudioPid);
#else
		CA_NA_PreDsc_SetPidForPreDescramble(hAction, usVideoPid, usAudioPid);
#endif
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_SKYD) || defined(CONFIG_MW_CAS_NAGRA_KDG) || defined(CONFIG_MW_CAS_NAGRA_RCS)
	HxSTD_memset(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	DB_SVC_GetServiceInfo (pstCaPlayInst->hSvc, &stSvcInfo);

	HxLOG_Print("(Real Action Handle=0x%x, Dolby Flag=0x%x)\n", hAction, DbSvc_GetDolbyFlag(&stSvcInfo));

	if (DbSvc_GetDolbyFlag(&stSvcInfo))
	{
		for (ucIdx=0; ucIdx < pstCaDscInfo->ulPidNum; ucIdx++)
		{
			if (pstCaDscInfo->stPidInfo[ucIdx].usPid != NULL_PID && pstCaDscInfo->stPidInfo[ucIdx].esType == eEsType_Audio)
			{
				if (pstCaDscInfo->stPidInfo[ucIdx].usPid == DbSvc_GetAudioPid(&stSvcInfo))
				{
					pstCaDscInfo->stPidInfo[ucIdx].esType = eEsType_None;
					pstCaDscInfo->stPidInfo[ucIdx].usPid = NULL_PID;
				}
			}
		}
	}
	else
	{
		for (ucIdx=0; ucIdx < pstCaDscInfo->ulPidNum; ucIdx++)
		{
			if (pstCaDscInfo->stPidInfo[ucIdx].usPid != NULL_PID && pstCaDscInfo->stPidInfo[ucIdx].esType == eEsType_Audio)
			{
				if (pstCaDscInfo->stPidInfo[ucIdx].usPid == DbSvc_GetDolbyPid(&stSvcInfo))
				{
					pstCaDscInfo->stPidInfo[ucIdx].esType = eEsType_None;
					pstCaDscInfo->stPidInfo[ucIdx].usPid = NULL_PID;
				}
			}
		}
	}
#endif

#if (CONFIG_SCRAMBLE_PLAYBACK == 0)
	if ( (pstCaPlayInst->eActType == eCAS_ActionType_Playback)
		|| (pstCaPlayInst->eActType == eCAS_ActionType_Tsr_Play) )
	{
		return;
	}
#endif

	if (pstCaDscInfo->ulPidNum > 0)
	{
		TUnsignedInt8 i;
		TUnsignedInt8 ucAudioCnt = 0;

		for (i=0; i<pstCaDscInfo->ulPidNum; i++)
		{
#if defined(CONFIG_MW_CAS_NAGRA_SKYD) || defined(CONFIG_MW_CAS_NAGRA_KDG) || defined(CONFIG_MW_CAS_NAGRA_RCS)
			// Pre-program serivce(RTL Television, Sat.1, Prosieben)에서 PCM,  DOLBY PID만 존재할 경우 PID가 변경된 경우 PID가 0인
			// 경우가 발생해서 우선 0은 Skip 하도록 수정함 by mslee 2009-11-07
			if (pstCaDscInfo->stPidInfo[i].usPid != NULL_PID && pstCaDscInfo->stPidInfo[i].usPid != 0)
#else
			if (pstCaDscInfo->stPidInfo[i].usPid != NULL_PID)
#endif
			{
				switch (pstCaDscInfo->stPidInfo[i].esType)
				{
					case eEsType_Video:
						usVideoPid = pstCaDscInfo->stPidInfo[i].usPid;
						break;

					case eEsType_Audio:
						if (ucAudioCnt < MAX_NUM_OF_AUDIO_TRACK)
							ausAudioPid[ucAudioCnt++] = pstCaDscInfo->stPidInfo[i].usPid;
						break;

					case eEsType_Teletext:
						usTeletextPid = pstCaDscInfo->stPidInfo[i].usPid;
						break;

					case eEsType_Subtitle:
						usSubtitlePid = pstCaDscInfo->stPidInfo[i].usPid;
						break;

					default:
						break;
				}
			}
		}

		HxLOG_Print("Status (0x%x, 0x%x)\n", pstCaPlayInst->bGotPmt, pstCaPlayInst->bConnect);
		HxLOG_Print("PID_1 (0x%x, 0x%x, 0x%x, 0x%x)\n",usVideoPid, ausAudioPid[0], usTeletextPid, usSubtitlePid);

		if (pstCaPlayInst->bGotPmt == FALSE) 	// Session start 이후 첫 PID setting
		{
			if (usVideoPid != NULL_PID)
				pstCaPlayInst->stEsPidList.usVideoPid = usVideoPid;

			for (i=0; i<ucAudioCnt; i++)
				pstCaPlayInst->stEsPidList.ausAudioPid[i] = ausAudioPid[i];

			if (usTeletextPid != NULL_PID)
				pstCaPlayInst->stEsPidList.usTeletextPid = usTeletextPid;

			if (usSubtitlePid != NULL_PID)
				pstCaPlayInst->stEsPidList.usSubtitlePid = usSubtitlePid;

			HxLOG_Print("PID_2 (0x%x, 0x%x, 0x%x, 0x%x)\n", pstCaPlayInst->stEsPidList.usVideoPid, pstCaPlayInst->stEsPidList.ausAudioPid[0],
																									pstCaPlayInst->stEsPidList.usTeletextPid, pstCaPlayInst->stEsPidList.usSubtitlePid);
		}
		else									// PID change
		{
			if (usVideoPid != NULL_PID)
			{
				if ( (usVideoPid != 0) && (pstCaPlayInst->stEsPidList.usVideoPid != usVideoPid) )
				{
					pstCaPlayInst->stEsPidList.usVideoPid = usVideoPid;
					CA_NA_PsiSetService(eEsType_Video, usVideoPid, hAction, MPEG_TYPE_VIDEO);
				}
			}

			for (i=0; i<ucAudioCnt; i++)
			{
				if (i >= MAX_NUM_OF_AUDIO_TRACK)
				{
					break;
				}
				if (ausAudioPid[i] != NULL_PID)
				{
					if ( (ausAudioPid[i] != 0) && (pstCaPlayInst->stEsPidList.ausAudioPid[i] != ausAudioPid[i]) )
					{
						pstCaPlayInst->stEsPidList.ausAudioPid[i] = ausAudioPid[i];
						if (i == 0)
							ucStreamType = MPEG_TYPE_AUDIO;
						else if(i == 1)
							ucStreamType = MPEG_TYPE_AUDIO_13818;
						else
							ucStreamType = MPEG_TYPE_PRIVATE_DATA;

						CA_NA_PsiSetService(eEsType_Audio, ausAudioPid[i], hAction, ucStreamType);
					}
				}
			}

			if (usTeletextPid != NULL_PID)
			{
				if ( (usTeletextPid != 0) && (pstCaPlayInst->stEsPidList.usTeletextPid != usTeletextPid) )
				{
					pstCaPlayInst->stEsPidList.usTeletextPid = usTeletextPid;
					CA_NA_PsiSetService(eEsType_Teletext, usTeletextPid, hAction, MPEG_TYPE_PRIVATE_DATA);
				}
			}
			if (usSubtitlePid != NULL_PID)
			{
				if ( (usSubtitlePid != 0) && (pstCaPlayInst->stEsPidList.usSubtitlePid != usSubtitlePid) )
				{
					pstCaPlayInst->stEsPidList.usSubtitlePid = usSubtitlePid;
					CA_NA_PsiSetService(eEsType_Subtitle, usSubtitlePid, hAction, MPEG_TYPE_PRIVATE_DATA);
				}
			}
		}
	}

	CA_NA_PsiSetDsc(hAction, &pstCaPlayInst->stEsPidList);
}

static void _naMgr_cakSvc_cbInstSelected(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect)
{
	pCaPlayInst_t pstCaPlayInst;
	HUINT8 *pucPmt = NULL;

	HxLOG_Print("----------------------------------------------------------------------------------------\n");
	HxLOG_Print("[%s:%d] (Action Handle=0x%x, Connectd = %d)\n", __FUNCTION__, __HxLINE__, hAction, bConnect);
	HxLOG_Print("----------------------------------------------------------------------------------------\n");

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable\n");
		return;
	}

#if (CONFIG_SCRAMBLE_PLAYBACK == 0)
	if ( (pstCaPlayInst->eActType == eCAS_ActionType_Playback)
		|| (pstCaPlayInst->eActType == eCAS_ActionType_Tsr_Play) )
	{
		return;
	}
#endif

	pstCaPlayInst->bConnect = bConnect;
	pucPmt = (bConnect == TRUE) ? pstCaPlayInst->pucPmt : NULL;

	CA_NA_PsiReceivedPmt(hAction, pucPmt);
}

static void _naMgr_cakSvc_cbSetUpdatedPmt(Handle_t hAction, TUnsignedInt8 *pucPmt)
{
	pCaPlayInst_t pstCaPlayInst;
	TUnsignedInt8 i;
	TUnsignedInt8 ucStreamType;
	TUnsignedInt8 *pucPmtBuffer;
	TUnsignedInt32 ulLen;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	PSI_CA_PMT_INFO_t *pstCaPmtInfo;
#endif

	HxLOG_Print("----------------------------------------------------------------------------------------\n");
	HxLOG_Print("[%s:%d] (Action Handle=0x%x)\n", __FUNCTION__, __HxLINE__, hAction);
	HxLOG_Print("----------------------------------------------------------------------------------------\n");

	if(pucPmt == NULL)
	{
		HxLOG_Error("pucPmt NULL\n");
		return;
	}

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable\n");
		return;
	}

	HxLOG_Print("Connection status = 0x%x\n", pstCaPlayInst->bConnect);

#if (CONFIG_SCRAMBLE_PLAYBACK == 0)
	if ( (pstCaPlayInst->eActType == eCAS_ActionType_Playback)
		|| (pstCaPlayInst->eActType == eCAS_ActionType_Tsr_Play) )
	{
		if(CAS_MAX_NUM_OF_INSTANCE > 1)
		{
			svc_cas_MgrSubUpdateCasMessage(pstCaPlayInst->hAction, eCAS_MSG_TYPE_Fail, eDxCAS_GROUPID_NA, NULL, eCasShowType_None);
		}
		return;
	}
#endif

	HxLOG_Print("PMT received...(0x%x)\n", (unsigned int)hAction);

#if (CONFIG_MW_CAS_NAGRA_PREDSC_TEST == 0)
	if (MW_CA_NA_ReportCurrScStatus(hAction) != ERR_OK)
	{
		HxLOG_Print("error in MW_CA_NA_ReportCurrScStatus\n");
	}
#endif

	ulLen = get12bit(pucPmt + 1) + 3;
	pucPmtBuffer = (TUnsignedInt8*)OxCAS_Malloc(ulLen);
	if (pucPmtBuffer == NULL )
	{
		HxLOG_Error("Action:0x%x, pucPmtBuffer is NULL\n", hAction);
		return;
	}

	HxSTD_memcpy(pucPmtBuffer, pucPmt, ulLen);

	if(pstCaPlayInst->pucPmt != NULL)
	{
		OxCAS_Free(pstCaPlayInst->pucPmt);
	}

	pstCaPlayInst->pucPmt = pucPmtBuffer;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if (_naMgr_cakSvc_ParsePmt(hAction, pstCaPlayInst->pucPmt) != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_PSI_siExt_ParsePmt\n");
		return;
	}
  #if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
  #else
	if (pstCaPlayInst->eActType == eCAS_ActionType_Live)
	{
		if (_naMgr_cakSvc_GetPmtInfo(hAction, &pstCaPmtInfo) == ERR_OK)
		{
			HxSTD_memcpy(&pstCaPlayInst->stCaMacDesc, pstCaPmtInfo->stCaMacDesc, (sizeof(PSI_CA_MAC_DESC_t) * MAX_NUM_OF_CA_MAC_DESC));
		}
	}
  #endif
#endif

	if (pstCaPlayInst->bGotPmt == FALSE)
	{
		pstCaPlayInst->bGotPmt = TRUE;
		HxLOG_Print("PID (0x%x, 0x%x, 0x%x, 0x%x)\n", pstCaPlayInst->stEsPidList.usVideoPid, pstCaPlayInst->stEsPidList.ausAudioPid[0], pstCaPlayInst->stEsPidList.usTeletextPid, pstCaPlayInst->stEsPidList.usSubtitlePid);

		CA_NA_PsiSetService(eEsType_Video, pstCaPlayInst->stEsPidList.usVideoPid, hAction, MPEG_TYPE_VIDEO);

		for (i=0; i<MAX_NUM_OF_AUDIO_TRACK; i++)
		{
			if (pstCaPlayInst->stEsPidList.ausAudioPid[i] != NULL_PID)
			{
				if (i == 0)
					ucStreamType = MPEG_TYPE_AUDIO;
				else if(i == 1)
					ucStreamType = MPEG_TYPE_AUDIO_13818;
				else
					ucStreamType = MPEG_TYPE_PRIVATE_DATA;

				CA_NA_PsiSetService(eEsType_Audio, pstCaPlayInst->stEsPidList.ausAudioPid[i], hAction, ucStreamType);
			}
		}

		CA_NA_PsiSetService(eEsType_Teletext, pstCaPlayInst->stEsPidList.usTeletextPid, hAction, MPEG_TYPE_PRIVATE_DATA);
		CA_NA_PsiSetService(eEsType_Subtitle, pstCaPlayInst->stEsPidList.usSubtitlePid, hAction, MPEG_TYPE_PRIVATE_DATA);
	}

	if(pstCaPlayInst->bConnect)
	{
		CA_NA_PsiReceivedPmt(hAction, pstCaPlayInst->pucPmt);
	}
}

static void _naMgr_cakSvc_cbSetUpdatedCat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucCat)
{
	pCaPlayInst_t pstCaPlayInst;
#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	Handle_t hEmmAction, hSymAction;
#endif
	TUnsignedInt32	ulLen=0;
	TUnsignedInt8	*pucCatBuffer = NULL;

	HxLOG_Print("----------------------------------------------------------------------------------------\n");
	HxLOG_Print("[%s:%d] (Action Handle=0x%x)\n", __FUNCTION__, __HxLINE__, hAction);
	HxLOG_Print("----------------------------------------------------------------------------------------\n");

	if (_naMgr_cakSvc_GetTable(hAction, &pstCaPlayInst) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_GetTable\n");
		return;
	}

#if (CONFIG_SCRAMBLE_PLAYBACK == 0)
	if ( (pstCaPlayInst->eActType == eCAS_ActionType_Playback)
		|| (pstCaPlayInst->eActType == eCAS_ActionType_Tsr_Play) )
	{
		HxLOG_Error("ignore\n");
		return;
	}
#endif

	if (pucCat == NULL)
	{
		HxLOG_Error("Action:0x%x, pucCat is NULL\n", hAction);
		return;
	}

	if(pucCat[0] != CAT_TABLE_ID)
	{
		HxLOG_Error("It is not CAT table (0x%x)\n", pucCat[0]);
		return;
	}

	ulLen = get12bit(pucCat+1) + 3;
	pucCatBuffer = (TUnsignedInt8*)OxCAS_Malloc(ulLen);
	if (pucCatBuffer == NULL )
	{
		HxLOG_Error("Action:0x%x, pucCatBuffer is NULL\n", hAction);
		return;
	}

	HxSTD_memcpy(pucCatBuffer, pucCat, ulLen);

#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	if (_naMgr_cakSvc_SetEmmInfo(hAction, pstCaPlayInst->usUniqueId, &hSymAction) == ERR_OK)
	{
		CA_NA_PsiRecievedCat(hSymAction, pstCaPlayInst->usUniqueId, pucCatBuffer);
	}
	else
	{
		/* 현재 setting된 TP의 Action ID와 동일하면 Update 처리 */
		if (_naMgr_cakSvc_GetEmmInfo(pstCaPlayInst->usUniqueId, &hEmmAction, &hSymAction) == ERR_OK)
		{
			if (CHECK_SAME_ACTION_ID(hAction, hEmmAction))
			{
				HxLOG_Print("Update CAT .... Givne Action(0x%x), Emm Action(0x%x), Sym Action(0x%x)\n", hAction, hEmmAction, hSymAction);
				CA_NA_PsiRecievedCat(hSymAction, pstCaPlayInst->usUniqueId, pucCatBuffer);
			}
			else
			{
				HxLOG_Print("Stop EMM by Open Other Action...\n");
				_naMgr_cakSvc_UnsetEmmInfo(pstCaPlayInst->usUniqueId);
				CA_NA_PsiStopEmm(hSymAction, pstCaPlayInst->usUniqueId);

				if (_naMgr_cakSvc_SetEmmInfo(hAction, pstCaPlayInst->usUniqueId, &hSymAction) == ERR_OK)
				{
					CA_NA_PsiRecievedCat(hSymAction, pstCaPlayInst->usUniqueId, pucCatBuffer);
				}
				else
				{
					OxCAS_Free(pucCatBuffer);
				}
			}
		}
		else
		{
			OxCAS_Free(pucCatBuffer);
		}
	}
#else
	CA_NA_PsiRecievedCat(hAction, pstCaPlayInst->usUniqueId, pucCatBuffer);
#endif
}

static void _naMgr_cakSvc_cbSetUpdatedTdt(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucTime)
{
	HxLOG_Print("----------------------------------------------------------------------------------------\n");
	HxLOG_Print("[%s:%d] (Action Handle=0x%x)\n", __FUNCTION__, __HxLINE__, hAction);
	HxLOG_Print("----------------------------------------------------------------------------------------\n");
}

static void _naMgr_cakSvc_cbCheckLastMsg(Handle_t hAction, SvcCas_MsgType_e *peMsgType, DxCAS_GroupId_e *peCasId, void **ppMessage, SvcCas_ShowType_e *peShowType)
{
	HxLOG_Print("----------------------------------------------------------------------------------------\n");
	HxLOG_Print("[%s:%d] (Action Handle=0x%x)\n", __FUNCTION__, __HxLINE__, hAction);
	HxLOG_Print("----------------------------------------------------------------------------------------\n");
}

static HERROR _naMgr_cakSvc_InitTable(void)
{
	TUnsignedInt8 	i, j;

	HxSTD_memset(&s_stNaCaInst, 0x00, sizeof(CaNaInst_t));

	if(VK_SEM_Create((unsigned long*)&s_stNaCaInst.ulSem, "ca_psi_if_ext", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
	{
		HxLOG_Error("VK_SEM_Create() return error\n");
		return ERR_FAIL;
	}

	for (i = 0 ; i < MAX_NO_SESSION ; i++)
	{
		s_stNaCaInst.stCaPlayInstList[i].hAction 	= HANDLE_NULL;
		s_stNaCaInst.stCaPlayInstList[i].hSvc 		= HANDLE_NULL;

		s_stNaCaInst.stCaPlayInstList[i].bGotPmt	= FALSE;
		s_stNaCaInst.stCaPlayInstList[i].bConnect 	= FALSE;

		s_stNaCaInst.stCaPlayInstList[i].eInstState	= eInstStopState;
		s_stNaCaInst.stCaPlayInstList[i].eActType 	= eCAS_ActionType_None;

		s_stNaCaInst.stCaPlayInstList[i].stEsPidList.usVideoPid = NULL_PID;

		for (j=0; j<MAX_NUM_OF_AUDIO_TRACK; j++)
			s_stNaCaInst.stCaPlayInstList[i].stEsPidList.ausAudioPid[j] = NULL_PID;

		s_stNaCaInst.stCaPlayInstList[i].stEsPidList.usTeletextPid = NULL_PID;
		s_stNaCaInst.stCaPlayInstList[i].stEsPidList.usSubtitlePid = NULL_PID;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		HxSTD_memset(&s_stNaCaInst.stCaPlayInstList[i].stCaMacDesc, 0x00, (sizeof(PSI_CA_MAC_DESC_t) * MAX_NUM_OF_CA_MAC_DESC));
#endif
	}

	return ERR_OK;
}

static HERROR _naMgr_cakSvc_RegisterCb(void)
{
	SvcCas_CbFuncList_t cbList;
	HUINT32				ulCasInstId;

	HxSTD_memset(&cbList, 0, sizeof(SvcCas_CbFuncList_t));

	cbList.pfnCbStartSession		= _naMgr_cakSvc_cbStartSession;
	cbList.pfnCbStopSession		= _naMgr_cakSvc_cbStopSession;
	cbList.pfnCbSetDSC 			= _naMgr_cakSvc_cbSetDsc;
	cbList.pfnCbInstSelected		= _naMgr_cakSvc_cbInstSelected;
	cbList.pfnCbUpdatePmt		= _naMgr_cakSvc_cbSetUpdatedPmt;
	cbList.pfnCbUpdateCat		= _naMgr_cakSvc_cbSetUpdatedCat;
	cbList.pfnCbUpdateSdt;
	cbList.pfnCbUpdateNit;
	cbList.pfnCbUpdateEit;
	cbList.pfnCbUpdateTdt		= _naMgr_cakSvc_cbSetUpdatedTdt;
	cbList.pfnCbUpdateBat;
	cbList.pfnCbCheckLastMsg	= _naMgr_cakSvc_cbCheckLastMsg;

	if(svc_cas_MgrSubRegisterCb(eCasInst_Embedded, eDxCAS_GROUPID_NA, 0, &cbList, 1, &ulCasInstId) != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrSubRegisterCb() error\n");
		return ERR_FAIL;
	}
	s_stNaCaInst.hCasInst = ulCasInstId;

	return ERR_OK;
}


/**************************************************************************************************/
#define ________CA_CAK_SVC_Public_Functions_Body________________________________________________
/**************************************************************************************************/
HERROR CA_NA_CakSvcInitialization(void)
{
	if (_naMgr_cakSvc_InitGlobalInfo() != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_InitGlobalInfo\n");
		return ERR_FAIL;
	}

	if (VK_SEM_Create((unsigned long*)&s_ulPsiSemId, (TChar *)"ca_psi_sem", VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Error("error in VK_SEM_Create\n");
		return ERR_FAIL;
	}

	if (VK_CreateMessageQueue(256, (unsigned long*)&s_ulPsiQueueId) != ERR_OK)
	{
		HxLOG_Error("error in VK_CreateMessageQueue\n");
		return ERR_FAIL;
	}

// TODO: add ca_psi7.c
	if(_naMgr_cakSvc_InitTable() != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_RegisterCb\n");
		return ERR_FAIL;
	}

	if(_naMgr_cakSvc_RegisterCb() != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakSvc_RegisterCb\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}




/**************************************************************************************************/
#define ________CA_CAK_SVC_External_Functions_Body____________________________________________
/**************************************************************************************************/
HERROR CA_NA_CakSvcStartup(void)
{
	TSignedInt32 nErrCode;

	nErrCode = VK_TASK_Create(_naMgr_cakSvc_Task,
								NA_PSI_TASK_PRIORITY,
								NA_PSI_TASK_STACK_SIZE,
								NA_PSI_TASK_NAME,
								NULL,
								(unsigned long*)&s_ulPsiTaskId,
								0);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("error in VK_TASK_Create 0x%x\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_TASK_Start(s_ulPsiTaskId);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("error in VK_TASK_Start 0x%x\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}


void CA_NA_PsiStartService(TUnsignedInt16 usSvcID, TUnsignedInt32 hAction, TUnsignedInt32 hSvc, SvcCas_ActionType_e eActType)
{
	NaCasMsg_t 	stMsg;
	TSignedInt32	nErrCode;

	stMsg.wMsg 		= NA_SERVICE_ID;
	stMsg.hWnd 		= hAction;
	stMsg.info		= hSvc;

	stMsg.lParam1 	= (TSignedInt32)eActType;
	stMsg.lParam2 	= SERVICE_PSI_START_SERVICE;
	stMsg.lParam3 	= usSvcID;

	nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage 0x%x\n", (unsigned int)nErrCode);
	}
}

void CA_NA_PsiStopService(TUnsignedInt32	hAction, TUnsignedInt16 usSvcID)
{
	NaCasMsg_t 	stMsg;
	TSignedInt32	nErrCode;

	stMsg.wMsg 		= NA_SERVICE_ID;
	stMsg.hWnd 		= hAction;

	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= SERVICE_PSI_STOP_SERVICE ;
	stMsg.lParam3 	= usSvcID;

	nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage 0x%x\n", (unsigned int)nErrCode);
	}
}


void CA_NA_PsiStopEmm(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId)
{
	NaCasMsg_t 	stMsg;
	TSignedInt32	nErrCode;

	stMsg.wMsg 		= NA_SERVICE_ID;
	stMsg.hWnd		= hAction;

	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= SERVICE_PSI_STOP_EMM;

	stMsg.lParam3	= usUniqueId;

	nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage 0x%x\n", (unsigned int)nErrCode);
	}
}


void CA_NA_PsiPauseService(TUnsignedInt32	hAction)
{
	NaCasMsg_t 	stMsg;
	TSignedInt32	nErrCode;

	stMsg.wMsg 		= NA_SERVICE_ID;
	stMsg.hWnd 		= hAction;

	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= SERVICE_PSI_PAUSE_SERVICE ;

	nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage 0x%x\n", (unsigned int)nErrCode);
	}
}


void CA_NA_PsiRestartService(TUnsignedInt32 hAction)
{
	NaCasMsg_t 	stMsg;
	TSignedInt32	nErrCode;

	stMsg.wMsg 		= NA_SERVICE_ID;
	stMsg.hWnd 		= hAction;

	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= SERVICE_PSI_RESTART_SERVICE;

	nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage 0x%x\n", (unsigned int)nErrCode);
	}
}


void CA_NA_PsiSetService(TUnsignedInt8 ucElemType, TUnsignedInt16 usPid, TUnsignedInt32 hAction, TUnsignedInt8 ucStreamType)
{
	NaCasMsg_t 	stMsg;
	TSignedInt32	nErrCode;

	stMsg.wMsg 		= NA_SERVICE_ID;
	stMsg.hWnd 		= hAction;

	stMsg.lParam1 	= (TSignedInt32)ucElemType;
	stMsg.lParam2 	= SERVICE_PSI_SET_SERVICE;
	stMsg.lParam3 	= usPid;
	stMsg.info		= ucStreamType;

	nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage 0x%x\n", (unsigned int)nErrCode);
	}
}


void CA_NA_PsiReceivedPmt(TUnsignedInt32 hAction, TUnsignedInt8 *pucPmt)
{
	NaCasMsg_t 	stMsg;
	TSignedInt32	nErrCode;

	stMsg.wMsg 		= NA_FILTER_ID;
	stMsg.hWnd 		= hAction;

	stMsg.lParam1 	= (TSignedInt32)pucPmt;
	stMsg.lParam2 	= FILTER_PSI_PMT_ARRIVAL;

	nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage 0x%x\n", (unsigned int)nErrCode);
	}
}


void CA_NA_PsiRecievedCat(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucCat)
{
	NaCasMsg_t 	stMsg;
	TSignedInt32	nErrCode;

	stMsg.wMsg 		= NA_FILTER_ID;
	stMsg.hWnd 		= hAction;

	stMsg.lParam1 	= (TSignedInt32)pucCat;
	stMsg.lParam2 	= FILTER_PSI_CAT_ARRIVAL;
	stMsg.lParam3	= usUniqueId;

	nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage 0x%x\n", (unsigned int)nErrCode);
	}
}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
void CA_NA_PsiSetDsc(Handle_t hAction, CaEsPidList_t *pstEsPid)
{
	NaCasMsg_t 	stMsg;
	TSignedInt32	nErrCode;

	stMsg.wMsg 		= NA_SERVICE_ID;
	stMsg.hWnd 		= (TUnsignedInt32)hAction;

	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= SERVICE_PSI_SET_DSC;
	stMsg.lParam3 	= (TUnsignedInt32)pstEsPid;

	nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage 0x%x\n", (unsigned int)nErrCode);
	}
}
#endif

#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
HERROR CA_NA_PsiGetEmmAction(TUnsignedInt16 usUniqueId, Handle_t *phAction)
{
	Handle_t hSymAction;

	return _naMgr_cakSvc_GetEmmInfo(usUniqueId, phAction, &hSymAction);
}
#endif

HERROR CA_NA_PsiGetCasInstanceID(Handle_t *phCasHandle)
{
	return _naMgr_cakSvc_GetInstanceID(phCasHandle);
}

HERROR CA_NA_PsiGetLiveAction(Handle_t       *phAction)
{
	return _naMgr_cakSvc_GetLiveAction(phAction);
}


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR CA_NA_PSI_IsHdPlusService(Handle_t hAction)
{
	return _naMgr_cakSvc_IsHdPlusService(hAction);
}


HERROR CA_NA_PSI_GetSizeOfMacCaDesc(Handle_t hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucLen)
{
	return _naMgr_cakSvc_GetSizeOfMacCaDesc(hAction, usScCaSysId, pucLen);
}

HERROR CA_NA_PSI_GetContentOfMacCaDesc(Handle_t hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucDesc)
{
	return _naMgr_cakSvc_GetContentOfMacCaDesc(hAction, usScCaSysId, pucDesc);
}

HERROR CA_NA_PSI_GetActionsSameService(Handle_t hAction, Handle_t *phActions, TUnsignedInt8 *pucNum)
{
	return _naMgr_cakSvc_GetActionsSameService(hAction, phActions, pucNum);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
void NAMGR_CAKSVC_SetParentalControl(Handle_t hAction, TBoolean bUserPinPass)
{
	NaCasMsg_t		stMsg;
	TSignedInt32	nErrCode;
	Handle_t		ahAction[MAX_NO_SESSION];

	int i, j, nNumOfAction = 0;

	WAIT_FOR_SEM(s_stNaCaInst.ulSem)
	for(i = 0 ; i < MAX_NO_SESSION ; i++)
	{
		if(s_stNaCaInst.stCaPlayInstList[i].hAction == hAction)
		{
			break;
		}
	}

	if(i == MAX_NO_SESSION)
	{
		RELEASE_SEM(s_stNaCaInst.ulSem)
		return;
	}

	switch(s_stNaCaInst.stCaPlayInstList[i].eActType)
	{
		case eCAS_ActionType_Live:
			if(bUserPinPass == TRUE)
			{
				ahAction[nNumOfAction++] = s_stNaCaInst.stCaPlayInstList[i].hAction;
			}
			else
			{
				if(s_stNaCaInst.stCaLastLiveState.hSvc == s_stNaCaInst.stCaPlayInstList[i].hSvc)
				{
					if(s_stNaCaInst.stCaLastLiveState.eCaAccess == CA_ACCESS_GRANTED)
					{
						ahAction[nNumOfAction++] = s_stNaCaInst.stCaPlayInstList[i].hAction;
					}
				}
			}
			break;

		case eCAS_ActionType_Tsr_Rec:	// LIVE가 CA_ACCESS_GRANTED이면 PIN OK가 된 상태이다.
			for(j = 0 ; j < MAX_NO_SESSION ; j++)
			{
				if(s_stNaCaInst.stCaPlayInstList[j].eActType != eCAS_ActionType_Live)
					continue;

				if(s_stNaCaInst.stCaPlayInstList[j].eCaAccess != CA_ACCESS_GRANTED)
					continue;

				if(s_stNaCaInst.stCaPlayInstList[j].hSvc == s_stNaCaInst.stCaPlayInstList[i].hSvc)
					ahAction[nNumOfAction++] = s_stNaCaInst.stCaPlayInstList[i].hAction;
			}
			break;

		default:
			ahAction[nNumOfAction++] = s_stNaCaInst.stCaPlayInstList[i].hAction;
			break;
	}

	HxLOG_Print("NumberOfAction (%d)\n", nNumOfAction);

	stMsg.wMsg		= NA_SERVICE_ID;
	stMsg.lParam2	= SERVICE_PSI_SET_PARENTAL_CONTROL;

	for(i = 0 ; i < nNumOfAction ; i++)
	{
		stMsg.hWnd = ahAction[i];
		nErrCode = VK_PostMessage(s_ulPsiQueueId, (void *)&stMsg);
		if (nErrCode)
		{
			HxLOG_Error("[%s:%d] error in VK_PostMessage 0x%x\n", __FUNCTION__, __HxLINE__, nErrCode);
		}
	}

	RELEASE_SEM(s_stNaCaInst.ulSem)
}

HERROR NAMGR_CAKSVC_SetProgramAccess(Handle_t hAction, TCaAccess eCaAccess)
{
	HINT32 i;

	for(i = 0 ; i < MAX_NO_SESSION ; i++)
	{
		if(s_stNaCaInst.stCaPlayInstList[i].hAction == hAction)
		{
			s_stNaCaInst.stCaPlayInstList[i].eCaAccess = eCaAccess;
		}
	}

	return ERR_OK;
}

SvcCas_ActionType_e NAMGR_CAKSVC_GetActionTypeByActionHandle(Handle_t hAction)
{
	HINT32 i;

	for(i = 0 ; i < MAX_NO_SESSION ; i++)
	{
		if(s_stNaCaInst.stCaPlayInstList[i].hAction == hAction)
		{
			return s_stNaCaInst.stCaPlayInstList[i].eActType;
		}
	}

	return eCAS_ActionType_None;
}
#endif


/**************************************************************************************************/
#define ________CA_CAK_SVC_Debug_Functions_Body____________________________________________
/**************************************************************************************************/
#if defined(CONFIG_DEBUG)
void NAMGR_CAKSVC_PrintNagraInstance()
{
	HINT32 i, j;

	Hx_Print("\n============ NAGRA Instance ============\n");

	Hx_Print("CA Registed Instance : 0x%x\n", s_stNaCaInst.hCasInst);
	Hx_Print("LAST Live Service Handle : 0x%x\n", s_stNaCaInst.stCaLastLiveState.hSvc);
#if defined(CONFIG_DEBUG)
	Hx_Print("LAST Live Service Access : %s\n", NAMGR_GetStrAccessState(s_stNaCaInst.stCaLastLiveState.eCaAccess));
#endif

#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	for(i = 0 ; i < MAX_TUNER_NO ; i++)
	{
		Hx_Print("\n============ EMM Instance (%d) ============\n", i);
		if(s_stNaCaInst.stCaEmmInfo[i].hAction == HANDLE_NULL)
		{
			Hx_Print("\tNot Used...");
			continue;
		}

		Hx_Print("\tAction : 0x%x\n", s_stNaCaInst.stCaEmmInfo[i].hAction);
		Hx_Print("\tSymAction : 0x%x\n", s_stNaCaInst.stCaEmmInfo[i].hSymAction);
		Hx_Print("\tSetEmm : 0x%x\n", s_stNaCaInst.stCaEmmInfo[i].bSetEmm);
		Hx_Print("\tUniqueId : 0x%x\n", s_stNaCaInst.stCaEmmInfo[i].usUniqueId);
	}
#endif

	for(i = 0 ; i < MAX_NO_SESSION ; i++)
	{
		Hx_Print("\n============ PLAY Instance (%d) ============\n", i);

		if(s_stNaCaInst.stCaPlayInstList[i].hAction == HANDLE_NULL)
		{
			Hx_Print("\tNot Used...\n");
			continue;
		}

		Hx_Print("\tAction : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].hAction);
		Hx_Print("\tSvcHandle : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].hSvc);
#if defined(CONFIG_DEBUG)
		Hx_Print("\tActType : %s\n", NAMGR_GetStrCasActionType(s_stNaCaInst.stCaPlayInstList[i].eActType));
#endif
		Hx_Print("\tSvcId : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].usSvcId);
		Hx_Print("\tUniqueId : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].usUniqueId);
		if(VALID_PID(s_stNaCaInst.stCaPlayInstList[i].stEsPidList.usVideoPid))
		{
			Hx_Print("\tVideoPid : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].stEsPidList.usVideoPid);
		}
		for(j = 0 ; j < MAX_NUM_OF_AUDIO_TRACK ; j++)
		{
			if(VALID_PID(s_stNaCaInst.stCaPlayInstList[i].stEsPidList.ausAudioPid[j]))
			{
				Hx_Print("\tAudiPid[%d] : 0x%x\n", j, s_stNaCaInst.stCaPlayInstList[i].stEsPidList.ausAudioPid[j]);
			}
		}
		if(VALID_PID(s_stNaCaInst.stCaPlayInstList[i].stEsPidList.usTeletextPid))
		{
			Hx_Print("\tTeletextPid : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].stEsPidList.usTeletextPid);
		}
		if(VALID_PID(s_stNaCaInst.stCaPlayInstList[i].stEsPidList.usSubtitlePid))
		{
			Hx_Print("\tSubtitlePid : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].stEsPidList.usSubtitlePid);
		}
		Hx_Print("\tGotPmt : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].bGotPmt);
		Hx_Print("\tConnect : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].bConnect);
#if defined(CONFIG_DEBUG)
		Hx_Print("\tCaAccess : %s\n", NAMGR_GetStrAccessState(s_stNaCaInst.stCaPlayInstList[i].eCaAccess));
#endif
		Hx_Print("\tInstState : 0x%x\n", s_stNaCaInst.stCaPlayInstList[i].eInstState);
	}
}
#endif

