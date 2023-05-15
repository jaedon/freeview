/**************************************************************************************************/
#define ________CA_ICC_Private_Include____________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

//#include "_svc_cas_err.h"
#include "di_smartcard.h"
#include "svc_cas.h"
#include <_svc_cas_mgr_common.h>


/**************************************************************************************************/
#define ________CA_ICC_Golbal_Value________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_ICC_Private_Define_______________________________________________________
/**************************************************************************************************/





#if (CONFIG_FOR_DAL_TEST_DEF == 1)
#define CONFIG_RETRY_RESET				0
#else
#define CONFIG_RETRY_RESET				1
#endif

#define SMC_HANDLE_INVALID				((SciHandle_t) 0)
#define SC_INOUT_CHECK_PERIOD 			2000

#define ICC_REGISTER_CMD				0
#define ICC_STATUS_CHANGE_CMD			1

#define QUEUE_SIZE						(sizeof(TUnsignedInt32) * 4)

#define WAIT_FOR_SEM(x)				VK_SEM_Get(x);
#define RELEASE_SEM(x)					VK_SEM_Release(x);

#define VK_CreateMessageQueue(a, b)		VK_MSG_Create(a, QUEUE_SIZE, NA_ICC_MSG_Q_NAME, b, VK_SUSPENDTYPE_PRIORITY)
#define VK_PostMessage(a, b) 				VK_MSG_Send(a, b, QUEUE_SIZE)
#define VK_GetMessage(a, b)				VK_MSG_Receive(a, b, QUEUE_SIZE)
#define VK_WaitGetMessage(a, b, c)		VK_MSG_ReceiveTimeout(a, b, QUEUE_SIZE, c)


/**************************************************************************************************/
#define ________CA_ICC_Private_Type_Define_________________________________________________
/**************************************************************************************************/
typedef enum tagIccRegStatus
{
	eIccRegRegisted = 0x00,
	eIccRegUnregisted
} IccRegStatus;


typedef struct tagScAtrIInfo
{
	TUnsignedInt32 ulMinClock;
	TUnsignedInt32 ulBaudrate;

	TUnsignedInt8	ucK;
	TUnsignedInt8	ucFI;
	TUnsignedInt8	ucDI;
	TUnsignedInt8	ucN;
	TUnsignedInt8	ucWI;
	TUnsignedInt8	ucTFirst;
	TUnsignedInt8	ucT;
	TUnsignedInt8	ucIFSC;
	TUnsignedInt8	ucCWI;
	TUnsignedInt8	ucBWI;
	TUnsignedInt8	ucII;
	TUnsignedInt8	ucPI1;
	TUnsignedInt8	ucPI2;
	TUnsignedInt8	ucEDC;	//error detection code TC3
	TBoolean		bSpecificMode;
	TBoolean		bImplicitlyDefined;

	TBoolean		bPpsUnable;
	TBoolean		bExistTA1;
} scAtrInfo_t;


/**************************************************************************************************/
#define ________CA_ICC_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static TIccRegistrationId		s_ulRegId = 0;

static TUnsignedInt32			s_ulIccTaskId;
static TUnsignedInt32 			s_ulIccSemId;
static TUnsignedInt32 			s_ulIccQueueId;
#if (CONFIG_FOR_DAL_TEST_DEF == 1)
static IccRegStatus				s_eIccRegStatus = eIccRegUnregisted;
#endif

#if defined(CONFIG_PROD_PRHD2000C)
static SvcCas_DevSciScParam_t 			s_stDefaultAtrParam =
{
		eCAS_SCI_T14,	// protocol
		6000,		// Min Clock
		0,			// baudrate
		1,			// FI
		1,			// DI
		0,			// N
		13, 		// CWI
		5,			// BWI
};
#elif defined(CONFIG_PROD_NAHD2000C) && defined(CONFIG_MW_CAS_NAGRA_CAK6)
static SvcCas_DevSciScParam_t 			s_stDefaultAtrParam =
{
		eCAS_SCI_T14,	// protocol
		6000,		// Min Clock
		0,			// baudrate
		1,			// FI
		1,			// DI
		0,			// N
		13, 		// CWI
		5,			// BWI
};
#else
static SvcCas_DevSciScParam_t 			s_stDefaultAtrParam =
{
		eCAS_SCI_T0,		// protocol
		0,			// Min Clock
		0,			// baudrate
		1,			// FI
		1,			// DI
		0,			// N
		13,			// CWI
		4,			// BWI
};
#endif


static TUnsignedInt8 s_aucFiOrder[16] =
{ 0xFF, 1, 3, 4, 7, 8, 10, 0xFF, 0xFF, 2, 5, 6, 9, 11, 0xFF, 0xFF };

static TUnsignedInt8 s_aucDiOrder[16] =
{ 0xFF, 1, 2, 3, 4, 6, 8, 0xFF, 5, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static TUnsignedInt8 s_aucFiValue[16] =
{ 0xFF, 1, 9, 2, 3, 10, 11, 4, 5, 12, 6, 13, 0xFF, 0xFF, 0xFF, 0xFF };

static TUnsignedInt8 s_aucDiValue[16] =
{ 0xFF, 1, 2, 3, 4, 8, 5, 9, 6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


typedef enum ECAICC_EDCEncode
{
	ECAICC_EDCEncode_eLRC = 0,   		/* 1-byte LRC   */
	ECAICC_EDCEncode_eCRC      	  		/* 2-byte CRC */
} ECAICC_EDCEncode;


/**************************************************************************************************/
#define ________CA_ICC_Global_Value_______________________________________________________
/**************************************************************************************************/
TUnsignedInt32			g_ulNumOfSlot = 0;
ScMgrIntance_t			*g_pstScMgrInstance = NULL;
TIccEventNotification		g_pfnNaIccNotiCallback = NULL;


/**************************************************************************************************/
#define ________CA_ICC_Private_Static_Prototype_____________________________________________
/**************************************************************************************************/
static void 			local_caIcc_Task(void* p);

static HERROR			local_caIcc_GetSmcHandle(TIccSessionId ulSessionId, SciHandle_t *pHandle, TUnsignedInt8 *pucIndex);
static HERROR 		local_caIcc_IsValidNaSmartCard(TUnsignedInt8 ucIndex);
static HERROR 		local_caIcc_InitSmcMgrInstance(TUnsignedInt32 ulNumOfSlot);

#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
/* Add AtrLength parameter for Atr validation check */
static HERROR			local_caIcc_ParseAtr(TUnsignedInt8 *pucAtr, TUnsignedInt32 ucAtrLen, scAtrInfo_t *pstAtrInfo);
#else
static HERROR 		local_caIcc_ParseAtr(TUnsignedInt8 *pucAtr, scAtrInfo_t *pstAtrInfo);
#endif

static HERROR 		local_caIcc_RequestPps(SciHandle_t hSci, TUnsignedInt8 ucT, TUnsignedInt8 ucFi, TUnsignedInt8 ucDi, SvcCas_DevSciScParam_t *pstDefaultParam);
static HERROR 		local_caIcc_NegotiateByValueOrder(SciHandle_t hSci, scAtrInfo_t *pstAtrInfo, TUnsignedInt8 ucWI, TUnsignedInt8 ucBWI, SvcCas_DevSciScParam_t *pstDefaultParam);
static HERROR 		local_caIcc_Negotiate(SciHandle_t hSci, scAtrInfo_t *pstAtrInfo);

static void 			local_caIcc_ScMgrCallbackFunc_0(SciHandle_t hSci, TUnsignedInt8 ucSlotId, SvcCas_DevSciEvent_e eStatus);
static void 			local_caIcc_ScMgrCallbackFunc_1(SciHandle_t hSci, TUnsignedInt8 ucSlotId, SvcCas_DevSciEvent_e eStatus);

static void 			local_caIcc_DebugParsedAtr(scAtrInfo_t *pstAtrInfo);


/**************************************************************************************************/
#define ________CA_ICC_Private_Static_Prototype_Body________________________________________
/**************************************************************************************************/
static void local_caIcc_Task(void* p)
{
	TUnsignedInt32	i;

	TUnsignedInt8		ucSlotId;
	TUnsignedInt8		ucIndex;

	TUnsignedInt32	ulMsg[4];
	TUnsignedInt32	ulErrCode;

	TIccAccessMode	eAccMode = ICC_ACCESS_NONE;
	SvcCas_DevSciScStatus_e	eScStatus = eCAS_SCI_SC_NOT_PRESENT;
	TUnsignedInt32	ulAtrLen = 0;

	while(1)
	{
		ulErrCode = VK_WaitGetMessage(s_ulIccQueueId, (void*)&ulMsg, SC_INOUT_CHECK_PERIOD);
		if (ulErrCode != ERR_OK)
		{
			continue;
		}

		switch(ulMsg[0])
		{
			case ICC_REGISTER_CMD:
				g_pfnNaIccNotiCallback = (TIccEventNotification)ulMsg[1];
				for (i=0; i<g_ulNumOfSlot; i++)
				{
					g_pstScMgrInstance[i].eMode = ICC_ACCESS_EXCLUSIVE;
					switch(g_pstScMgrInstance[i].eCardStatus)
					{
						case eSc_In:
							ulErrCode = svc_cas_DevScGetSlotId(g_pstScMgrInstance[i].hSmcHandle, (TUnsignedInt8*)&ucSlotId);
							if (ulErrCode != ERR_OK)
							{
								HxLOG_Info("error in svc_cas_DevScGetSlotId (IN)(%x)\n", (unsigned int)ulErrCode);
								break;
							}

							g_pstScMgrInstance[i].ulSessionId = ucSlotId;
							if (g_pfnNaIccNotiCallback != NULL)
							{
							#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED,
																	g_pstScMgrInstance[i].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[i].ucAtr,
																	g_pstScMgrInstance[i].ulFreq);
							#else
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED_SINGLE_CLIENT_SUPPORT,
																	g_pstScMgrInstance[i].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[i].ucAtr,
																	g_pstScMgrInstance[i].ulFreq);
							#endif
							}

							if (eAccMode == ICC_ACCESS_NONE)
							{
								HxLOG_Info("(In) ICC_ACCESS_NONE\n");
								break;
							}
							else if (eAccMode == ICC_ACCESS_SHARED)
							{
								HxLOG_Info("(In) ICC_ACCESS_SHARED\n");
								break;
							}
							break;

						case eSc_Mute:
							ulErrCode = svc_cas_DevScGetSlotId(g_pstScMgrInstance[i].hSmcHandle, (TUnsignedInt8*)&ucSlotId);
							if (ulErrCode != ERR_OK)
							{
								HxLOG_Info("error in svc_cas_DevScGetSlotId (MUTE)(%x)\n", (unsigned int)ulErrCode);
								break;
							}

							g_pstScMgrInstance[i].ulSessionId = ucSlotId;
							if (g_pfnNaIccNotiCallback != NULL)
							{
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_MUTE,
																	g_pstScMgrInstance[i].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[i].ucAtr,
																	g_pstScMgrInstance[i].ulFreq);
							}

							if (eAccMode == ICC_ACCESS_NONE)
							{
								HxLOG_Info("(Mute) ICC_ACCESS_NONE\n");
								break;
							}
							else if (eAccMode == ICC_ACCESS_SHARED)
							{
								HxLOG_Info("(Mute) ICC_ACCESS_SHARED\n");
								break;
							}
							break;

						case eSc_Out:
							ulErrCode = svc_cas_DevScGetSlotId(g_pstScMgrInstance[i].hSmcHandle, (TUnsignedInt8*)&ucSlotId);
							if (ulErrCode != ERR_OK)
							{
								HxLOG_Info("error in svc_cas_DevScGetSlotId (OUT)(%x)\n", (unsigned int)ulErrCode);
								break;
							}

							if (g_pfnNaIccNotiCallback != NULL)
							{
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_NO_AVAILABLE_CARD,
																	g_pstScMgrInstance[i].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[i].ucAtr,
																	g_pstScMgrInstance[i].ulFreq);
							}
							g_pstScMgrInstance[i].ulSessionId = ucSlotId;

							if (eAccMode == ICC_ACCESS_NONE)
							{
								HxLOG_Info("(Remove) ICC_ACCESS_NONE\n");
								break;
							}
							else if (eAccMode == ICC_ACCESS_SHARED)
							{
								HxLOG_Info("(Remove) ICC_ACCESS_SHARED\n");
								break;
							}
							break;

						case eSc_Invalid:
							ulErrCode = svc_cas_DevScGetSlotId(g_pstScMgrInstance[i].hSmcHandle, (TUnsignedInt8*)&ucSlotId);
							if (ulErrCode != ERR_OK)
							{
								HxLOG_Info("error in svc_cas_DevScGetSlotId (INVALID)(%x)\n", (unsigned int)ulErrCode);
								break;
							}

							g_pstScMgrInstance[i].ulSessionId = ucSlotId;
							if (g_pfnNaIccNotiCallback != NULL)
							{
							#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED,
																	g_pstScMgrInstance[i].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[i].ucAtr,
																	g_pstScMgrInstance[i].ulFreq);
							#else
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED_SINGLE_CLIENT_SUPPORT,
																	g_pstScMgrInstance[i].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[i].ucAtr,
																	g_pstScMgrInstance[i].ulFreq);
							#endif
							}

							if (eAccMode == ICC_ACCESS_NONE)
							{
								HxLOG_Info("(Invalid) ICC_ACCESS_NONE\n");
								break;
							}
							else if (eAccMode == ICC_ACCESS_SHARED)
							{
								HxLOG_Info("(Invalid) ICC_ACCESS_SHARED\n");
								break;
							}
							break;
						default:
							break;
					}
				}
				break;

			case ICC_STATUS_CHANGE_CMD:
				ucIndex = (TUnsignedInt8)ulMsg[1];
				ucSlotId = (TUnsignedInt8)ulMsg[2];

				ulErrCode = svc_cas_DevScGetCardStatus(g_pstScMgrInstance[ucIndex].hSmcHandle, &eScStatus);
				if (ulErrCode != ERR_OK)
				{
					HxLOG_Info("error in DI_SC_GetCardStatus (%x)\n", (unsigned int)ulErrCode);
					break;
				}

				if (eScStatus == eCAS_SCI_SC_NOT_PRESENT)
				{
					if (g_pstScMgrInstance[ucIndex].eCardStatus == eSc_Out)
					{
						HxLOG_Info("error : icc state machine is in congestion(Out)\n");
						break;
					}

					if ((TUnsignedInt8)g_pstScMgrInstance[ucIndex].ulSessionId != ucSlotId)
					{
						HxLOG_Info("error : SMC state machine is in congestion(not coresponding slot ID)\n");
						break;
					}

					g_pstScMgrInstance[ucIndex].eCardStatus = eSc_Out;
					g_pstScMgrInstance[ucIndex].eInstState = eSc_Extracted;

					if (g_pfnNaIccNotiCallback != NULL)
					{
						eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_REMOVED,
															g_pstScMgrInstance[ucIndex].ulSessionId,
															(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
															g_pstScMgrInstance[ucIndex].ulFreq);
						if (eAccMode == ICC_ACCESS_NONE)
						{
							HxLOG_Info("(Remove) ICC_ACCESS_NONE\n");

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
							//if (ucSlotId != 0)		// Conax & Satsa의 경우는 0번째 slot 만 처리하므로 svc_cas_MgrSubUpdateCasInfo 을 여기서 부를 수 밖에 없다.
							{
								TUnsignedInt32	hCasInstance;

								if (CA_NA_PSI_ifExt_GetCasInstanceID(&hCasInstance) != ERR_OK)
								{
									HxLOG_Info("error in CA_NA_PSI_ifExt_GetCasInstanceID\n");
								}

								CASMGR_UpdateCasEtcEvent(0, hCasInstance, eDxCAS_GROUPID_NA, eSEVT_CAS_SC_REMOVED, ucSlotId, eDxCAS_GROUPID_NA, 0);
								CA_NA_PsiUpdateCasErrorInfo(ucSlotId);
							}
#endif
							break;
						}
						else if (eAccMode == ICC_ACCESS_SHARED)
						{
							HxLOG_Info("(Remove) ICC_ACCESS_SHARED\n");
							break;
						}
					}
				}
				else if (eScStatus == eCAS_SCI_SC_PRESENT)
				{
					TBoolean fScInserted = FALSE;

					for(i=0; i<g_ulNumOfSlot; i++)
					{
						if ( (g_pstScMgrInstance[i].ulSessionId == ucSlotId)
							&& (g_pstScMgrInstance[i].eCardStatus == eSc_In) )
						{
							HxLOG_Info("error : icc state machine is in congestion(In)\n");
							fScInserted = TRUE;
							break;
						}
					}

					if (fScInserted)
					{
						HxLOG_Info("error : icc state machine is in congestion(SC_IN)\n");
						break;
					}

					g_pstScMgrInstance[ucIndex].ulSessionId 	= ucSlotId;
					g_pstScMgrInstance[ucIndex].eCardStatus 	= eSc_In;
					g_pstScMgrInstance[ucIndex].eInstState 	= eSc_Inserted;
					g_pstScMgrInstance[ucIndex].eMode 		= ICC_ACCESS_EXCLUSIVE;

					HxSTD_memset((char*)g_pstScMgrInstance[ucIndex].ucAtr, 0x00, (ICC_ATR_MAX_LEN+1));
					ulErrCode = svc_cas_DevScReset(g_pstScMgrInstance[ucIndex].hSmcHandle,
												(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
												&ulAtrLen,
										#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC) && defined(CONFIG_MW_CAS_NAGRA_DALTEST)
										/* When smartcard is inserted , do cold reset !!*/
 												eCAS_SCM_RESET_TYPE_COLD,
										#else
 												eCAS_SCM_RESET_TYPE_WARM,
										#endif
												&s_stDefaultAtrParam);
				#if (CONFIG_RETRY_RESET == 1)
					if ( (ulErrCode != ERR_OK) || (ulAtrLen < 4) )
					{
						HxLOG_Info("[ICC_STATUS_CHANGE_CMD] svc_cas_DevScReset retry\n");
						ulErrCode = svc_cas_DevScReset(g_pstScMgrInstance[ucIndex].hSmcHandle,
													(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
													&ulAtrLen,
													eCAS_SCM_RESET_TYPE_COLD,
													&s_stDefaultAtrParam);
					}
				#endif
					g_pstScMgrInstance[ucIndex].ucAtrLen = (TUnsignedInt8)ulAtrLen;
					if (ulErrCode == ERR_OK)
					{
						scAtrInfo_t 	stAtrInfo;
						SvcCas_DevSciScParam_t 	stScParam;

						HxSTD_memset(&stAtrInfo, 0x00, sizeof(scAtrInfo_t));

					#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
					/* Add AtrLength parameter for Atr validation check */
						if (local_caIcc_ParseAtr(g_pstScMgrInstance[ucIndex].ucAtr, g_pstScMgrInstance[ucIndex].ucAtrLen, &stAtrInfo) != ERR_OK)
					#else
						if (local_caIcc_ParseAtr(g_pstScMgrInstance[ucIndex].ucAtr, &stAtrInfo) != ERR_OK)
					#endif
						{
							HxLOG_Info("error in local_caIcc_ParseAtr (%x)\n", (unsigned int)ulErrCode);
						#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
							ulErrCode = svc_cas_DevScNotifyScValidity(g_pstScMgrInstance[ucIndex].hSmcHandle, ePAL_SC_FLAG_DISABLE);
							if (ulErrCode != ERR_OK)
							{
								HxLOG_Info("error in svc_cas_DevScNotifyScValidity (%x)\n", (unsigned int)ulErrCode);
							}
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
							//if (ucSlotId != 0)		// Conax & Satsa의 경우는 0번째 slot 만 처리하므로 svc_cas_MgrSubUpdateCasInfo 을 여기서 부를 수 밖에 없다.
							{
								TUnsignedInt32	hCasInstance;

								if (CA_NA_PSI_ifExt_GetCasInstanceID(&hCasInstance) != ERR_OK)
								{
									HxLOG_Info("error in CA_NA_PSI_ifExt_GetCasInstanceID\n");
								}

								CASMGR_UpdateCasEtcEvent(0, hCasInstance, eDxCAS_GROUPID_NA, eSEVT_CAS_SC_INSERTED, ucSlotId, eDxCAS_GROUPID_NA, 0);
								CA_NA_PsiUpdateCasErrorInfo(ucSlotId);
							}
#endif
						#endif

						#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC) || defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_HGS1000S)
						/* CAK7 : When ATR error, We should send Mute event to DAL lib, Otherwise DAL lib will waite until time out */
							if (g_pfnNaIccNotiCallback != NULL)
							{
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_MUTE,
																	g_pstScMgrInstance[ucIndex].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
																	g_pstScMgrInstance[ucIndex].ulFreq);
							}
						#endif
							break;
						}

						if (local_caIcc_Negotiate(g_pstScMgrInstance[ucIndex].hSmcHandle, &stAtrInfo) != ERR_OK)
						{
							HxLOG_Info("error in local_caIcc_Negotiate (%x)\n", (unsigned int)ulErrCode);
						#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
							ulErrCode = svc_cas_DevScNotifyScValidity(g_pstScMgrInstance[ucIndex].hSmcHandle, ePAL_SC_FLAG_DISABLE);
							if (ulErrCode != ERR_OK)
							{
								HxLOG_Info("error in svc_cas_DevScNotifyScValidity (%x)\n", (unsigned int)ulErrCode);
							}
						#endif

							if (g_pfnNaIccNotiCallback != NULL)
							{
							#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED,
																	g_pstScMgrInstance[ucIndex].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
																	g_pstScMgrInstance[ucIndex].ulFreq);
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
							//if (ucSlotId != 0 && eAccMode == ICC_ACCESS_NONE)		// Conax & Satsa의 경우는 0번째 slot 만 처리하므로 svc_cas_MgrSubUpdateCasInfo 을 여기서 부를 수 밖에 없다.
							if (eAccMode == ICC_ACCESS_NONE)
							{
								TUnsignedInt32	hCasInstance;

								if (CA_NA_PSI_ifExt_GetCasInstanceID(&hCasInstance) != ERR_OK)
								{
									HxLOG_Info("error in CA_NA_PSI_ifExt_GetCasInstanceID\n");
								}

								CASMGR_UpdateCasEtcEvent(0, hCasInstance, eDxCAS_GROUPID_NA, eSEVT_CAS_SC_REMOVED, ucSlotId, eDxCAS_GROUPID_NA, 0);
								CA_NA_PsiUpdateCasErrorInfo(ucSlotId);
							}
#endif
							#else
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED_SINGLE_CLIENT_SUPPORT,
																	g_pstScMgrInstance[ucIndex].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
																	g_pstScMgrInstance[ucIndex].ulFreq);
							#endif
							}
							break;
						}

						if (svc_cas_DevScGetParams(g_pstScMgrInstance[ucIndex].hSmcHandle, &stScParam) != ERR_OK)
						{
							HxLOG_Info("error in svc_cas_DevScGetParams\n");
							break;
						}

						g_pstScMgrInstance[ucIndex].eInstState = eSc_ArtReceived;
						g_pstScMgrInstance[ucIndex].ulFreq = (stScParam.ulClock * 1000);
						g_pstScMgrInstance[ucIndex].eProtocol = stScParam.eProtocol;

					#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
						// validity check 호출
						if (local_caIcc_IsValidNaSmartCard(ucIndex) != ERR_OK)
						{
							ulErrCode = svc_cas_DevScNotifyScValidity(g_pstScMgrInstance[ucIndex].hSmcHandle, eCAS_SCM_FLAG_DISABLE);
							if (ulErrCode != ERR_OK)
							{
								HxLOG_Info("error in svc_cas_DevScNotifyScValidity (%x)\n", (unsigned int)ulErrCode);
								break;
							}
						}
						else
						{
							ulErrCode = svc_cas_DevScNotifyScValidity(g_pstScMgrInstance[ucIndex].hSmcHandle, eCAS_SCM_FLAG_ENABLE);
							if (ulErrCode != ERR_OK)
							{
								HxLOG_Info("error in svc_cas_DevScNotifyScValidity (%x)\n", (unsigned int)ulErrCode);
								break;
							}
						}
					#endif

						g_pstScMgrInstance[ucIndex].eInstState = eSc_ValidityReceived;
						if (g_pfnNaIccNotiCallback != NULL)
						{
						#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
							eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED,
																g_pstScMgrInstance[ucIndex].ulSessionId,
																(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
																g_pstScMgrInstance[ucIndex].ulFreq);
						#else
							eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED_SINGLE_CLIENT_SUPPORT,
																g_pstScMgrInstance[ucIndex].ulSessionId,
																(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
																g_pstScMgrInstance[ucIndex].ulFreq);
						#endif
							if (eAccMode == ICC_ACCESS_NONE)
							{
							#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1) && defined(CONFIG_MW_CAS_SATSA)
								HERROR	hErr;
								TSmartcardState eScAlarm;
								hErr = MW_CA_NA_GetCurrScStatus(ucIndex, &eScAlarm);
								if ( (hErr == ERR_OK) && (eScAlarm == CA_SMARTCARD_STATE_INVALID) )
								{
									ulErrCode = svc_cas_DevScNotifyScValidity(g_pstScMgrInstance[ucIndex].hSmcHandle, eCAS_SCM_FLAG_DISABLE);
									if (ulErrCode != ERR_OK)
									{
										HxLOG_Info("error in svc_cas_DevScNotifyScValidity (%x)\n", (unsigned int)ulErrCode);
									}
								}
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
								// Conax & Satsa의 경우는 0번째 slot 만 처리하므로 svc_cas_MgrSubUpdateCasInfo 을 여기서 부를 수 밖에 없다.
								if (MW_CA_NA_CheckTivuService())
								{
									TUnsignedInt32	hCasInstance;

									if (CA_NA_PSI_ifExt_GetCasInstanceID(&hCasInstance) != ERR_OK)
									{
										HxLOG_Info("error in CA_NA_PSI_ifExt_GetCasInstanceID\n");
									}

									CASMGR_UpdateCasEtcEvent(0, hCasInstance, eDxCAS_GROUPID_NA, eSEVT_CAS_SC_REMOVED, ucSlotId, eDxCAS_GROUPID_NA, 0);
									CA_NA_PsiUpdateCasErrorInfo(ucSlotId);
								}
#endif
							#endif
								HxLOG_Info("(Remove) ICC_ACCESS_NONE\n");
								break;
							}
							else if (eAccMode == ICC_ACCESS_SHARED)
							{
								HxLOG_Info("(Remove) ICC_ACCESS_SHARED\n");
								break;
							}
						}
						else
						{
							HxLOG_Info("card is inserted, but icc notify function is null\n");
						}
					}
					else
					{
						HxLOG_Info("Reset Error now else routine (%x)\n", (unsigned int)ulErrCode);

						#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
						/* parity error 시 deactivation timeout (410, 430) 해결 위해 여기서 deactivation 수행 */
							HxLOG_Info("error in svc_cas_DevScReset... Deactivate right now!!!! CArd insert ccase ~~~~~~~~~~~~~\n");
							if (svc_cas_DevScDeactivate(g_pstScMgrInstance[ucIndex].hSmcHandle) != ERR_OK)
							{
								HxLOG_Info("error in svc_cas_DevScDeactivate...\n");
							}
						#endif
					#if (0)
						if (ulErrCode == ERR_SCI_RESET)
							g_pstScMgrInstance[ucIndex].eCardStatus = eSc_Mute;
						else
							g_pstScMgrInstance[ucIndex].eCardStatus = eSc_Invalid;
					#else
						g_pstScMgrInstance[ucIndex].eCardStatus = eSc_Mute;
					#endif

					#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
						ulErrCode = svc_cas_DevScNotifyScValidity(g_pstScMgrInstance[ucIndex].hSmcHandle, ePAL_SC_FLAG_DISABLE);
						if (ulErrCode)
						{
							HxLOG_Info("error in svc_cas_DevScNotifyScValidity (%x)\n", (unsigned int)ulErrCode);
							break;
						}
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
						// Conax & Satsa의 경우는 0번째 slot 만 처리하므로 svc_cas_MgrSubUpdateCasInfo 을 여기서 부를 수 밖에 없다.
						if (MW_CA_NA_CheckTivuService())
						{
							TUnsignedInt32	hCasInstance;

							if (CA_NA_PSI_ifExt_GetCasInstanceID(&hCasInstance) != ERR_OK)
							{
								HxLOG_Info("error in CA_NA_PSI_ifExt_GetCasInstanceID\n");
							}

							CASMGR_UpdateCasEtcEvent(0, hCasInstance, eDxCAS_GROUPID_NA, eSEVT_CAS_SC_INSERTED, ucSlotId, eDxCAS_GROUPID_NA, 0);
							CA_NA_PsiUpdateCasErrorInfo(ucSlotId);
						}
#endif
					#endif

						g_pstScMgrInstance[ucIndex].eInstState = eSc_ValidityReceived;

						if (g_pfnNaIccNotiCallback != NULL)
						{
						#if (0)
							if (ulErrCode == ERR_SCI_MUTE)
							{
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_MUTE,
																	g_pstScMgrInstance[ucIndex].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
																	0);
							}
							else
							{
							#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED,
																	g_pstScMgrInstance[ucIndex].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
																	0);
							#else
								eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_INSERTED_SINGLE_CLIENT_SUPPORT,
																	g_pstScMgrInstance[ucIndex].ulSessionId,
																	(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
																	0);
							#endif
							}
						#else
							eAccMode = (*g_pfnNaIccNotiCallback)(ICC_EVENT_CARD_MUTE,
																g_pstScMgrInstance[ucIndex].ulSessionId,
																(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
																0);
						#endif
							if (eAccMode == ICC_ACCESS_NONE)
							{
								HxLOG_Info("(Remove) ICC_ACCESS_NONE\n");
								break;
							}
							else if (eAccMode == ICC_ACCESS_SHARED)
							{
								HxLOG_Info("(Remove) ICC_ACCESS_SHARED\n");
								break;
							}
						}
						else
						{
							HxLOG_Info("card is muted or invalid, but icc notify function is null\n");
						}
					}
				}
				break;
			default:
				break;
		}
	}
}


static HERROR local_caIcc_GetSmcHandle(TIccSessionId ulSessionId, SciHandle_t *pHandle, TUnsignedInt8 *pucIndex)
{
	TUnsignedInt32 i;

	if ( (pHandle == NULL) || (pucIndex == NULL) )
		return ERR_FAIL;

	for (i=0; i<g_ulNumOfSlot; i++)
	{
		if (g_pstScMgrInstance[i].ulSessionId == ulSessionId)
		{
			*pHandle = g_pstScMgrInstance[i].hSmcHandle;
			*pucIndex = i;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}


static HERROR local_caIcc_IsValidNaSmartCard(TUnsignedInt8 ucIndex)
 {
 	return ((g_pstScMgrInstance[ucIndex].eProtocol == eCAS_SCI_T_1_NA) ? ERR_OK : ERR_FAIL);
}


static HERROR local_caIcc_InitSmcMgrInstance(TUnsignedInt32 ulNumOfSlot)
{
	TUnsignedInt32 i;

	g_pstScMgrInstance = (ScMgrIntance_t *)OxCAS_Malloc((ulNumOfSlot *  sizeof(ScMgrIntance_t)));
	if (g_pstScMgrInstance == NULL)
	{
		HxLOG_Info("[local_caIcc_InitSmcMgrInstance] lack of momery ...\n");
		return ERR_FAIL;
	}

	for (i=0; i<ulNumOfSlot; i++)
	{
		g_pstScMgrInstance[i].hSmcHandle 	= SMC_HANDLE_INVALID;
		g_pstScMgrInstance[i].ulSessionId 	= ICC_SESSION_ID_NONE;
		g_pstScMgrInstance[i].eInstState 	= eSc_Extracted;
		g_pstScMgrInstance[i].eMode 		= ICC_ACCESS_EXCLUSIVE;
		g_pstScMgrInstance[i].eCardStatus 	= eSc_Out;
		g_pstScMgrInstance[i].ulFreq 		= 0;
		g_pstScMgrInstance[i].eProtocol 	= eCAS_SCI_T_1_NA;

		switch(i)
		{
			case 0:
				g_pstScMgrInstance[i].pfnSmcCallback = local_caIcc_ScMgrCallbackFunc_0;
				break;
			case 1:
				g_pstScMgrInstance[i].pfnSmcCallback = local_caIcc_ScMgrCallbackFunc_1;
				break;
			default:
				break;
		}
	}
	g_pfnNaIccNotiCallback = NULL;
	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
static HERROR local_caIcc_ParseAtr(TUnsignedInt8 *pucAtr, TUnsignedInt32 ucAtrLen, scAtrInfo_t *pstAtrInfo)
#else
static HERROR local_caIcc_ParseAtr(TUnsignedInt8 *pucAtr, scAtrInfo_t *pstAtrInfo)
#endif
{
	TUnsignedInt8 *pucData;
	TUnsignedInt8 ucIndex = 1;
	TUnsignedInt8 ucTS, ucT0, ucY, ucTAx, ucTBx, ucTCx, ucTDx;
	TUnsignedInt8 ucModeT = 0;
#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
	TUnsignedInt8 ucCurIndex = 0;
#endif
	TUnsignedInt8 ucSupportedProtocolTypes = 0;

	if ( (pucAtr == NULL) || (pstAtrInfo == NULL) )
	{
		HxLOG_Info("targets are null ...\n");
		return ERR_FAIL;
	}

	pucData = pucAtr;					// TS
	ucTS = *pucData;

	if ((ucTS != 0x3B) && (ucTS != 0x3F))
	{
		HxLOG_Info("Invalid TS value\n");
		return ERR_FAIL;
	}

	ucY = ucT0 = *(++pucData);
	pstAtrInfo->ucK = ucT0 & 0x0F;

	while (ucY)
	{
		HxLOG_Info("start ucIndex(%d)\t",ucIndex );
		if (ucY & 0x10)			// TA
		{
			ucTAx = *(++pucData);
			if (ucIndex == 1)			// TA1
			{
				pstAtrInfo->ucFI = (ucTAx >> 4) & 0x0F;
				pstAtrInfo->ucDI = ucTAx & 0x0F;
				pstAtrInfo->bExistTA1 = TRUE;
			}
			else if (ucIndex == 2)		// TA2
			{
				pstAtrInfo->bSpecificMode = TRUE;
				pstAtrInfo->bPpsUnable = ((ucTAx & 0x80) == 0x80);
				pstAtrInfo->bImplicitlyDefined = ((ucTAx & 0x10) == 0x10);
			}
			else if (ucIndex > 2 && pstAtrInfo->ucT == 1) // TA3
			{
				pstAtrInfo->ucIFSC = ucTAx;
			}
		}
		else
		{
			if (ucIndex == 1)
			{
				/* If TA1 is absent, use ucFfactor = 1, ucDFactor = 1 */
				pstAtrInfo->ucFI = 1;
				pstAtrInfo->ucDI = 1;
				pstAtrInfo->bExistTA1 = FALSE;
			}
		}

		if (ucY & 0x20)				// TB
		{
			ucTBx = *(++pucData);
			if (ucIndex == 1)				// TB1
			{
				pstAtrInfo->ucII = (ucTBx >> 5) & 0x07;
				pstAtrInfo->ucPI1 = ucTBx & 0x1F;
			}
			else if (ucIndex == 2)			// TB2
			{
				pstAtrInfo->ucPI2 = ucTBx;
			}
			else if (ucIndex > 2 && pstAtrInfo->ucT == 1) // TB3
			{
				pstAtrInfo->ucCWI = ucTBx & 0x0F;
				pstAtrInfo->ucBWI = (ucTBx >> 4) & 0x0F;
			}
		}

		if (ucY & 0x40)			// TC
		{
			ucTCx = *(++pucData);
			if (ucIndex == 1)			// TC1
			{
				/* if mode is T1, TC1(FF) should be transfered with original FF value for setting mininum 11etu */
				pstAtrInfo->ucN = ucTCx;
			}
			else if (ucIndex == 2)		// TC2
			{
				pstAtrInfo->ucWI = ucTCx;
			}
			else if (ucIndex > 2 && pstAtrInfo->ucT == 1) // TC3
			{
				pstAtrInfo->ucEDC = ucTCx;
			}
		}

		if (ucY & 0x80)			// TD
		{
			ucTDx = *(++pucData);
			ucModeT = (ucTDx & 0x0F);
			if (ucIndex == 1)			// TD1
			{
				pstAtrInfo->ucTFirst = ucTDx & 0x0F;
				pstAtrInfo->ucT = pstAtrInfo->ucTFirst;
			}
			else
			{
				pstAtrInfo->ucT = ucTDx & 0x0F;
			}
			ucY = ucTDx;	// next loop
		}
		else
		{
			ucY = 0;		// last loop
			if (ucIndex == 1)
			{
				/* default mode is T0 when TD1 is absent */
	        		ucModeT = 0;
			}
		}

		ucSupportedProtocolTypes |= (1 << ucModeT);
		ucIndex++;
	}

#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
{
	TUnsignedInt8 ucTck = 0;

	/* check historical byte size for checking Max ATR size validation */
	/* read the historical characters. */
	if (pstAtrInfo->ucK > 0)
	{
		pucData += pstAtrInfo->ucK;
		ucCurIndex = pucData - pucAtr + 1;
	}

	HxLOG_Info("ATR Size atrlen %d,  curindex %d , K %d\n", ucAtrLen, ucCurIndex, pstAtrInfo->ucK);

   	/* TCK character should also be present only if list of supported protocol types is not t=0 by itself. */
	/* if in T = 1 mode, then TCK byte must be present and validated */
	if (ucSupportedProtocolTypes != 1)
	{
		TSignedInt32 i = 1;
		TUnsignedInt8 ucTCKCompare = 0;

		ucTck = *pucData;		// 사용안함.
		/* Start from T0 to TCK.  Including historical bytes if they exist */
		for (i=1; i<=ucCurIndex; i++)
		{
			ucTCKCompare = ucTCKCompare ^ pucAtr[i];
			HxLOG_Info("In SmartCardValidateTCKByte inp_ucATR[%d] = %02x, 0x%2x\n", i, pucAtr[i] , ucTCKCompare);

		}
		ucCurIndex++;			// inc index

		if (ucTCKCompare != 0)
		{
			/* James Nagra :: In nagra test case, this is called with EDC/Lrc in ATR !!! */
			HxLOG_Info("Invalid TCK. : in nagra EDC, Lrc !!!!!!!!!!! \n");
			return ERR_FAIL;
		}
		else
		{
			/* TCK validated successfully */
			HxLOG_Info("Valid TCK.!! \n");
		}
	}
	HxLOG_Info("ucSupportedProtocolTypes %d, ucTck %d \n", ucSupportedProtocolTypes, ucTck);

	/* Check Validation of ATR size : Only extra chars are allowed in ATR */
	if (ucAtrLen < ucCurIndex)
	{
		/* ATR Size Error */
		HxLOG_Info("ATR Size Error\n");
		return ERR_FAIL;
	}
}
#endif

#if defined(CONFIG_DEBUG)
	local_caIcc_DebugParsedAtr(pstAtrInfo);
#endif
	return ERR_OK;
}


static HERROR local_caIcc_RequestPps(SciHandle_t hSci, TUnsignedInt8 ucT, TUnsignedInt8 ucFi, TUnsignedInt8 ucDi, SvcCas_DevSciScParam_t *pstDefaultParam)
{
	HERROR nErr = ERR_OK;
	TUnsignedInt8 aPpsRequest[4];
	TUnsignedInt8 aPpsResponse[4] = {0,0,0,0};
	TUnsignedInt8 ucNumRead;

	nErr = svc_cas_DevScSetParams(hSci, pstDefaultParam);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("err<%x>\n", nErr);
	}

	aPpsRequest[0] = 0xFF;
	aPpsRequest[1] = 0x10 | (ucT & 0x0F);
	aPpsRequest[2] = ( (ucFi << 4) & 0xF0 ) | (ucDi & 0x0F );
	aPpsRequest[3] = aPpsRequest[0] ^ aPpsRequest[1] ^ aPpsRequest[2];

	nErr = svc_cas_DevScNegotiatePTS(hSci, aPpsRequest, 4, aPpsResponse, &ucNumRead);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("err<%x>\n", nErr);
	}

	HxLOG_Print("Req : %x, %x, %x, %x\n", aPpsRequest[0], aPpsRequest[1], aPpsRequest[2], aPpsRequest[3]);
	HxLOG_Print("Res : %x, %x, %x, %x (%d)\n", aPpsResponse[0], aPpsResponse[1], aPpsResponse[2], aPpsResponse[3], ucNumRead);

	if ( (aPpsResponse[0] | aPpsResponse[1] | aPpsResponse[2] | aPpsResponse[3]) == 0 )
	{
		HxLOG_Error("err in data\n");
		return ERR_FAIL;
	}

	// skk
	// (1) response is same with request
	// (2) reponse(1-4) is same with request (1-4) and response(5-7) set to 0
	// if (memcmp(aPpsRequest, aPpsResponse, 4))
	if ( !((aPpsRequest[1] == aPpsResponse[1]) && (aPpsRequest[2] == aPpsResponse[2])) &&
		!((aPpsResponse[1] < 0x10) && ((aPpsRequest[1] & 0xF) == (aPpsResponse[1] & 0xF))) )
	{
		HxLOG_Error("reject \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


static HERROR local_caIcc_NegotiateByValueOrder(SciHandle_t hSci, scAtrInfo_t *pstAtrInfo, TUnsignedInt8 ucWI, TUnsignedInt8 ucBWI, SvcCas_DevSciScParam_t *pstDefaultParam)
{
	HERROR nErr = ERR_OK;

	TUnsignedInt8 i;
	TUnsignedInt8 ucT;
	TUnsignedInt8 ucF, ucD;
	TUnsignedInt8 ucCntF, ucCntD;
	TUnsignedInt8 ucOriCntD;

	scAtrInfo_t *p;
	SvcCas_DevSciScParam_t stScParam;

	p = pstAtrInfo;

	ucF = p->ucFI;
	ucD = p->ucDI;

	ucCntF = s_aucFiOrder[ucF];
	ucCntD = ucOriCntD = s_aucDiOrder[ucD];

	/* check if Fi and Di have valid value*/
	if (ucCntF == 0xFF)
	{
		ucCntF = 1;		// try once with default value
	}

	if (ucCntD == 0xFF)
	{
		ucCntD = 1;		// try once with default value
	}

	HxLOG_Print("[T=%x, %x]\n", p->ucT, p->ucTFirst);
	if (p->ucT == p->ucTFirst)
	{
		ucT = p->ucT;
		HxLOG_Print("UNIQUE T\n");
		while(ucCntF >= 1)		// ucCntF try
		{
			ucF = s_aucFiValue[ucCntF];
			while(ucCntD >= 1)	// ucCntD try
			{
				ucD = s_aucDiValue[ucCntD];
				HxLOG_Print("PPS T(%d) F(%d) D(%d) Dcnt(%d)\n", ucT, ucF, ucD, ucCntD);
				nErr = local_caIcc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
				if (nErr == ERR_OK)
				{
					stScParam.eProtocol = (SvcCas_DevSciProtocol_e) ucT;
					stScParam.ulClock= (TUnsignedInt32) p->ulMinClock;
					stScParam.ulBaudrate = (TUnsignedInt32) p->ulBaudrate;
					stScParam.ucFI = ucF;
					stScParam.ucDI =ucD;
					stScParam.ucN = p->ucN;
					stScParam.ucCWI = ucWI;
					stScParam.ucBWI = ucBWI;

					nErr = svc_cas_DevScSetParams(hSci, &stScParam);
					if (nErr)
					{
						HxLOG_Error("[local_cxsc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
					}
					HxLOG_Print("T(%d) F(%d) D(%d)", ucT, ucF, ucD);
					return ERR_OK;
				}
				ucCntD--;
			}
			ucCntF--;
			ucCntD = ucOriCntD;
		}
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Print("MULTIPLE T\n");
		for (i=0; i<2; i++)
		{
			if (i == 0)
				ucT = p->ucTFirst;
			else
				ucT = p->ucT;

			while (ucCntF >= 1)		// ucCntF try
			{
				ucF = s_aucFiValue[ucCntF];
				while (ucCntD >= 1)	// ucCntD try
				{
					ucD = s_aucDiValue[ucCntD];
					HxLOG_Print("PPS T(%d) F(%d) D(%d)\n", ucT, ucF, ucD);
					nErr = local_caIcc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
					if (nErr == ERR_OK)
					{
						stScParam.eProtocol = ucT;
						stScParam.ulClock= p->ulMinClock;
						stScParam.ulBaudrate = p->ulBaudrate;
						stScParam.ucFI = ucF;
						stScParam.ucDI =ucD;
						stScParam.ucN = p->ucN;
						stScParam.ucCWI = ucWI;
						stScParam.ucBWI = ucBWI;

						nErr = svc_cas_DevScSetParams(hSci, &stScParam);
						if (nErr)
						{
							HxLOG_Error("[local_cxsc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
						}
						HxLOG_Print("T(%d) F(%d) D(%d)", ucT, ucF, ucD);
						return ERR_OK;
					}
					ucCntD--;
				}
				ucCntF--;
			}
		}
		return ERR_FAIL;
	}
}


static HERROR local_caIcc_Negotiate(SciHandle_t hSci, scAtrInfo_t *pstAtrInfo)
{
#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
	HERROR 			nErr = ERR_OK;
#endif
	TUnsignedInt8 	ucWI;
	TUnsignedInt8 	ucBWI;
	SvcCas_DevSciScParam_t 	stScParam;
	SvcCas_DevSciProtocol_e	eScProtocol = eCAS_SCI_T0;

	if (pstAtrInfo == NULL)
	{
		HxLOG_Info("target is null ...\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
/* if TD(1) is present , firset offer is T : Therefore we should use FirstT instead T */
	if (pstAtrInfo->ucTFirst == eCAS_SCI_T1)
		eScProtocol = eCAS_SCI_T_1_NA;
	else if (pstAtrInfo->ucTFirst == eCAS_SCI_T14)
		eScProtocol = eCAS_SCI_T14;

	if (pstAtrInfo->ucTFirst== eCAS_SCI_T0)
		ucWI = pstAtrInfo->ucWI;
	else
		ucWI = pstAtrInfo->ucCWI;

	if ( (pstAtrInfo->ucTFirst == eCAS_SCI_T14) && (pstAtrInfo->ucBWI < 5) )
		ucBWI = 5;
	else
		ucBWI = pstAtrInfo->ucBWI;
#else
	if (pstAtrInfo->ucT == eCAS_SCI_T1)
		eScProtocol = eCAS_SCI_T_1_NA;
	else if (pstAtrInfo->ucT == eCAS_SCI_T14)
		eScProtocol = eCAS_SCI_T14;

	if (pstAtrInfo->ucT == eCAS_SCI_T0)
		ucWI = pstAtrInfo->ucWI;
	else
		ucWI = pstAtrInfo->ucCWI;

	if ( (pstAtrInfo->ucT == eCAS_SCI_T14) && (pstAtrInfo->ucBWI < 5) )
		ucBWI = 5;
	else
		ucBWI = pstAtrInfo->ucBWI;
#endif


#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
/*  Set EDC to param !! : */
	if (pstAtrInfo->ucEDC & 0x01) // bit 1 : 1  CRC  ,  0 : LRC
	{
	// there is no param to set edc in the current structure!!
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
	if (pstAtrInfo->bSpecificMode)
	{
		HxLOG_Print("SPECIFIC\n");
		if (pstAtrInfo->bImplicitlyDefined)
		{
			HxLOG_Print("IMPLICIT\n");
			goto Atr_Parameters;
		}
		else
		{
			HxLOG_Print("EXPLICIT\n");
			goto Atr_Parameters;
		}
	}
	else	// negotiable mode
	{
		HxLOG_Print("NEGOTIABLE\n");

		if (pstAtrInfo->bExistTA1 == TRUE) // there should be TA1 for starting PPS
		{
			// check if default Fd, Dd
			if ( (pstAtrInfo->ucT == pstAtrInfo->ucTFirst)
				&& ((pstAtrInfo->ucFI == 1 && pstAtrInfo->ucDI == 1)
				|| (pstAtrInfo->ucFI == 0 && pstAtrInfo->ucDI == 1)) )		// only 1 protocol & Fd Dd : no need to proceed PPS
					goto Atr_Parameters;

			HxLOG_Print("START NEGOTIABLE PPS !!!!!!\n");
			nErr = local_caIcc_NegotiateByValueOrder(hSci, pstAtrInfo, ucWI, ucBWI, &s_stDefaultAtrParam);
			if (nErr != ERR_OK)
				goto Default_Parameters;
			else
				return ERR_OK;
		}
		else
		{
			goto Atr_Parameters;
		}
	}

Default_Parameters:
	HxLOG_Print("SET Default FI DI\n");
//	pstDefaultParam->ulBaudrate = 9600;
	nErr = svc_cas_DevScSetParams(hSci, &s_stDefaultAtrParam);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("err<%x>\n", nErr);
	}
	return ERR_OK;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
Atr_Parameters:
#endif

	/* valid check F, D here because we need to compare with original Fn, Dn value above */
	if (pstAtrInfo->ucFI == 0 && pstAtrInfo->ucDI == 0)
	{
		HxLOG_Print("(stScParam.ucFI == 0 && stScParam.ucDI == 0 )\n\n\n");
		pstAtrInfo->ucFI = 1;
		pstAtrInfo->ucDI = 1;
	}

	if (pstAtrInfo->ucDI < 1 || pstAtrInfo->ucDI > 9)
	{
		HxLOG_Print("(stScParam.ucDI < 1 ||stScParam.ucDI > 9) stScParam.ucDI = %d\n\n\n", pstAtrInfo->ucDI);
		pstAtrInfo->ucDI = 1;
	}

	HxSTD_memset(&stScParam, 0x00, sizeof(SvcCas_DevSciScParam_t));

	stScParam.eProtocol  	= eScProtocol;
	stScParam.ulClock 		= pstAtrInfo->ulMinClock;
	stScParam.ulBaudrate 	= pstAtrInfo->ulBaudrate;
	stScParam.ucFI 		= pstAtrInfo->ucFI;
	stScParam.ucDI 		= pstAtrInfo->ucDI;
	stScParam.ucN 		= pstAtrInfo->ucN;
	stScParam.ucCWI 		= ucWI;
	stScParam.ucBWI 		= ucBWI;

	if (svc_cas_DevScSetParams(hSci, &stScParam) != ERR_OK)
	{
		HxLOG_Info("error in svc_cas_DevScSetParams\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


static void local_caIcc_ScMgrCallbackFunc_0(SciHandle_t hSci, TUnsignedInt8 ucSlotId, SvcCas_DevSciEvent_e eStatus)
{
	TUnsignedInt32 	ulMsg[4];
	TSignedInt32	nErrCode;

	ulMsg[0] = ICC_STATUS_CHANGE_CMD;
	ulMsg[1] = 0;
	ulMsg[2] = ucSlotId;
	ulMsg[3] = eStatus;

	nErrCode = VK_PostMessage(s_ulIccQueueId, (void *)&ulMsg);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}
}


static void local_caIcc_ScMgrCallbackFunc_1(SciHandle_t hSci, TUnsignedInt8 ucSlotId, SvcCas_DevSciEvent_e eStatus)
{
	TUnsignedInt32 	ulMsg[4];
	TSignedInt32	nErrCode;

	ulMsg[0] = ICC_STATUS_CHANGE_CMD;
	ulMsg[1] = 1;
	ulMsg[2] = ucSlotId;
	ulMsg[3] = eStatus;

	nErrCode = VK_PostMessage(s_ulIccQueueId, (void *)&ulMsg);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}
}


static void local_caIcc_DebugParsedAtr(scAtrInfo_t *pstAtrInfo)
{
	HxLOG_Print("____ SC PARSED ATR INFO ____\n");

	if (pstAtrInfo == NULL)
		return;

	if (pstAtrInfo->bSpecificMode)
		HxLOG_Print("specific mode\n");
	else
		HxLOG_Print("negotiable mode\n");

	if (pstAtrInfo->bSpecificMode)
	{
		if(pstAtrInfo->bPpsUnable)
			HxLOG_Print("PPS Request unable\n");
		else
			HxLOG_Print("PPS Request available\n");
	}

	if (pstAtrInfo->bSpecificMode)
	{

		if (pstAtrInfo->bSpecificMode && pstAtrInfo->bImplicitlyDefined)
			HxLOG_Print("Parameter defined implicitly\n");
		else
			HxLOG_Print("Parameter defined explicitly. Use FI, DI\n");
	}

	HxLOG_Print("Historical bytes number (%d)\n", pstAtrInfo->ucK);

	HxLOG_Print("T first (%d), T (%d)\n", pstAtrInfo->ucTFirst, pstAtrInfo->ucT);

	HxLOG_Print("FI (%d), DI (%d), N(%d), WI (%d)\n", pstAtrInfo->ucFI, pstAtrInfo->ucDI, pstAtrInfo->ucN, pstAtrInfo->ucWI);

	if (pstAtrInfo->ucT == 1)
		HxLOG_Print("IFSC (%d), CWI (%d), BWI(%d)\n", pstAtrInfo->ucIFSC, pstAtrInfo->ucCWI, pstAtrInfo->ucBWI);

	HxLOG_Print("II (%d), PI1 (%d), PI2 (%d)\n", pstAtrInfo->ucII, pstAtrInfo->ucPI1, pstAtrInfo->ucPI2);

	HxLOG_Print("H:");

	HxLOG_Print("\n________________________________\n");
}



/**************************************************************************************************/
#define ________CA_ICC_Public_Functions_Body________________________________________________
/**************************************************************************************************/
TIccStatus iccRegister(TIccEventNotification xIccEventNotification, TIccRegistrationId *pxRegistrationId)
{
	TSignedInt32	nErrCode;
	TUnsignedInt32 	ulMsg[4];

	if ( (pxRegistrationId == NULL)
		|| (xIccEventNotification == NULL) )
		return ICC_ERROR ;

	*pxRegistrationId = s_ulRegId;

	ulMsg[0] = (TUnsignedInt32)ICC_REGISTER_CMD;
	ulMsg[1] = (TUnsignedInt32)xIccEventNotification;

#if (CONFIG_FOR_DAL_TEST_DEF == 1)
{
	TUnsignedInt32 i;

	if (s_eIccRegStatus == eIccRegUnregisted)
	{
		HxLOG_Info("s_eIccRegStatus %d, \n", s_eIccRegStatus);

		for (i=0; i<g_ulNumOfSlot; i++)
		{
			HxLOG_Info("g_pstScMgrInstance[i].eCardStatus =%d \n", g_pstScMgrInstance[i].eCardStatus );
			if ((g_pstScMgrInstance[i].eCardStatus == eSc_In) ||(g_pstScMgrInstance[i].eCardStatus == eSc_Mute))
			{
				g_pstScMgrInstance[i].eMode = ICC_ACCESS_EXCLUSIVE;
				if ((nErrCode = iccSmartcardReset(g_pstScMgrInstance[i].ulSessionId, TRUE)) != ICC_NO_ERROR)
				{
					HxLOG_Info("error in iccSmartcardReset...\n");
				#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
				/* If Reset error occurs when Regisering,  you should return mute. If not , DAL Lib crashes.*/
					if (nErrCode == ICC_ERROR_CARD_MUTE || nErrCode == ICC_ERROR)
					{
						HxLOG_Info("Mute in iccSmartcardReset.. Mute .\n");
						// change stats to mute & send mesg with mute state.
						g_pstScMgrInstance[i].eCardStatus = eSc_Mute;
					}
					else if (nErrCode == ICC_ERROR_CARD_REMOVED)
					{
						g_pstScMgrInstance[i].eCardStatus = eSc_Out;
					}
					else
					{
						/* return errocode if other error like ICC_ERROR_SESSION_ID, ICC_ERROR_MODE */
						return nErrCode;
					}
				#else
					return ICC_ERROR;
				#endif
				}
				HxLOG_Info("Success in iccSmartcardReset...\n");
			}
		}
	}
	s_eIccRegStatus = eIccRegRegisted;
}
#endif

	nErrCode = VK_PostMessage(s_ulIccQueueId, (void *)&ulMsg);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
		return ICC_ERROR ;
	}
	return ICC_NO_ERROR;
}


TIccStatus iccCancelRegistration(TIccRegistrationId xRegistrationId)
{
	if (xRegistrationId != s_ulRegId)
	{
		HxLOG_Info("it is not valid register id (0x%x)\n", (unsigned int)xRegistrationId);
		return ICC_ERROR;
	}

#if (CONFIG_FOR_DAL_TEST_DEF == 1)
{
	TUnsignedInt32 i;
	for (i=0; i<g_ulNumOfSlot; i++)
	{
		HxLOG_Info("g_pstScMgrInstance[i].eCardStatus =%d \n",g_pstScMgrInstance[i].eCardStatus);
		if ((g_pstScMgrInstance[i].eCardStatus == eSc_In) ||(g_pstScMgrInstance[i].eCardStatus == eSc_Mute))
		{
			#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
			/* for case 880 885 : need to reset casrdstatus after revious reset error for next test */
			g_pstScMgrInstance[i].eCardStatus = eSc_In;
			HxLOG_Info("g_pstScMgrInstance[i].eCardStatus =%d \n",g_pstScMgrInstance[i].eCardStatus);
			#endif

			if (svc_cas_DevScDeactivate(g_pstScMgrInstance[i].hSmcHandle) != ERR_OK)
			{
				HxLOG_Info("error in svc_cas_DevScDeactivate...\n");
				return ICC_ERROR;
			}
			HxLOG_Info("Success in svc_cas_DevScDeactivate...\n");
		}
	}
	s_eIccRegStatus = eIccRegUnregisted;
}
#endif

	g_pfnNaIccNotiCallback = NULL;
	return ICC_NO_ERROR;
}


TIccStatus iccT1RawExchange(TIccSessionId xSessionId, TSize xSendLen, const TUnsignedInt8 *pxSendBlock, TSize xReplyMaxLen, TSize *pxReplyLen, TUnsignedInt8 *pxReplyBlock)
{
	SciHandle_t		sHandle;
	TSignedInt32		nErrCode;
	TUnsignedInt8 	ucIndex;
	TUnsignedInt32	ulReadLen = 0;
	SvcCas_DevSciScStatus_e 	eStatus;

	if (xSessionId >= g_ulNumOfSlot)
	{
		HxLOG_Info("Invalid Session ID\n");
		return ICC_ERROR_SESSION_ID;
	}

	if (xSendLen == 0)
	{
		HxLOG_Info("xSendLen is 0\n");
		return ICC_ERROR;
	}

	if (xReplyMaxLen == 0)
	{
		HxLOG_Info("xReplyMaxLen is 0\n");
		return ICC_ERROR;
	}

	if (pxSendBlock == NULL)
	{
		HxLOG_Info("pxSendBlock is NULL\n");
		return ICC_ERROR;
	}

	if (pxReplyLen == NULL)
	{
		HxLOG_Info("pxReplyLen is NULL\n");
		return ICC_ERROR;
	}

	if (pxReplyBlock == NULL)
	{
		HxLOG_Info("pxReplyBlock is NULL\n");
		return ICC_ERROR;
	}

	nErrCode = local_caIcc_GetSmcHandle(xSessionId, (SciHandle_t*)&sHandle, &ucIndex);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in local_caIcc_GetSmcHandle (%x)\n", (unsigned int)nErrCode);
		return ICC_ERROR_SESSION_ID;
	}

	if (g_pstScMgrInstance[ucIndex].eCardStatus == eSc_Out)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (0)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (svc_cas_DevScGetCardStatus(g_pstScMgrInstance[ucIndex].hSmcHandle, &eStatus) != ERR_OK)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (1)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (eStatus == eCAS_SCI_SC_NOT_PRESENT)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (2)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (g_pstScMgrInstance[ucIndex].eCardStatus == eSc_Invalid)
	{
		HxLOG_Info("ICC_ERROR\n");
		return ICC_ERROR ;
	}

#if (CONFIG_FOR_DAL_TEST_DEF == 1)
/* DAL Icc Test for 1450, 1460 TA1 =96   :  CardSim does not respond when the such fast Baurate and short interval between T1 packets */
	VK_TASK_Sleep(5);
#endif

	WAIT_FOR_SEM(s_ulIccSemId)
	nErrCode = svc_cas_DevScTransfer(sHandle, (TUnsignedInt8*)pxSendBlock, (TUnsignedInt32)xSendLen, (TUnsignedInt8*)pxReplyBlock, (unsigned int*)&ulReadLen, xReplyMaxLen);
	if ( (ulReadLen > xReplyMaxLen) && (ulReadLen != 0) )
		*pxReplyLen = (TSize)xReplyMaxLen;
	else
		*pxReplyLen = (TSize)ulReadLen;
	RELEASE_SEM(s_ulIccSemId)

	HxLOG_Print("Data transfer Send(%d), Recv(%d, %d)\n", xSendLen, (int)ulReadLen, xReplyMaxLen);
	if ( (ulReadLen < xReplyMaxLen) && (ulReadLen != 0) )
	{
	#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
		/* This is size error due to CWT error during T1 trans : set mute status to block next t1 transmision */
		g_pstScMgrInstance[ucIndex].eCardStatus = eSc_Mute;
	#endif
		HxLOG_Info("ICC_TIMEOUT (ret=0x%08x)\n", (unsigned int)nErrCode);
		return ICC_ERROR_TIMEOUT;
	}
	else if (ulReadLen == 0)
	{
		HxLOG_Info("ICC_MUTE (ret=0x%08x) (xSendLend:%d), (xReplyMaxLen:%d)\n", (unsigned int)nErrCode, xSendLen, xReplyMaxLen);
	#if (0)//defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
		/* CWT error during T1 trans : stop  sending & reset */
			#if (1) /* : for dal test case 2000,  not deactivate just set status mute to block next t1 transmision */
				if ((g_pstScMgrInstance[ucIndex].eCardStatus == eSc_In) || (g_pstScMgrInstance[ucIndex].eCardStatus == eSc_Mute))
				{
					g_pstScMgrInstance[ucIndex].eCardStatus = eSc_Mute;
				}
				HxLOG_Info("g_pstScMgrInstance[i].eCardStatus =%d \n",g_pstScMgrInstance[i].eCardStatus);
				return ICC_ERROR_TIMEOUT;		// chagen to err timeout
			#else
				HxLOG_Info("g_pstScMgrInstance[ucIndex].eCardStatus =%d \n",g_pstScMgrInstance[ucIndex].eCardStatus);
				if ((g_pstScMgrInstance[ucIndex].eCardStatus == eSc_In) ||(g_pstScMgrInstance[ucIndex].eCardStatus == eSc_Mute))
				{
					if (svc_cas_DevScDeactivate(g_pstScMgrInstance[ucIndex].hSmcHandle) != ERR_OK)
					{
						HxLOG_Info("=== error in svc_cas_DevScDeactivate...\n");
						return ICC_ERROR;
					}
					HxLOG_Info("=== Success in svc_cas_DevScDeactivate...\n");
					g_pstScMgrInstance[ucIndex].eCardStatus = eSc_Mute;
				}
				return ICC_ERROR_TIMEOUT;		// chagen to err timeout
			#endif
	#else
		return ICC_ERROR_CARD_MUTE ;
	#endif
	}
	else if (nErrCode != ERR_OK)
	{
		HxLOG_Info("ICC_ERROR (ret=0x%08x)\n", (unsigned int)nErrCode);
		return ICC_ERROR;
	}
	return ICC_NO_ERROR;
}


#if 1
TIccStatus iccT0Receive( TIccSessionId xSessionId, const TIccT0Header pxHeader, TSize xDataExpectedLen, TSize* pxDataLen, TUnsignedInt8* pxData, TIccT0StatusWords pxStatusWords )
{
	return ICC_NO_ERROR;
}
#else
TIccStatus iccT0Receive(TIccSessionId xSessionId, TSize xReceiveMaxLen, TSize *pxReceiveLen, TUnsignedInt8 *pxReceiveBytes)
{
	SciHandle_t		sHandle;
	TSignedInt32	nErrCode;
	TUnsignedInt8	ucIndex;
	SvcCas_DevSciScStatus_e	eStatus;

	nErrCode = local_caIcc_GetSmcHandle(xSessionId, (SciHandle_t*)&sHandle, &ucIndex);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in local_caIcc_GetSmcHandle (%x)\n", (unsigned int)nErrCode);
		return ICC_ERROR_SESSION_ID;
	}

	if (g_pstScMgrInstance[ucIndex].eCardStatus == eSc_Out)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (0)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (svc_cas_DevScGetCardStatus(g_pstScMgrInstance[ucIndex].hSmcHandle, &eStatus) != ERR_OK)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (1)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (eStatus == eCAS_SCI_SC_NOT_PRESENT)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (2)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	nErrCode = svc_cas_DevScRead(sHandle, NULL, 0, pxReceiveBytes, pxReceiveLen);
	if (nErrCode)
	{
		HxLOG_Info("ICC_ERROR\n");
		return ICC_ERROR;
	}
	return ICC_NO_ERROR;
}
#endif


#if 1
TIccStatus iccT0Send( TIccSessionId xSessionId, const TIccT0Header pxHeader, TSize xDataLen, const TUnsignedInt8* pxData, TIccT0StatusWords pxStatusWords )
{
	return ICC_NO_ERROR;
}
#else
TIccStatus iccT0Send(TIccSessionId xSessionId, TSize xSendLen, TUnsignedInt8 *pxSendBytes)
{
	SciHandle_t		sHandle;
	TSignedInt32	nErrCode;
	TUnsignedInt8	ucIndex;
	SvcCas_DevSciScStatus_e	eStatus;

	nErrCode = local_caIcc_GetSmcHandle(xSessionId, (SciHandle_t*)&sHandle, &ucIndex);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in local_caIcc_GetSmcHandle (%x)\n", (unsigned int)nErrCode);
		return ICC_ERROR_SESSION_ID;
	}

	if (g_pstScMgrInstance[ucIndex].eCardStatus == eSc_Out)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (0)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (svc_cas_DevScGetCardStatus(g_pstScMgrInstance[ucIndex].hSmcHandle, &eStatus) != ERR_OK)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (1)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (eStatus == eCAS_SCI_SC_NOT_PRESENT)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (2)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	nErrCode = svc_cas_DevScWrite(sHandle, pxSendBytes, xSendLen, NULL, NULL);
	if (nErrCode)
	{
		HxLOG_Info("ICC_ERROR\n");
		return ICC_ERROR;
	}
	return ICC_NO_ERROR;
}
#endif


TIccStatus iccModeChange(TIccSessionId xSessionId, TIccAccessMode xMode)
{
	SciHandle_t		sHandle;
	TSignedInt32	nErrCode;
	TUnsignedInt8	ucIndex;
	SvcCas_DevSciScStatus_e	eStatus;

	nErrCode = local_caIcc_GetSmcHandle(xSessionId, (SciHandle_t*)&sHandle, &ucIndex);
	if (nErrCode != 0)
	{
		HxLOG_Info("error in local_caIcc_GetSmcHandle (%x)\n", (unsigned int)nErrCode);
		return ICC_ERROR_SESSION_ID;
	}

	if (g_pstScMgrInstance[ucIndex].eCardStatus == eSc_Out)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (0)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (svc_cas_DevScGetCardStatus(g_pstScMgrInstance[ucIndex].hSmcHandle, &eStatus) != ERR_OK)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (1)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (eStatus == eCAS_SCI_SC_NOT_PRESENT)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (2)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (xMode == ICC_ACCESS_SHARED)
	{
		HxLOG_Info("Invalid MODE\n");
		return ICC_ERROR_MODE;
	}

	g_pstScMgrInstance[ucIndex].eMode = xMode;
	return ICC_NO_ERROR;
}


TIccStatus iccSmartcardReset(TIccSessionId xSessionId, TBoolean xColdReset)
{
	SciHandle_t		sHandle;
	TUnsignedInt32	ulErrCode;
	TUnsignedInt8		ucIndex;
	SvcCas_DevSciScStatus_e 	eStatus;
	scAtrInfo_t 		stAtrInfo;
	SvcCas_DevSciScParam_t 	stScParam;
	SvcCas_DevScmResetType_e	eResetType = eCAS_SCM_RESET_TYPE_WARM;
	TUnsignedInt32	ulAtrLen = 0;

	ulErrCode = local_caIcc_GetSmcHandle(xSessionId, (SciHandle_t*)&sHandle, &ucIndex);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Info("error in local_caIcc_GetSmcHandle (%x)\n", (unsigned int)ulErrCode);
		return ICC_ERROR_SESSION_ID;
	}

	if (g_pstScMgrInstance[ucIndex].eCardStatus == eSc_Out)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (0)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (svc_cas_DevScGetCardStatus(g_pstScMgrInstance[ucIndex].hSmcHandle, &eStatus) != ERR_OK)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (1)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (eStatus == eCAS_SCI_SC_NOT_PRESENT)
	{
		HxLOG_Info("ICC_ERROR_CARD_REMOVED (2)\n");
		return ICC_ERROR_CARD_REMOVED;
	}

	if (g_pstScMgrInstance[ucIndex].eMode == ICC_ACCESS_NONE)
	{
		HxLOG_Error("Invalid MODE\n");
		return ICC_ERROR_MODE;
	}

	if (xColdReset == TRUE)
	{
		eResetType = eCAS_SCM_RESET_TYPE_COLD;
	}

	HxSTD_memset((TChar*)g_pstScMgrInstance[ucIndex].ucAtr, 0, sizeof(g_pstScMgrInstance[ucIndex].ucAtr));
	ulErrCode = svc_cas_DevScReset(g_pstScMgrInstance[ucIndex].hSmcHandle,
								(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
								&ulAtrLen,
								eResetType,
								&s_stDefaultAtrParam);

#if (CONFIG_RETRY_RESET == 1)
	if ( (ulErrCode != ERR_OK) || (ulAtrLen < 4) )
	{
		HxLOG_Info("[iccSmartcardReset] svc_cas_DevScReset retry\n");
		ulErrCode = svc_cas_DevScReset(g_pstScMgrInstance[ucIndex].hSmcHandle,
									(TUnsignedInt8*)g_pstScMgrInstance[ucIndex].ucAtr,
									&ulAtrLen,
									eResetType,
									&s_stDefaultAtrParam);
	}
#endif
	g_pstScMgrInstance[ucIndex].ucAtrLen = (TUnsignedInt8)ulAtrLen;
	if (ulErrCode != ERR_OK)
	{
	#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
		/* parity error 시 deactivation timeout (410, 430)  해결 위해  여기서  deactivation 수행 */
		HxLOG_Info("error in svc_cas_DevScReset... Deactivate right now!!!! ~~~~~~~~~~~~~\n");
		if (svc_cas_DevScDeactivate(g_pstScMgrInstance[ucIndex].hSmcHandle) != ERR_OK)
		{
			HxLOG_Info("error in svc_cas_DevScDeactivate...\n");
			return ICC_ERROR_CARD_MUTE;
		}
	#endif
		HxLOG_Info("ICC_ERROR_MUTE\n");
		return ICC_ERROR_CARD_MUTE;
	}

	HxSTD_memset(&stAtrInfo, 0x00, sizeof(scAtrInfo_t));

#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
	/* Add AtrLength parameter for Atr validation check */
	ulErrCode = local_caIcc_ParseAtr(g_pstScMgrInstance[ucIndex].ucAtr, g_pstScMgrInstance[ucIndex].ucAtrLen, &stAtrInfo);
#else
	ulErrCode = local_caIcc_ParseAtr(g_pstScMgrInstance[ucIndex].ucAtr, &stAtrInfo);
#endif
 	if (ulErrCode != ERR_OK)
	{
	#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
		/* parity error 시 deactivation timeout (410, 430)  해결 위해  여기서  deactivation 수행 */
		HxLOG_Info("error in local_caIcc_ParseAtr... Deactivate right now!!!! ~~~~~~~~~~~~~\n");
		if (svc_cas_DevScDeactivate(g_pstScMgrInstance[ucIndex].hSmcHandle) != ERR_OK)
		{
			HxLOG_Info("error in svc_cas_DevScDeactivate...\n");
			return ICC_ERROR_CARD_MUTE;
		}
	#endif
		HxLOG_Info("error in local_caIcc_ParseAtr\n");
		return ICC_ERROR;
	}

	ulErrCode = local_caIcc_Negotiate(g_pstScMgrInstance[ucIndex].hSmcHandle, &stAtrInfo);
 	if (ulErrCode != ERR_OK)
	{
	#if defined(CONFIG_MW_CAS_NAGRA_ISO7816_ICC)
		/* parity error 시 deactivation timeout (410, 430)  해결 위해  여기서  deactivation 수행 */
		HxLOG_Info("error in local_caIcc_Negotiate... Deactivate right now!!!! ~~~~~~~~~~~~~\n");
		if (svc_cas_DevScDeactivate(g_pstScMgrInstance[ucIndex].hSmcHandle) != ERR_OK)
		{
			HxLOG_Info("error in svc_cas_DevScDeactivate...\n");
			return ICC_ERROR_CARD_MUTE;
		}
	#endif
		HxLOG_Info("error in local_caIcc_Negotiate\n");
		return ICC_ERROR;
	}

	ulErrCode = svc_cas_DevScGetParams(g_pstScMgrInstance[ucIndex].hSmcHandle, &stScParam);
 	if (ulErrCode != ERR_OK)
	{
		HxLOG_Info("error in svc_cas_DevScGetParams\n");
		return ICC_ERROR;
	}

	g_pstScMgrInstance[ucIndex].eInstState = eSc_ArtReceived;
	g_pstScMgrInstance[ucIndex].ulFreq = (stScParam.ulClock * 1000);
	g_pstScMgrInstance[ucIndex].eProtocol = stScParam.eProtocol;
	return ICC_NO_ERROR;
}

/**************************************************************************************************/
#define ________CA_ICC_Externel_Functions_Body________________________________________________
/**************************************************************************************************/
HERROR CA_NA_IccInitialization(void)
{
	TUnsignedInt32	i;
	TSignedInt32	nErrCode;

	// Creat MSG Queue
	nErrCode = VK_CreateMessageQueue(256, (unsigned long*)&s_ulIccQueueId);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_CreateMessageQueue (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	// Creat SEM
	nErrCode = VK_SEM_Create((unsigned long*)&s_ulIccSemId, (TChar *)"ca_icc_sem", VK_SUSPENDTYPE_PRIORITY);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_SEM_Create (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	// Creat Task
	nErrCode = VK_TASK_Create(local_caIcc_Task,
								NA_ICC_TASK_PRIORITY,
								NA_ICC_TASK_STACK_SIZE,
								(TUnsignedInt8*)NA_ICC_TASK_NAME,
								(void *)NULL,
								(unsigned long*)&s_ulIccTaskId,
								0);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_TASK_Create (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	// start icc task
	nErrCode = VK_TASK_Start(s_ulIccTaskId);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_TASK_Start (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	// Get number of sc slot(s)
	g_ulNumOfSlot = svc_cas_DevScGetNumberOfModule();
	if (g_ulNumOfSlot <= 0)
	{
		HxLOG_Info("There is no sc slot....\n");
		return ERR_FAIL;
	}

	// Initialize Internal(CAS Module에서 사용하는) Data
	if (local_caIcc_InitSmcMgrInstance(g_ulNumOfSlot) != ERR_OK)
	{
		HxLOG_Info("error in local_caIcc_InitSmcMgrInstance....\n");
		return ERR_FAIL;
	}

	for (i=0; i<g_ulNumOfSlot; i++)
	{
		nErrCode = svc_cas_DevScRegisterCbFunc(i, eDxCAS_GROUPID_NA, g_pstScMgrInstance[i].pfnSmcCallback, &(g_pstScMgrInstance[i].hSmcHandle));
		if (nErrCode != ERR_OK)
		{
			HxLOG_Info("error in svc_cas_DevScRegisterCbFunc (0x%x)\n", (unsigned int)nErrCode);
			return ERR_FAIL;
		}
		HxLOG_Print("hSci[%d], handle <%x>\n", (int)i, (unsigned int)g_pstScMgrInstance[i].hSmcHandle);
	}

	return ERR_OK;
}


