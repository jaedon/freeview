/**************************************************************************************************/
#define ________CA_CAK_MGR_Private_Include_____________________________________________________
/**************************************************************************************************/
#include <db_param.h>

#include "na_glob.h"
#include "util.h"
//#include "pal_sys.h"
#include <_svc_cas_mgr_common.h>

#include "namgr_main.h"

/**************************************************************************************************/
#define ________CA_CAK_MGR_Golbal_Value________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_MGR_Private_Define_______________________________________________________
/**************************************************************************************************/





// for setting casysid from inserted sc
#if defined(CONFIG_OP_SES)
#define CONFIG_SET_SC_CASYSID				1
#else
#define CONFIG_SET_SC_CASYSID				0
#endif

// support sw pairing-key


// support to get SC zipcode
#if defined(CONFIG_OP_SES)
#define CONFIG_USE_SC_ZIPCODE				1
#else
#define CONFIG_USE_SC_ZIPCODE				0
#endif

#define MAX_ZIPCODE_SIZE					10
#define ALARM_BUFFER_SIZE	   				20
#define IRDCMD_BUFFER_SIZE					32
#define IRDCMD_MAX_CMD_LEN				128 // in case of Merlin

#define QUEUE_SIZE_1						sizeof(NaCasMsg_t)
#define QUEUE_SIZE_2						(sizeof(TUnsignedInt32) * 4)
#define QUEUE_TIME_OUT						20000

#define VK_CreateMessageQueue(a, b, c, d)		VK_MSG_Create(a, d, c, b, VK_SUSPENDTYPE_PRIORITY)
#define VK_PostMessage(a, b, c) 				VK_MSG_Send(a, b, c)
#define VK_GetMessage(a ,b, c)				VK_MSG_Receive(a, b, c)
#define VK_WaitGetMessage(a, b, c, d)			VK_MSG_ReceiveTimeout(a, b, d, c)

#define UI_REQ_LOC_INFO 					0
#define UI_REQ_SYSTEM_INFO					1

#define NAME_CHECK_PIN_CODE				"caCak1"
#define NAME_CHANGE_PIN_CODE				"caCak2"
#define NAME_VALIDITY_HD_QUEUE			"caCak3"

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
// DLK_CAK_DevGuide_1.1.3 (작업일자 기준) - PARENTAL CONTROL chapter 참조
#define ALWAYS_CA_PARENTAL_CONTROL_OFF		15
#define ALWAYS_CA_PARENTAL_CONTROL_ON		0
#endif


/**************************************************************************************************/
#define ________CA_CAK_MGR_Private_Type_Define_________________________________________________
/**************************************************************************************************/
typedef struct tagScExpirationInfo
{
	TBoolean			bEndOfTime;
	TBoolean			bGotScExpireDate;
	TCalendarTime	stScExpireDate;
} ScExpirationInfo_t;


/**************************************************************************************************/
#define ________CA_CAK_MGR_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static TUnsignedInt32 			s_ulCakTaskId;
static TUnsignedInt32 			s_ulCakQueueId;

static TIrdInfo					s_stSysIrdInfo;
static NaChipsetInfo_t			s_stChipsetInfo;

static TUnsignedInt8			s_xMaxNumberOfIcc = NO_SMC_SLOT;

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
static TUnsignedInt8			s_ucCscMaxIndex;
#endif

static TSmartcardInfoItem		s_stSysScInfo[NO_SMC_SLOT];
static TLocationInfoItem		s_stLocInfo[NO_SMC_SLOT];

static NaSmartcardStatus_t		s_stSmarcardAlarmItem[ALARM_BUFFER_SIZE][NO_SMC_SLOT];
static NaAccessAlarm_t			s_stStreamAlarmItem[ALARM_BUFFER_SIZE][MAX_TOTAL_NO_STREAM];
static TUnsignedInt32			s_ulAlarmInBuff = 0;
static TUnsignedInt32			s_ulAlarmOutBuff = 0;

static TUnsignedInt8			s_ucIrdCmdBuff[IRDCMD_BUFFER_SIZE][IRDCMD_MAX_CMD_LEN];
static TUnsignedInt32			s_ulIrdcmdInBuff = 0;
static TUnsignedInt32			s_ulIrdcmdOutBuff = 0;

static TUnsignedInt16			s_usSysScInfoNb = 0;
static TSystemInfoStatus 		s_eSysInfoStatus;
static TUnsignedInt8			s_ucNotiScInfo = 0;

static TUnsignedInt16			s_usLocaInfoNb = 0;
static TLocationInfoStatus		s_eLocInfoStatus;

static TAccessAlarmCallback		s_pfnAlarmCallBack = NULL;
static TSystemInfoCallback		s_pfnSysInfoCallBack = NULL;
static TLocationInfoCallback		s_pfnLocInfoCallBack = NULL;
static TSmartcardAlarmCallback	s_pfnScAlarmCallback = NULL;
static TCaPopupCallback			s_pfnCaPopupCallback = NULL;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
static TCopInfoCallback 				s_pfnCopCallback = NULL;
#endif

static TSmartcardState			s_eScLastAlarm[NO_SMC_SLOT];
static ScExpirationInfo_t			s_stScExpireInfo[NO_SMC_SLOT];
static TUnsignedInt32			s_ulZipCodeInfo[NO_SMC_SLOT];
static TUnsignedInt32			s_ulValidityHdQueueId;

// Expiration Date Time을 가져오기 위한 요소...
static TCaDuration				s_ulProductDuration;

static TCaListener 				*s_pSystemInfoListener;
static TCaListener 				*s_pSmatcardListener;
static TCaListener 				*s_pProgramsListener;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static TCaListener 				*s_pPopupListener;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
static TCaListener 				*s_pOperatorsListener;
#endif
static TCaListener 				*s_pIrdCmdListener;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static TCaListener				*s_pAccessRightListener;
static TCaAccess				s_ulAccessStatus;
#endif

static TBoolean					s_bOccuredTriggerOfSysInfo = FALSE;
static TBoolean					s_bOccuredTriggerOfScInfo = FALSE;

//static TUnsignedInt8 			s_ucPairingData[MAX_PD_SIZE];

/**************************************************************************************************/
#define ________CA_CAK_MGR_Private_Static_Prototype______________________________________________
/**************************************************************************************************/
static void 		_naMgr_cakMgr_Task(void* p);
static HERROR	 	_naMgr_cakMgr_Startup(void);
static HERROR 		_naMgr_cakMgr_NagraStartup(void);
static TCaStatus 	_naMgr_cakMgr_PairingDataImportation(const TUnsignedInt8 **ppxNasps_ucPairingData);
static TCaStatus 	_naMgr_cakMgr_UtcTimeImportation(TCalendarTime *pxUtcTime);
//static void 		_naMgr_cakMgr_GetPairingData(void);
static void 		_naMgr_cakMgr_InitGlobalInfo(void);
static void 		_naMgr_cakMgr_SendCakMsg(TUnsignedInt8 msg, TUnsignedInt16 param1, TUnsignedInt8 *param2, TUnsignedInt32 param3);

static void 		_naMgr_cakMgr_InitSmartCardStatus(void);
static void 		_naMgr_cakMgr_InitSystemIRDInfo(void);
static void 		_naMgr_cakMgr_InitSystemSmartCardInfo(void);
static void 		_naMgr_cakMgr_FreeSystemIRDInfo(void);
static void 		_naMgr_cakMgr_FreeSystemSmartCardInfo(void);

static void 		_naMgr_cakMgr_CaSystemInfoListener(const TCaListener *pxListener, TCaExportation *pxExportation);
static void 		_naMgr_cakMgr_RequestSyncSystemInfo(void);

static void 		_naMgr_cakMgr_CaSmatcardListener(const TCaListener *pxListener, TCaExportation *pxExportation);
static void 		_naMgr_cakMgr_GetSystemSmartCardInfo(TUnsignedInt32 ulSlot, TIccSessionId xSessionId, TCaSmartcard* pxSmartcard);
static void 		_naMgr_cakMgr_RequestSyncSmartCardStatus(void);

static void 		_naMgr_cakMgr_GetSmartCardStatus(void);
static void 		_naMgr_cakMgr_CaProgramsListener(const TCaListener *pxListener, TCaExportation *pxExportation);
static TCaStatus	local_caCak_InitValidityHdProduct(void);
#if (CONFIG_DSC_METADATA == 0)
static HERROR 	_naMgr_cakMgr_GetMetaDescriptor(TUnsignedInt8 *pxInputData, TUnsignedInt32 xInputDataLength,
													TUnsignedInt8 xTag, TUnsignedInt8 **ppxOutputData, TUnsignedInt8 *pxOutputDataLength);
#endif
static void		_naMgr_cakMgr_RequestEnablingConsumption(const TCaProgram *pxProgram, TBoolean bOneShot);

static void 		_naMgr_cakMgr_RequestSyncProgramInfo(void);

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static void 		_naMgr_cakMgr_CaPopupListener(const TCaListener *pxListener, TCaExportation *pxExportation);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
static void			_naMgr_cakMgr_CaOperatorsListener(const TCaListener *pxListener, TCaExportation *pxExportation);
static void			_naMgr_cakMgr_RequestSyncOperatorsInfo(void);
#endif
static void 		_naMgr_cakMgr_RequestSyncPopupInfo(void);
static void 		_naMgr_cakMgr_CaIrdCmdListener(const TCaListener *pxListener, TCaExportation *pxExportation);
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static void		_naMgr_cakMgr_CaAccessRightListener(const TCaListener *pxListener, TCaExportation *pxExportation);
#endif
static void 		_naMgr_cakMgr_ExecIrdCmdExportation(TSize xIrdCmdLength, TUnsignedInt8 *pxIrdCmd);

static HERROR 	_naMgr_cakMgr_CheckPinCode(TCaPinCodeId xPinCodeId, TPinCode xPinCode, TPinCodeStatus *xStatus);
static HERROR		_naMgr_cakMgr_ChangePinCode(TCaPinCodeId xPinCodeId, TPinCode xOldPinCode, TPinCode xNewPinCode, TPinCodeStatus *xStatus);

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
static void		_naMgr_cakMgr_EnablingConsumptionForced(TBoolean bOneShot);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
static void		_naMgr_cakMgr_RequestSyncSetParentalControl(void);
#endif

extern HERROR CA_NA_PsiGetCasInstanceID(Handle_t *phCasHandle);

#if defined(CONFIG_DEBUG) || defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
static void		_naMgr_cakMgr_caLogEnable(void);
#endif

/**************************************************************************************************/
#define ________CA_CAK_MGR_Private_Static_Prototype_Body________________________________________
/**************************************************************************************************/
static void _naMgr_cakMgr_Task(void* p)
{
	NaCasMsg_t stInMsg;

	while(1)
	{
		if (VK_GetMessage(s_ulCakQueueId, (void *)&stInMsg, QUEUE_SIZE_1) != ERR_OK)
		{
			continue;
		}

		switch (stInMsg.wMsg)
		{
			case NA_CAK_ID:
				switch (stInMsg.lParam2)
				{
					case CAK_ALARM_ACCESS:
						HxLOG_Print("CAK_ALARM_ACCESS....\n");

						if (s_pfnAlarmCallBack != NULL)
						{
							(*s_pfnAlarmCallBack)((TUnsignedInt8)stInMsg.lParam3,
												s_stSmarcardAlarmItem[s_ulAlarmOutBuff],
												(TUnsignedInt8)stInMsg.info,
												s_stStreamAlarmItem[s_ulAlarmOutBuff]);
						}

						#if (1)
						{
							TUnsignedInt8 i;
							for (i=0; i<NO_SMC_SLOT; i++)
							{
								if (s_eScLastAlarm[i] != s_stSmarcardAlarmItem[s_ulAlarmOutBuff][i].alarm)
								{
									// Smartcard alarm msg를 app에 notify한다.
									if (s_pfnScAlarmCallback != NULL)
										(*s_pfnScAlarmCallback)(i, s_stSmarcardAlarmItem[s_ulAlarmOutBuff][i].alarm);

									s_eScLastAlarm[i] = s_stSmarcardAlarmItem[s_ulAlarmOutBuff][i].alarm;
								}
							}
						}
						#endif

						s_ulAlarmOutBuff++;
						if (s_ulAlarmOutBuff == ALARM_BUFFER_SIZE)
							s_ulAlarmOutBuff = 0;

						if (s_ucNotiScInfo)
						{
							s_ucNotiScInfo = 0;
							_naMgr_cakMgr_SendCakMsg(UI_REQ_SYSTEM_INFO, 0, NULL, 0);
						}
						break;

					case CAK_CA_POPUP_COMMAND:
						if (s_pfnCaPopupCallback != NULL)
								(*s_pfnCaPopupCallback)((NaPopupUI_t *)stInMsg.lParam3);
						break;

					case CAK_IRD_COMMAND:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
						CA_NA_IRDCMD_ProcessIRDCmd((TUnsignedInt8*) s_ucIrdCmdBuff[s_ulIrdcmdOutBuff]);
						s_ulIrdcmdOutBuff = ((s_ulIrdcmdOutBuff + 1) >= IRDCMD_BUFFER_SIZE) ? 0 : s_ulIrdcmdOutBuff + 1;
#else
						HxLOG_Warning("Not Support IRD Command. Need to be checked");
#endif
						break;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) && !defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
					case CAK_ACCESSRIGHT_LISTENER_NOTIFICATION:
						HxLOG_Info("[_naMgr_cakMgr_Task] Received CAK_ACCESSRIGHT_LISTENER_NOTIFICATION \n");
						MW_CA_NA_ReportCurrAlarmData();
						// Action 을 취해야 함...
						break;
#endif
					default:
						break;
				}
				break;

			case NA_FILTER_ID:
				switch (stInMsg.lParam2)
				{
					case CAK_FILTER_SDT_ARRIVAL:
						break;

					case CAK_FILTER_EIT_PF_ARRIVAL:
						break;

					case CAK_FILTER_NIT_ARRIVAL:
						break;

					case CAK_FILTER_TDT_ARRIVAL:
						break;

					default:
						break;
				}
				break;

			case NA_SERVICE_ID:
				switch (stInMsg.lParam2)
				{
					case CAK_SERVICE_CHANGE_TP:
						break;

					case CAK_SERVICE_CHANGE_CHANNEL:
						break;

					default:
						break;
				}
				break;

			case NA_APP_ID:
				{
					switch((TSignedInt32)stInMsg.lParam1)
					{
						case CAK_SYSTEM_INFO_REQ_ID:
							_naMgr_cakMgr_RequestSyncSystemInfo();
							break;

						case CAK_SMARTCARD_INFO_REQ_ID:
							_naMgr_cakMgr_RequestSyncSmartCardStatus();
							break;

						case CAK_PROGRAMS_INFO_REQ_ID:
							_naMgr_cakMgr_RequestSyncProgramInfo();
							break;

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
						case CAK_OPERATORS_REQ_ID:
							_naMgr_cakMgr_RequestSyncOperatorsInfo();
							break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
						case CAK_PARENTAL_CONTROL_REQ_ID:
							_naMgr_cakMgr_RequestSyncSetParentalControl();
							break;
#endif
					}
				}
				break;

			default:
				break;
		}
	}
}


static HERROR _naMgr_cakMgr_Startup(void)
{
	TSignedInt32 nErrCode;

	nErrCode = VK_TASK_Create(_naMgr_cakMgr_Task, NA_CAK_TASK_PRIORITY, NA_CAK_TASK_STACK_SIZE, NA_CAK_TASK_NAME, NULL, (unsigned long*)&s_ulCakTaskId, 0);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("error in creating UIT thread 0x%x\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_TASK_Start(s_ulCakTaskId);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("error in starting UIT thread 0x%x\n",  (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static TCaListener	*_naMgr_cakMgr_AddCaListener(TCaListenerType xType, TCaListenerCallback xCallback, TBoolean bExportMode)
{
	TCaListenerStatus	eListenerStatus;
	TCaListener* 		pxListener;

	eListenerStatus = caListenerCreate(xType, xCallback, &pxListener);
	if(eListenerStatus != CA_LISTENER_NO_ERROR)
	{
		HxLOG_Error("error in TCaListenerType(%d) create (0x%x)\n", xType, eListenerStatus);
		return NULL;
	}

	if(bExportMode)
	{
		eListenerStatus = caListenerSetExportationMode(pxListener);
		if (eListenerStatus != CA_LISTENER_NO_ERROR)
		{
			HxLOG_Error("error in TCaListenerType(%d) mode (0x%x)\n", xType, eListenerStatus);
			goto _ERROR;
		}
	}

	eListenerStatus = caListenerRegister(pxListener);
	if (eListenerStatus != CA_LISTENER_NO_ERROR)
	{
		HxLOG_Error("error in TCaListenerType(%d) register (0x%x)\n", xType, eListenerStatus);
		goto _ERROR;
	}

	return pxListener;

_ERROR:
	caListenerDispose(pxListener);
	return NULL;
}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
static HBOOL _naMgr_IsSmartcardInserted(void)
{
	SvcCas_DevSciScStatus_e	eStatus;
	HINT32 i;

	for (i = 0 ; i < NO_SMC_SLOT ; i++)
	{
		eStatus = eCAS_SCI_SC_NOT_PRESENT;

		if(svc_cas_DevScGetCardStatusBySlotID(i, &eStatus) != ERR_OK)
			continue;

		if(eStatus == eCAS_SCI_SC_PRESENT)
			return TRUE;
	}

	return FALSE;
}
#endif

static HERROR _naMgr_cakMgr_NagraStartup(void)
{
	TCaStatus eStatus;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	TCaListenerStatus eListenerStatus;
#endif

#if 0
	/* 1. Getting pairing data from flash memory */
	_naMgr_cakMgr_GetPairingData();
#endif

	/* 2. CAK Initalizeation  */
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	if(_naMgr_IsSmartcardInserted() == FALSE)
	{
		s_xMaxNumberOfIcc = 0;	// ICME CAK의 경우 s_xMaxNumberOfIcc == 0일 경우 cardless로 동작.
	}
#endif

	eStatus = caInitialization(_naMgr_cakMgr_PairingDataImportation, _naMgr_cakMgr_UtcTimeImportation, s_xMaxNumberOfIcc);
	if (eStatus != CA_NO_ERROR)
	{
		HxLOG_Error("caInitialization return error \n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG) || defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	_naMgr_cakMgr_caLogEnable();
#endif

	/* 3. Linstener register to get some info from CAK  */
	s_pSystemInfoListener	= _naMgr_cakMgr_AddCaListener(CA_LISTENER_TYPE_SYSTEM, _naMgr_cakMgr_CaSystemInfoListener, FALSE);
	s_pSmatcardListener		= _naMgr_cakMgr_AddCaListener(CA_LISTENER_TYPE_SMARTCARDS, _naMgr_cakMgr_CaSmatcardListener, FALSE);
	s_pProgramsListener		= _naMgr_cakMgr_AddCaListener(CA_LISTENER_TYPE_PROGRAMS, _naMgr_cakMgr_CaProgramsListener, FALSE);

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	s_pPopupListener		= _naMgr_cakMgr_AddCaListener(CA_LISTENER_TYPE_POPUP, _naMgr_cakMgr_CaPopupListener, TRUE);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	s_pOperatorsListener	= _naMgr_cakMgr_AddCaListener(CA_LISTENER_TYPE_OPERATORS, _naMgr_cakMgr_CaOperatorsListener, FALSE);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
	// 3_4 - irdcmd information
	s_pIrdCmdListener  = _naMgr_cakMgr_AddCaListener(CA_LISTENER_TYPE_IRD_COMMAND, _naMgr_cakMgr_CaIrdCmdListener, TRUE);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	// 3_5 - Access Right (For EM 36)

	HxLOG_Info("\n\n\n\n================================\n");
	HxLOG_Info("[_naMgr_cakMgr_NagraStartup] _naMgr_cakMgr_CaAccessRightListener Create\n");
	HxLOG_Info("================================\n\n\n\n\n");
	eListenerStatus = caListenerCreate(CA_LISTENER_TYPE_ACCESS_RIGHTS, _naMgr_cakMgr_CaAccessRightListener, &s_pAccessRightListener);
	if(eListenerStatus == CA_LISTENER_NO_ERROR)
	{
		eListenerStatus = caListenerRegister(s_pAccessRightListener);
		HxLOG_Info("[_naMgr_cakMgr_CaAccessRightListener] Regist !!! \n");
		if (eListenerStatus != CA_LISTENER_NO_ERROR)
		{
			caListenerDispose(s_pAccessRightListener);
			HxLOG_Error("error in CA_LISTENER_TYPE_ACCESS_RIGHTS register (0x%x)\n", eListenerStatus);
		}
	}
	else
	{
		HxLOG_Error("error in CA_LISTENER_TYPE_ACCESS_RIGHTS create (0x%x)\n", eListenerStatus);
	}
#endif

	/* 5.  CAK communication Task Creat */
	if (_naMgr_cakMgr_Startup() != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_cakMgr_Startup...\n");
		return ERR_FAIL;
	}

	/* 6. CAK is strated up */
	eStatus = caStartUp();
	if (eStatus != CA_NO_ERROR)
	{
		HxLOG_Error("error in caStartUp...\n");
		return ERR_FAIL;
	}

	/* 7. frist booting시 system 정보와 s/c status를 얻어온다. */
	_naMgr_cakMgr_InitSmartCardStatus();
	CA_NA_CakRequest(CAK_SYSTEM_INFO_REQ_ID);

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	CA_NA_CakRequest(CAK_PARENTAL_CONTROL_REQ_ID);
#endif

	return ERR_OK;
}


static TCaStatus _naMgr_cakMgr_PairingDataImportation(const TUnsignedInt8 **ppxNasps_ucPairingData)
{
	HxLOG_Print("_naMgr_cakMgr_PairingDataImportation()...\n", __FUNCTION__);

	if (ppxNasps_ucPairingData == NULL)
	{
		HxLOG_Error("ppxNasps_ucPairingData is null...\n");
		return CA_ERROR;
	}

	*ppxNasps_ucPairingData = NAMGR_GetPairingData();
	return CA_NO_ERROR;
}


static TCaStatus _naMgr_cakMgr_UtcTimeImportation(TCalendarTime *pxUtcTime)
{
	HxLOG_Print("_naMgr_cakMgr_UtcTimeImportation()...\n", __FUNCTION__);

	if (pxUtcTime == NULL)
	{
		HxLOG_Error("pxUtcTime is null...\n");
		return CA_ERROR;
	}

	if (CA_NA_CakGetGmtTimeAsCalenderTime((TCalendarTime*)pxUtcTime) == ERR_OK)
	{
		return CA_NO_ERROR;
	}

	HxLOG_Error("error in reading UTC\n");
	return CA_ERROR;
}

#if 0
static void _naMgr_cakMgr_GetPairingData(void)
{
#if defined(USE_SW_PAIRING_KEY)
  #include "na_mgr_cak_pairingdata.h"	// model별 CSO, Operator...
	HxSTD_memcpy(s_ucPairingData, s_aucNaPairingData, MAX_PD_SIZE);
#else
	// TODO: copy decrypted PK from RAM
#endif
}
#endif

static void _naMgr_cakMgr_InitGlobalInfo(void)
{
	TUnsignedInt8 i;

	s_ulAlarmInBuff 		= 0;
	s_ulAlarmOutBuff	= 0;

	s_ulIrdcmdInBuff 	= 0;
	s_ulIrdcmdOutBuff 	= 0;

	s_usSysScInfoNb 	= 0;
	s_usLocaInfoNb 		= 0;
	s_ulProductDuration	= 0;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	s_ulAccessStatus = 0;
#endif
	s_eSysInfoStatus 	= CA_SYSTEM_INFO_ERROR;

	for (i=0; i<NO_SMC_SLOT; i++)
	{
		s_eScLastAlarm[i] = CA_ALARM_SMARTCARD_END;
		HxSTD_memset(&s_stScExpireInfo[i], 0x00, sizeof(ScExpirationInfo_t));
		s_ulZipCodeInfo[i] = 0;
	}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
	s_ucCscMaxIndex = 0;
#endif

	_naMgr_cakMgr_InitSystemIRDInfo();
	_naMgr_cakMgr_InitSystemSmartCardInfo();
}


static void _naMgr_cakMgr_SendCakMsg(TUnsignedInt8 ucMsg, TUnsignedInt16 param1, TUnsignedInt8 *param2, TUnsignedInt32 param3)
{
	switch(ucMsg)
	{
		case UI_REQ_LOC_INFO:
			if (s_pfnLocInfoCallBack != NULL) (*s_pfnLocInfoCallBack)(s_eLocInfoStatus, s_usLocaInfoNb, s_stLocInfo);
			HxLOG_Print("Sent UI_REQ_LOC_INFO msg\n");
			break;

		case UI_REQ_SYSTEM_INFO:
			if (s_pfnSysInfoCallBack != NULL) (*s_pfnSysInfoCallBack)(s_eSysInfoStatus, &s_stSysIrdInfo, s_usSysScInfoNb, s_stSysScInfo);
			HxLOG_Print("Sent UI_REQ_SYSTEM_INFO msg\n");
			break;

		default:
			break;
	}
}


static void _naMgr_cakMgr_InitSmartCardStatus(void)
{
	TUnsignedInt8			i;
	NaSmartcardStatus_t 	*pxSmartCardStatus = NULL;

	NaCasMsg_t			stMsg;
	TSignedInt32			nErrCode;

	ScStatusItem_t		stScStatus[NO_SMC_SLOT];

	if ((((s_ulAlarmInBuff + 1) == ALARM_BUFFER_SIZE) ? 0 : (s_ulAlarmInBuff + 1)) == s_ulAlarmOutBuff)
	{
		HxLOG_Error("Alarm buffer overflow\n");
		return;
	}

	for (i=0; i<NO_SMC_SLOT; i++)
	{
		pxSmartCardStatus = &s_stSmarcardAlarmItem[s_ulAlarmInBuff][i];
		pxSmartCardStatus->iccSessionId = i;
		pxSmartCardStatus->changed = 1;
		pxSmartCardStatus->alarm = CA_ALARM_SMARTCARD_REMOVED;
	}

#if (1)
	if (CA_NA_IccGetCardStatus(stScStatus) == ERR_OK)
	{
		for (i=0; i<NO_SMC_SLOT; i++)
		{
			pxSmartCardStatus = &s_stSmarcardAlarmItem[s_ulAlarmInBuff][i];
			if (stScStatus[i].eCardStatus == eSc_Out)
			{
				pxSmartCardStatus->alarm = CA_ALARM_SMARTCARD_REMOVED;
			}
			else if ( (stScStatus[i].eCardStatus == eSc_Mute) || (stScStatus[i].eCardStatus == eSc_Invalid) )
			{
				pxSmartCardStatus->alarm = CA_SMARTCARD_STATE_MUTE;
			}
			else
			{
				pxSmartCardStatus->alarm = CA_SMARTCARD_STATE_OK;
			}
		}
	}
#endif

	stMsg.wMsg		= NA_CAK_ID;
	stMsg.lParam2		= CAK_ALARM_ACCESS;
	stMsg.lParam3		= NO_SMC_SLOT;
	stMsg.lParam1		= (TSignedInt32)s_ulAlarmInBuff;
	stMsg.info		= 0;

	s_ulAlarmInBuff++;
	if (s_ulAlarmInBuff == ALARM_BUFFER_SIZE)
		s_ulAlarmInBuff = 0;

	nErrCode = VK_PostMessage(s_ulCakQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}
}


static void _naMgr_cakMgr_InitSystemIRDInfo(void)
{
	 s_stSysIrdInfo.pProjectName = NULL;
	 s_stSysIrdInfo.pSerialNumber = NULL;
	 s_stSysIrdInfo.pVersion = NULL;
	 s_stSysIrdInfo.pRevision = NULL;

	 s_stChipsetInfo.pcChipsetType = NULL;
	 s_stChipsetInfo.pcChipsetRev = NULL;
}


static void _naMgr_cakMgr_InitSystemSmartCardInfo(void)
{
	 TUnsignedInt8 i;

	 s_usSysScInfoNb = 0;
	 for (i=0; i<NO_SMC_SLOT; i++)
	 {
		 s_stSysScInfo[i].pSerialNumber = NULL;
		 s_stSysScInfo[i].pVersion = NULL;
		 s_stSysScInfo[i].pRevision = NULL;
		 s_stSysScInfo[i].iccSessionId = (TIccSessionId)-1;
		 s_stSysScInfo[i].provider = (TPpid)-1;
	 }
}


static void _naMgr_cakMgr_FreeSystemIRDInfo(void)
{
	if (s_stSysIrdInfo.pProjectName != NULL)
		OxCAS_Free( (void*) s_stSysIrdInfo.pProjectName );
	if (s_stSysIrdInfo.pSerialNumber != NULL)
		OxCAS_Free( (void*) s_stSysIrdInfo.pSerialNumber );
	if (s_stSysIrdInfo.pVersion != NULL)
		OxCAS_Free( (void*) s_stSysIrdInfo.pVersion );
	if (s_stSysIrdInfo.pRevision != NULL)
		OxCAS_Free( (void*) s_stSysIrdInfo.pRevision );

	if ( s_stChipsetInfo.pcChipsetType != NULL )
		OxCAS_Free( (void*) s_stChipsetInfo.pcChipsetType );
	if ( s_stChipsetInfo.pcChipsetRev != NULL )
		OxCAS_Free( (void*) s_stChipsetInfo.pcChipsetRev );
}


static void _naMgr_cakMgr_FreeSystemSmartCardInfo(void)
{
	TUnsignedInt8 i;

	for (i=0; i<NO_SMC_SLOT; i++)
	{
		if (s_stSysScInfo[i].pSerialNumber != NULL)
			OxCAS_Free( (void*) s_stSysScInfo[i].pSerialNumber );
		if (s_stSysScInfo[i].pVersion != NULL)
			OxCAS_Free( (void*) s_stSysScInfo[i].pVersion );
		if (s_stSysScInfo[i].pRevision != NULL)
			OxCAS_Free( (void*) s_stSysScInfo[i].pRevision );
	}
}


static void _naMgr_cakMgr_CaSystemInfoListener(const TCaListener *pxListener, TCaExportation *pxExportation)
{
	if (pxExportation != NULL)
	{
		caExportationDispose(pxExportation);
	}

	if (pxListener != s_pSystemInfoListener)
	{
		HxLOG_Error("wrong listener (0x%x, 0x%x)\n", (unsigned int)pxListener, (unsigned int)s_pSystemInfoListener);
		return;
	}

	s_bOccuredTriggerOfSysInfo = (TBoolean)TRUE;

	CA_NA_CakRequest(CAK_SYSTEM_INFO_REQ_ID);
}


/* Get System information by Sync Call */
static void _naMgr_cakMgr_RequestSyncSystemInfo(void)
{
	TCaRequest 			*pxRequest = NULL;
	TCaRequestStatus 		eStatus;
	TCaObjectStatus 		eObjStatus;
	TCaProcessingStatus	eProcStatus;
	TUnsignedInt32 		ulNumOfSys = 0;
	TCaSystem			**ppSystem;
	TChar				*pxVersion = NULL;
	TChar				*pxProjectInformation = NULL;
#ifdef DISPLAY_SYSINFO_WITHOUT_ENCODING
	TUnsignedInt32		xSerialNumber;
	HUINT32				ulNuid;
#else
	TChar				*pxSerialNumber = NULL;
	TChar				*pxNuid = NULL;
#endif
	TChar       		*pxChipsetType = NULL;
	TChar       		*pxChipsetRev = NULL;
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
	TUnsignedInt8		xCscMaxIndex = 0;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_SOFTWARE_UPGRADE)
	TSystemFlags		xFlags;
	TCaDuration			xDisplayPeriod = 0;
	NaPopupUI_t			*pstPopupUi = NULL;
	HBOOL				bNeedPopupUI = FALSE;
#endif

	TIrdInfo			*pxIrdInfo = NULL;
	NaChipsetInfo_t		*pxChipsetInfo = NULL;

	eStatus = caRequestCreate(CA_REQUEST_TYPE_SYSTEM, &pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetObjects(pxRequest, &ulNumOfSys, (void***)&ppSystem);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (ulNumOfSys <= 0) )
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfSys);
		caRequestDispose(pxRequest);
		return;
	}

	eObjStatus = caSystemGetProjectInformation(ppSystem[0], (const TChar**)&pxProjectInformation);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caSystemGetProjectInformation (0x%x)\n", eObjStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eObjStatus = caSystemGetCakVersion(ppSystem[0], (const TChar**)&pxVersion);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caSystemGetCakVersion (0x%x)\n", eObjStatus);
		caRequestDispose(pxRequest);
		return;
	}

#ifdef DISPLAY_SYSINFO_WITHOUT_ENCODING
	eObjStatus = caSystemGetIrdSerialNumberInt(ppSystem[0], &xSerialNumber);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caSystemGetIrdSerialNumberInt (0x%x)\n", eObjStatus);
		caRequestDispose(pxRequest);
		return;
	}

	if(svc_cas_DevPortSysGetChipId(&ulNuid) != ERR_OK)
	{
		HxLOG_Error ("error in svc_cas_DevPortSysGetChipId\n");
		caRequestDispose(pxRequest);
		return;
	}
#else
	eObjStatus = caSystemGetIrdSerialNumber(ppSystem[0], (const TChar**)&pxSerialNumber);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caSystemGetIrdSerialNumber (0x%x)\n", eObjStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eObjStatus = caSystemGetNuid(ppSystem[0], (const TChar**)&pxNuid);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error ("error in caSystemGetNuid (0x%x)\n", eObjStatus);
		caRequestDispose(pxRequest);
		return;
	}
#endif

	eObjStatus = caSystemGetChipsetType(ppSystem[0], (const TChar**)&pxChipsetType);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error ("error in caSystemGetChipsetType (0x%x)\n", eObjStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eObjStatus = caSystemGetChipsetRevision(ppSystem[0], (const TChar**)&pxChipsetRev);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error ("error in caSystemGetChipsetRevision (0x%x)\n", eObjStatus);
		caRequestDispose(pxRequest);
		return;
	}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
	eObjStatus = caSystemGetCscMaxIndex(ppSystem[0], &xCscMaxIndex);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error ("error in caSystemGetCscMaxIndex (0x%x)\n", eObjStatus);
		/* 	CSC Key가 없는 보드에서 cas information이 안나오는 문제가 발생해 여기서 return시키지 않는다.
			그래서 xmgr_cas_na_util.c에서 xCscMaxIndex == 0인경우는 '-'로 display하도록 한다.
			- Nagra CAK test document -
			the csc max index field is present. Its value  is blank or "-", because there's no CSCData flashed in OTP.
			In case CSC Data are flashed, then the max Index is "7" or "35" for fully supported NOCS 3 STBs.
		*/
//		caRequestDispose(pxRequest);
//		return;
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_SOFTWARE_UPGRADE)
	eObjStatus = caSystemGetFlags(ppSystem[0], &xFlags);
	if (eObjStatus == CA_OBJECT_NO_ERROR)
	{
		if(xFlags & CA_SYSTEM_SOFTWARE_UPGRADE_RECOMMENDED)
		{
			HxLOG_Print("CA_SYSTEM_SOFTWARE_UPGRADE_RECOMMENDED flag is raised\n");

			eObjStatus = caSystemGetUpgradeWarningDelay(ppSystem[0], &xDisplayPeriod);
			if (eObjStatus == CA_OBJECT_NO_ERROR)
				bNeedPopupUI = TRUE;
			else
				HxLOG_Error ("error in caSystemGetUpgradeWarningDelay (0x%x)\n", eObjStatus);
		}
		else if(xFlags & CA_SYSTEM_SOFTWARE_UPGRADE_REQUIRED)
		{
			HxLOG_Print("CA_SYSTEM_SOFTWARE_UPGRADE_REQUIRED flag is raised\n");
			bNeedPopupUI = TRUE;
		}
		else
		{
			;
		}
	}
#endif

	_naMgr_cakMgr_FreeSystemIRDInfo();
	_naMgr_cakMgr_InitSystemIRDInfo();

	s_eSysInfoStatus = CA_SYSTEM_INFO_SUCCESS;

	pxIrdInfo = &s_stSysIrdInfo;
	pxIrdInfo->pRevision = NULL;

	if (pxProjectInformation != NULL)
	{
		HxLOG_Print("caSystemGetProjectInformation (%s)\n", pxProjectInformation);
		pxIrdInfo->pProjectName = OxCAS_Malloc( strlen(pxProjectInformation) + 1 );
		if (pxIrdInfo->pProjectName)
		{
			HxSTD_memset(  (void*)pxIrdInfo->pProjectName, 0x00, strlen(pxProjectInformation) + 1 );
			HxSTD_memcpy( (TUnsignedInt8*) pxIrdInfo->pProjectName, pxProjectInformation, strlen(pxProjectInformation) );
		}
	}

	if (pxVersion)
	{
		HxLOG_Print("caSystemGetCakVersion (%s)\n", pxVersion);
		pxIrdInfo->pVersion = OxCAS_Malloc( strlen(pxVersion) + 1 );
		if (pxIrdInfo->pVersion)
		{
			HxSTD_memset( (void*)pxIrdInfo->pVersion, 0x00, strlen(pxVersion) + 1 );
			HxSTD_memcpy( (TUnsignedInt8*) pxIrdInfo->pVersion, pxVersion, strlen(pxVersion) );
		}
	}

#if defined(DISPLAY_SYSINFO_WITHOUT_ENCODING)
	HxLOG_Print("caSystemGetIrdSerialNumberInt (%x)\n", xSerialNumber);
	pxIrdInfo->pSerialNumber = OxCAS_Malloc( sizeof(TUnsignedInt32) * 2 + 1);
	if (pxIrdInfo->pSerialNumber)
	{
		HxSTD_memset( (void*)pxIrdInfo->pSerialNumber, 0x00, sizeof(TUnsignedInt32) * 2 + 1 );
		HxSTD_sprintf(pxIrdInfo->pSerialNumber, "%X", xSerialNumber);
	}
#else
	if (pxSerialNumber != NULL)
	{
		HxLOG_Print("caSystemGetIrdSerialNumber (%s)\n", pxSerialNumber);
		pxIrdInfo->pSerialNumber = OxCAS_Malloc( strlen(pxSerialNumber) + 1 );
		if (pxIrdInfo->pSerialNumber)
		{
			HxSTD_memset( (void*)pxIrdInfo->pSerialNumber, 0x00, strlen(pxSerialNumber) + 1 );
			HxSTD_memcpy( (TUnsignedInt8*) pxIrdInfo->pSerialNumber, pxSerialNumber, strlen(pxSerialNumber) );
		}
	}
#endif

	pxChipsetInfo = &s_stChipsetInfo;

#ifdef DISPLAY_SYSINFO_WITHOUT_ENCODING
	HxLOG_Print("svc_cas_DevPortSysGetChipId (%x)\n", ulNuid);
	pxChipsetInfo->pcNuid = OxCAS_Malloc( sizeof(HUINT32) * 2 + 1 );
	if (pxChipsetInfo->pcNuid)
	{
		HxSTD_memset(  pxChipsetInfo->pcNuid, 0x00, sizeof(HUINT32) * 2 + 1 );
		HxSTD_sprintf(pxChipsetInfo->pcNuid, "%X", ulNuid);
	}
#else
	if (pxNuid != NULL)
	{
		HxLOG_Print("caSystemGetNuid (%s)\n", pxNuid);
		pxChipsetInfo->pcNuid = OxCAS_Malloc( strlen(pxNuid) + 1 );
		if (pxChipsetInfo->pcNuid)
		{
			HxSTD_memset(  pxChipsetInfo->pcNuid, 0x00, strlen(pxNuid) + 1 );
			HxSTD_memcpy( (TUnsignedInt8*) pxChipsetInfo->pcNuid, pxNuid, strlen(pxNuid) );
		}
	}
#endif

	if (pxChipsetType != NULL)
	{
		HxLOG_Print("caSystemGetChipsetType (%s)\n", pxChipsetType);
		pxChipsetInfo->pcChipsetType = OxCAS_Malloc( strlen(pxChipsetType) + 1 );
		if (pxChipsetInfo->pcChipsetType)
		{
			HxSTD_memset(  pxChipsetInfo->pcChipsetType, 0x00, strlen(pxChipsetType) + 1 );
			HxSTD_memcpy( (TUnsignedInt8*) pxChipsetInfo->pcChipsetType, pxChipsetType, strlen(pxChipsetType) );
		}
	}

	if (pxChipsetRev != NULL)
	{
		HxLOG_Print("caSystemGetChipsetRevision (%s)\n", pxChipsetRev);
		pxChipsetInfo->pcChipsetRev = OxCAS_Malloc( strlen(pxChipsetRev) + 1 );
		if (pxChipsetInfo->pcChipsetRev)
		{
			HxSTD_memset(  pxChipsetInfo->pcChipsetRev, 0x00, strlen(pxChipsetRev) + 1 );
			HxSTD_memcpy( (TUnsignedInt8*) pxChipsetInfo->pcChipsetRev, pxChipsetRev, strlen(pxChipsetRev) );
		}
	}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
	s_ucCscMaxIndex = xCscMaxIndex;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_SOFTWARE_UPGRADE)
	if(bNeedPopupUI == TRUE)
	{
		pstPopupUi = OxCAS_Calloc(sizeof(NaPopupUI_t));
		if(pstPopupUi != NULL)
		{
			pstPopupUi->eType = ePopup_SWupgrade;
			pstPopupUi->ulPeriod = (HUINT32)xDisplayPeriod;

			NAMGR_CAKMGR_SendPopupMessage(pstPopupUi);
		}
	}
#endif

	eStatus = caRequestDispose(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestDispose (0x%x)\n", eStatus);
	}

}


static void _naMgr_cakMgr_CaSmatcardListener(const TCaListener *pxListener, TCaExportation *pxExportation)
{
	if (pxExportation != NULL)
	{
		caExportationDispose(pxExportation);
	}

	if (pxListener != s_pSmatcardListener)
	{
		HxLOG_Error("wrong listener (0x%x, 0x%x)\n", (unsigned int)pxListener, (unsigned int)s_pSmatcardListener);
		return;
	}

	CA_NA_CakRequest(CAK_SMARTCARD_INFO_REQ_ID);
}


static void _naMgr_cakMgr_GetSystemSmartCardInfo(TUnsignedInt32 ulSlot, TIccSessionId xSessionId, TCaSmartcard* pxSmartcard)
{
	TCaObjectStatus 		eObjStatus;

	TChar				*pxVersion = NULL;
	TChar				*pxSerialNumber = NULL;
	TCaSystemId			xEcmCaSystemId = 0;
	TCaSystemId			xEmmCaSystemId = 0;
	TPpid          			xProviderId = 0;
	TSmartcardInfoItem 	*pxSmartCardInfo = NULL;

	if (pxSmartcard == NULL)
	{
		HxLOG_Error("target is null...\n");
		return;
	}

	eObjStatus = caSmartcardGetVersion(pxSmartcard, (const TChar**)&pxVersion);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caSmartcardGetVersion (0x%x)\n", eObjStatus);
		return;
	}

	eObjStatus = caSmartcardGetSerialNumber(pxSmartcard, (const TChar**)&pxSerialNumber);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caSmartcardGetSerialNumber (0x%x)\n", eObjStatus);
		return;
	}

	eObjStatus = caSmartcardGetProviderId(pxSmartcard, &xProviderId);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caSmartcardGetProviderId (0x%x)\n", eObjStatus);
		return;
	}

	HxLOG_Print("caSmartcardGetProviderId (0x%X)\n", xProviderId);

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	// CAK7 DLK에서는 지원하지 않는 function임.
#else
	eObjStatus = caSmartcardGetEcmCaSystemId(pxSmartcard, &xEcmCaSystemId);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caSmartcardGetEcmCaSystemId (0x%x)\n", eObjStatus);
		return;
	}

	HxLOG_Print("caSmartcardGetEcmCaSystemId (0x%X)\n", xEcmCaSystemId);

	eObjStatus = caSmartcardGetEmmCaSystemId(pxSmartcard, &xEmmCaSystemId);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caSmartcardGetEmmCaSystemId (0x%x)\n", eObjStatus);
		return;
	}

	HxLOG_Print("caSmartcardGetEmmCaSystemId (0x%X)\n", xEmmCaSystemId);
#endif

	pxSmartCardInfo = &s_stSysScInfo[ulSlot];
	if (pxVersion != NULL)
	{
		HxLOG_Print("caSmartcardGetVersion (%s)\n", pxVersion);
		pxSmartCardInfo->pVersion = OxCAS_Malloc( strlen(pxVersion) + 1 );
		if (pxSmartCardInfo->pVersion != NULL)
		{
			HxSTD_memset( (void*)pxSmartCardInfo->pVersion, 0x00,  strlen(pxVersion) + 1 );
			HxSTD_memcpy( (TUnsignedInt8*)pxSmartCardInfo->pVersion, pxVersion, strlen(pxVersion) );
		}
	}

	if (pxSerialNumber != NULL)
	{
		HxLOG_Print("caSmartcardGetSerialNumber (%s)\n", pxSerialNumber);
		pxSmartCardInfo->pSerialNumber = OxCAS_Malloc( strlen(pxSerialNumber) + 1 );
		if (pxSmartCardInfo->pSerialNumber != NULL)
		{
			HxSTD_memset((void*)pxSmartCardInfo->pSerialNumber, 0x00,  strlen(pxSerialNumber) + 1 );
			HxSTD_memcpy( (TUnsignedInt8*) pxSmartCardInfo->pSerialNumber, pxSerialNumber, strlen(pxSerialNumber) );
		}
	}

	pxSmartCardInfo->iccSessionId = xSessionId;
	pxSmartCardInfo->provider= xProviderId;
	pxSmartCardInfo->ecmCaSystemId = xEcmCaSystemId;
	pxSmartCardInfo->emmCaSystemId = xEmmCaSystemId;
	pxSmartCardInfo->pRevision = NULL;
}


static void _naMgr_cakMgr_RequestSyncSmartCardStatus(void)
{
	TCaRequest			*pxRequest = NULL;
	TCaRequestStatus		eStatus;
	TCaObjectStatus 		eObjStatus;
	TCaProcessingStatus 	eProcStatus;

	TCaSmartcard			**ppCaScArray;
	TUnsignedInt32		i, ulNumOfScs = 0;
	TCalendarTime		stDateTime;
	TSmartcardState 		eScStatus;
	TIccSessionId			xSessionId = 0;

#if (CONFIG_USE_SC_ZIPCODE == 1)
 	TUnsignedInt32           	xZipCodeSize = 0;
  	TUnsignedInt8 		*pxZipCode = NULL;
 	TUnsignedInt8		ucIdx;
 	TUnsignedInt32		ulFactor = 1, ulZipCode = 0;
#endif

	NaSmartcardStatus_t 	*pxSmartCardStatus = NULL;
	NaCasMsg_t			stMsg;
	TSignedInt32			nErrCode;

	TUnsignedInt32		hCasHandle;
	TUnsignedInt16		usCaSysTbl[1] = {0x1800};

	HxLOG_Print("_naMgr_cakMgr_RequestSyncSmartCardStatus\n");

	eStatus = caRequestCreate(CA_REQUEST_TYPE_SMARTCARDS, &pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetObjects(pxRequest, &ulNumOfScs, (void***)&ppCaScArray);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfScs);
		caRequestDispose(pxRequest);
		return;
	}

	if ((((s_ulAlarmInBuff + 1) == ALARM_BUFFER_SIZE) ? 0 : (s_ulAlarmInBuff + 1)) == s_ulAlarmOutBuff)
	{
		HxLOG_Error("Alarm buffer overflow\n");
		caRequestDispose(pxRequest);
		return;
	}

	HxLOG_Print("Number Of Smarcards (%d)\n", ulNumOfScs);

	// update smartcard system info
	_naMgr_cakMgr_FreeSystemSmartCardInfo();
	_naMgr_cakMgr_InitSystemSmartCardInfo();
	s_usSysScInfoNb = ulNumOfScs;

	for (i=0; i<NO_SMC_SLOT; i++)
	{
		HxSTD_memset(&s_stScExpireInfo[i], 0x00, sizeof(ScExpirationInfo_t));
	}

	if (ulNumOfScs <= 0)
	{
		for (i=0; i<NO_SMC_SLOT; i++)
		{
#if (CONFIG_FOR_DAL_TEST_DEF == 0)
#if 1 //1 To to 필요여부 확인 후 추가 필요.
			if (CA_NA_PsiGetCasInstanceID((unsigned int*)&hCasHandle) != ERR_OK)
			{
				HxLOG_Error("error in CA_NA_PSI_ifExt_GetCasInstanceID\n");
			}
#endif
			if (svc_cas_MgrSubUpdateCasInfo(hCasHandle,
										eDxCAS_GROUPID_NA,
										(TUnsignedInt16)i,
										(TUnsignedInt16*)NULL,
										(TUnsignedInt32)0,
										(TUnsignedInt32)MAX_NO_SESSION,
										TRUE) != ERR_OK)
			{
				HxLOG_Error("error in svc_cas_MgrSubUpdateCasInfo\n");
			}
#endif
			pxSmartCardStatus = &s_stSmarcardAlarmItem[s_ulAlarmInBuff][i];
			pxSmartCardStatus->iccSessionId = i;
			pxSmartCardStatus->changed = 1;
			pxSmartCardStatus->alarm = CA_ALARM_SMARTCARD_REMOVED;
		}
	}
	else
	{
		TUnsignedInt16 usSlotId;

		for (i=0; i<ulNumOfScs && i<NO_SMC_SLOT; i++)
		{
			eObjStatus = caSmartcardGetState(ppCaScArray[i], &eScStatus);
			if (eObjStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("error in caSmartcardGetState (0x%x)\n", eObjStatus);
			}

			HxLOG_Print("caSmartcardGetState (%d)\n", eScStatus);

			eObjStatus = caSmartcardGetSessionId(ppCaScArray[i], &xSessionId);
			if (eObjStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("error in caSmartcardGetSessionId (0x%x)\n", eObjStatus);
			}
			HxLOG_Print("caSmartcardGetSessionId (%d)\n", xSessionId);

#if (CONFIG_USE_SC_ZIPCODE == 1)
{
			TUnsignedInt32 j;

			eObjStatus = caSmartcardGetZipCode(ppCaScArray[i], (TSize*)&xZipCodeSize, (const TUnsignedInt8**)&pxZipCode);
			if (eObjStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("error in caSmartcardGetZipCode (0x%x)\n", eObjStatus);
			}

			for (ucIdx = xZipCodeSize; ucIdx > 0; ucIdx--)
			{
				ulFactor += ((xZipCodeSize - ucIdx) * 10);
				ulZipCode += (ulFactor * pxZipCode[ucIdx - 1]);
			}
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)	// cak test 중 CAK-MER-EMM-04 test log -> 150
			HxLOG_Print("\n\n ZipCode Size(%d)\n", xZipCodeSize);
			if (xZipCodeSize != 0)
			{
				for (j=0; j < xZipCodeSize; j++)
				{
					HxLOG_Print(" %02X", pxZipCode[j]);
				}
#if defined(CONFIG_DEBUG)
					HxLOG_Print("\n[CAK-MER-MEE-04] ZipCode Value = %d \n", pxZipCode[j-1]);
#else
					{
						HUINT8 ucTmp[64];
						HxSTD_memset(ucTmp, 0x00, 64);
						snprintf(ucTmp, 64, "[CAK-MER-EMM-04] ZipCode Value = %d\n", pxZipCode[j-1]);
						MW_CA_NA_PrintForRelease(ucTmp);
					}
#endif
			}
#endif

#if 0
			HxSTD_memset(ucZipCode, 0x00, MAX_ZIPCODE_SIZE);
			xZipCodeSize = (xZipCodeSize > MAX_ZIPCODE_SIZE) ? MAX_ZIPCODE_SIZE : xZipCodeSize;
			HxSTD_memcpy(ucZipCode, pxZipCode, xZipCodeSize);
			HxLOG_Print("ZipCode :");

			for (j=0; j<xZipCodeSize; j++)
			{
				HxLOG_Print(" %02X", ucZipCode[j]);
			}
			HxLOG_Print("\n");
#endif
}
#endif

			_naMgr_cakMgr_GetSystemSmartCardInfo(i, xSessionId, ppCaScArray[i]);

			pxSmartCardStatus = &s_stSmarcardAlarmItem[s_ulAlarmInBuff][i];
			pxSmartCardStatus->iccSessionId = xSessionId;
			pxSmartCardStatus->changed = 1;
			pxSmartCardStatus->alarm = eScStatus;
#if (CONFIG_USE_SC_ZIPCODE == 1)
			s_ulZipCodeInfo[i] = ulZipCode;
#endif
			eObjStatus = caSmartcardGetExpirationDate(ppCaScArray[i], &stDateTime);
			if (eObjStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("error in caSmartcardGetExpirationDate (0x%x)\n", eObjStatus);
				s_stScExpireInfo[i].bEndOfTime = TRUE;
				s_stScExpireInfo[i].bGotScExpireDate = TRUE;
			}
			else
			{
				HxLOG_Print("Expiration Data : Day (%d-%d-%d)\n", stDateTime.year, stDateTime.month, stDateTime.day);
				HxLOG_Print("Expiration Data : Time (%d-%d-%d)\n", stDateTime.hour, stDateTime.minute, stDateTime.second);

				HxSTD_memcpy(&s_stScExpireInfo[i].stScExpireDate, &stDateTime, sizeof(TCalendarTime));
				s_stScExpireInfo[i].bGotScExpireDate = TRUE;
			}

#if 1 //1 To to 필요여부 확인 후 추가 필요.

			if (CA_NA_PsiGetCasInstanceID((unsigned int*)&hCasHandle) != ERR_OK)
			{
				HxLOG_Error("error in CA_NA_PSI_ifExt_GetCasInstanceID\n");
			}
#endif

#if (CONFIG_SET_SC_CASYSID == 1)
			if (eScStatus == CA_SMARTCARD_STATE_OK)
			{
				usCaSysTbl[0] = s_stSysScInfo[i].ecmCaSystemId;
			}
#endif

#if (CONFIG_FOR_DAL_TEST_DEF == 1)
			usSlotId = 0;
#else
  #if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
			usSlotId = i;	// DLK에서는 xSessionId값이 -1이기에 slot값이로는 맞지 않다.
  #else
			usSlotId = xSessionId;
  #endif
#endif
			if (svc_cas_MgrSubUpdateCasInfo(hCasHandle,
										eDxCAS_GROUPID_NA,
										(HUINT16)usSlotId,
										(HUINT16 *)usCaSysTbl,
										(HUINT32)1,
										(HUINT32)MAX_NO_SESSION,
										TRUE) != ERR_OK)
			{
				HxLOG_Error("error in svc_cas_MgrSubUpdateCasInfo\n");
			}
		}
	}

	caRequestDispose(pxRequest);

	stMsg.wMsg		= NA_CAK_ID;
	stMsg.lParam2		= CAK_ALARM_ACCESS;
	stMsg.lParam3		= NO_SMC_SLOT;
	stMsg.lParam1		= (TSignedInt32)s_ulAlarmInBuff;
	stMsg.info		= 0;

	if (ulNumOfScs > 0)
		s_ucNotiScInfo = 1;

	s_ulAlarmInBuff++;
	if (s_ulAlarmInBuff == ALARM_BUFFER_SIZE)
		s_ulAlarmInBuff = 0;

	nErrCode = VK_PostMessage(s_ulCakQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
		s_ucNotiScInfo = 0;
	}
}


static void _naMgr_cakMgr_GetSmartCardStatus(void)
{
	TCaRequest 			*pxRequest = NULL;
	TCaRequestStatus 		eStatus;
	TCaObjectStatus 		eObjStatus;
	TCaProcessingStatus	eProcStatus;

	TCaSmartcard 		**ppCaScArray;
	TUnsignedInt32 		i, ulNumOfScs = 0;
	TSmartcardState		eScStatus;
	TIccSessionId    		xSessionId = 0;
	NaSmartcardStatus_t 	*pxSmartCardStatus = NULL;

	eStatus = caRequestCreate(CA_REQUEST_TYPE_SMARTCARDS, &pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetObjects(pxRequest, &ulNumOfScs, (void***)&ppCaScArray);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfScs);
		caRequestDispose(pxRequest);
		return;
	}

	HxLOG_Print("Number Of Smarcards (%d)\n", ulNumOfScs);

	if (ulNumOfScs <= 0)
	{
		for (i=0; i<NO_SMC_SLOT; i++)
		{
			pxSmartCardStatus = &s_stSmarcardAlarmItem[s_ulAlarmInBuff][i];
			pxSmartCardStatus->iccSessionId = i;
			pxSmartCardStatus->changed = 1;
			pxSmartCardStatus->alarm = CA_ALARM_SMARTCARD_REMOVED;
		}
	}
	else
	{
		for (i=0; i<ulNumOfScs && i<NO_SMC_SLOT; i++)
		{
			eObjStatus = caSmartcardGetState(ppCaScArray[i], &eScStatus);
			if (eObjStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("error in caSmartcardGetState (0x%x)\n", eObjStatus);
			}

			if (eScStatus != CA_SMARTCARD_STATE_OK)
			{
				HxLOG_Error("CA_SMARTCARD_STATE_OK is not set....(0x%X)\n", eScStatus);
			}

			eObjStatus = caSmartcardGetSessionId(ppCaScArray[i], &xSessionId);
			if (eObjStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("error in caSmartcardGetSessionId (0x%x)\n", eObjStatus);
			}

			HxLOG_Print("caSmartcardGetSessionId (%d)\n", xSessionId);

			pxSmartCardStatus = &s_stSmarcardAlarmItem[s_ulAlarmInBuff][i];
			pxSmartCardStatus->iccSessionId = xSessionId;
			pxSmartCardStatus->changed = 1;
			pxSmartCardStatus->alarm= eScStatus;
		}
	}

	caRequestDispose(pxRequest);
}


static void _naMgr_cakMgr_CaProgramsListener(const TCaListener *pxListener, TCaExportation *pxExportation)
{
	if (pxExportation != NULL)
	{
		caExportationDispose(pxExportation);
	}

	if (pxListener != s_pProgramsListener)
	{
		HxLOG_Error("wrong listener (0x%x, 0x%x)\n", (unsigned int)pxListener, (unsigned int)s_pProgramsListener);
		return;
	}

	CA_NA_CakRequest(CAK_PROGRAMS_INFO_REQ_ID);
}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static void _naMgr_cakMgr_CaPopupListener(const TCaListener *pxListener, TCaExportation *pxExportation)
{
	TCaObjectStatus	eObjStatus;
	TUnsignedInt32	xNumOfPopup;
	TCaPopup		**ppxPopups;
	TSize			xPopupSize;
	TUnsignedInt8	*pxText;
	HINT32			i;
	NaPopupUI_t		*pstPopupUi;

	if (pxExportation == NULL)
	{
		return;
	}

	if (pxListener != s_pPopupListener)
	{
		HxLOG_Error("wrong listener (0x%x, 0x%x)\n", (unsigned int)pxListener, (unsigned int)s_pPopupListener);
		goto _caExportationDispose;
	}

	eObjStatus = caExportationGetObjects(pxExportation, &xNumOfPopup, (void***)&ppxPopups);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caExportationGetObjects.. (0x%x)\n", eObjStatus);

		pstPopupUi = (NaPopupUI_t *)OxCAS_Calloc(sizeof(NaPopupUI_t));
		if(pstPopupUi == NULL)
		{
			HxLOG_Error("OxCAS_Calloc() fail \n");
			return;
		}

		pstPopupUi->eType = ePopup_MessageRemove;
		pstPopupUi->pucMsg = (HUINT8 *)OxCAS_Calloc(1);
		if(pstPopupUi->pucMsg == NULL)
		{
			HxLOG_Error("OxCAS_Calloc() fail \n");
			OxCAS_Free(pstPopupUi);
			return;
		}
		NAMGR_CAKMGR_SendPopupMessage(pstPopupUi);

		goto _caExportationDispose;
	}

	for (i = 0 ; i < xNumOfPopup ; i++)
	{
		if (ppxPopups[i] != NULL)
		{
			pstPopupUi = (NaPopupUI_t *)OxCAS_Calloc(sizeof(NaPopupUI_t));
			if(pstPopupUi == NULL)
			{
				HxLOG_Error("pstPopupUi is NULL\n");
				continue;
			}

			eObjStatus = caPopupGetText(ppxPopups[i], &xPopupSize, (const TUnsignedInt8 **)&pxText);
			if(eObjStatus == CA_OBJECT_NOT_AVAILABLE)
			{
				pstPopupUi->eType = ePopup_Message;
			}
			else if (eObjStatus != CA_OBJECT_NO_ERROR)
			{
				OxCAS_Free(pstPopupUi);
				HxLOG_Error("error in caPopupGetText.. (0x%x)\n", eObjStatus);
				continue;
			}
			else
			{
				pstPopupUi->eType = ePopup_Message;
				pstPopupUi->pucMsg = (HUINT8 *)OxCAS_Calloc(xPopupSize + 1);
				if(pstPopupUi->pucMsg != NULL)
					HxSTD_memcpy(pstPopupUi->pucMsg, pxText, xPopupSize);
			}

			NAMGR_CAKMGR_SendPopupMessage(pstPopupUi);
		}
	}

_caExportationDispose:
	caExportationDispose(pxExportation);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
static void _naMgr_cakMgr_CaOperatorsListener(const TCaListener *pxListener, TCaExportation *pxExportation)
{
	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	if (pxExportation != NULL)
	{
		caExportationDispose(pxExportation);
	}

	if(pxListener != s_pOperatorsListener)
	{
		HxLOG_Error("[%s:%d] wrong listener (0x%x, 0x%x)\n", pxListener, s_pOperatorsListener);
		return;
	}

	CA_NA_CakRequest(CAK_OPERATORS_REQ_ID);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)	// cak test 중 CAK-MER-EMM-04 test log -> 150

STATIC void local_HexPrint(HUINT16 ucgap, HUINT32 ulSize, HUINT8* p_data)
{
        HUINT32 i = 0;

#if defined(CONFIG_DEBUG)
        HxLOG_Print("[local_HexPrint] hex_data[%d] : \n", ulSize);
        HLIB_CMD_Printf(" 0000 : ");
        for (i = 0; i < ulSize; i++)
        {
                HLIB_CMD_Printf("%02X ", p_data[i]);

              	if (i % ucgap == (ucgap-1))
             	{
                HLIB_CMD_Printf("\n %04x : ", i+1);
              	}
        }
        HLIB_CMD_Printf("\n\n");
#else
	{
		HUINT8 ucTmp[64];
		HUINT8 ucTmp_Big[2048];

		HxSTD_memset(ucTmp, 0x00, 64);
		snprintf(ucTmp, 64, "[CAK-MER-PSI][MGR] hex_data[PMT size = %d] : \n", ulSize);
		if (ulSize > 2048)
			return;

		MW_CA_NA_PrintForRelease(ucTmp);

		MW_CA_NA_PrintForRelease(" 0000 : ");

		HxSTD_memset(ucTmp_Big, 0x00, 2048);
		for (i = 0; i < ulSize; i++)
		{
			snprintf(ucTmp_Big, 2048, "%02X ", p_data[i]);
			MW_CA_NA_PrintForRelease(ucTmp_Big);

			if (i % ucgap == (ucgap-1))
			{
				HxSTD_memset(ucTmp_Big, 0x00, 2048);
				snprintf(ucTmp_Big, 2048, "\n 0x%04x : ", i+1);
				MW_CA_NA_PrintForRelease(ucTmp_Big);
			}
		}
		MW_CA_NA_PrintForRelease(" end : ");
	}
#endif


        return;
}
#endif

#if (CONFIG_DSC_METADATA == 0)
static HERROR _naMgr_cakMgr_GetMetaDescriptor(TUnsignedInt8 *pxInputData, TUnsignedInt32 xInputDataLength,
											TUnsignedInt8 xTag, TUnsignedInt8 **ppxOutputData, TUnsignedInt8 *pxOutputDataLength)
{
	TUnsignedInt8 ucTag;
	TUnsignedInt8 ucHeadLen = 0;
	TUnsignedInt8 ucDataLen = 0;
	TUnsignedInt8 *pucInputData = pxInputData;

	if ( (pxInputData == NULL) || (xInputDataLength <= 0)
		|| (ppxOutputData == NULL) || (pxOutputDataLength == NULL) )
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	//*ppxOutputData = NULL;
	*pxOutputDataLength = 0;

	while ((TUnsignedInt32)(pucInputData - pxInputData) < xInputDataLength)
	{
		ucTag = *pucInputData;
		if ((*pucInputData & 0x80) == 0)
		{
			// Format 1
			ucDataLen = *pucInputData & 0x0F;
			ucHeadLen = 1;
			ucTag = (*pucInputData >> 4) & 0x0F;
		}
		else if ((*pucInputData & 0xC0) == 0x80)
		{
			// Format 2
			ucDataLen = *(pucInputData+1);
			ucHeadLen = 2;
		}
		else if ((*pucInputData & 0xF0) == 0xC0)
		{
			// Format 3
			ucDataLen = 4;
			ucHeadLen = 1;
		}
		else if ((*pucInputData & 0xF0) == 0xD0)
		{
			// Format 4
			ucDataLen = 3;
			ucHeadLen = 1;
		}
		else if ((*pucInputData & 0xF0) == 0xE0)
		{
			// Format 5
			ucDataLen = 2;
			ucHeadLen = 1;
		}
		else if ((*pucInputData & 0xF0) == 0xF0)
		{
			// Format 6
			ucDataLen = 1;
			ucHeadLen = 1;
		}

		if (ucTag == xTag)
		{
			// Yes, this is the descriptor that I'm looking for.
			*pxOutputDataLength = ucDataLen;
			*ppxOutputData = pucInputData + ucHeadLen;
			return ERR_OK;
		}
		else
		{
			// This is not the right descriptor, go to the next one.
			pucInputData = pucInputData + ucHeadLen + ucDataLen;
		}
	} // while
	return ERR_FAIL;
}
#endif


static void _naMgr_cakMgr_RequestEnablingConsumption(const TCaProgram *pxProgram, TBoolean bOneShot)
{
	TCaRequestStatus 		eStatus;
	TCaProcessingStatus	eProcStatus;
	TCaRequest			*pxRequest = NULL;

	eStatus = caRequestCreate(CA_REQUEST_TYPE_ENABLE_CONSUMPTION, &pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return;
	}

	eStatus = caRequestSetProgram(pxRequest, pxProgram);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		caRequestDispose(pxRequest);
		return;
	}

	if (bOneShot == TRUE)
	{
		eStatus = caRequestSetConsumptionMode(pxRequest, CA_CONSUMPTION_MODE_ONE_SHOT);
	}
	else
	{
		eStatus = caRequestSetConsumptionMode(pxRequest, CA_CONSUMPTION_MODE_CONTINUOUS);
	}

	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || ( eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pxRequest);
		return;
	}

	caRequestDispose(pxRequest);
}


#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
static void _naMgr_cakMgr_EnablingConsumptionForced(TBoolean bOneShot)
{
	TCaRequestStatus 			eStatus;
	TCaObjectStatus 			eObjStatus;
	TCaProcessingStatus		eProcStatus;

	TCaRequest	 			*pxRequest = NULL;
	TCaProgram 				**ppCaPgmArray;
	TUnsignedInt32 			i, ulNumOfPgms = 0;
	TCaAccess				eCaAccess;

	eStatus = caRequestCreate(CA_REQUEST_TYPE_PROGRAMS, &pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || ( eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetObjects(pxRequest, &ulNumOfPgms, (void***)&ppCaPgmArray);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (ulNumOfPgms <= 0) )
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfPgms);
		caRequestDispose(pxRequest);
		return;
	}

	HxLOG_Print("Program number (%d)\n", (int)ulNumOfPgms);

	for (i=0; i<ulNumOfPgms; i++)
	{
		eObjStatus = caProgramGetAccess((const TCaProgram*)ppCaPgmArray[i], &eCaAccess);
		if (eObjStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Error("error in caProgramGetAccess (0x%x)\n", eObjStatus);
		}

		HxLOG_Print("Access (0x%x)\n", eCaAccess);

		if (eCaAccess == CA_ACCESS_DENIED_DIALOG_REQUIRED)
		{
			_naMgr_cakMgr_RequestEnablingConsumption((const TCaProgram*)ppCaPgmArray[i], bOneShot);
		}
	}

	caRequestDispose(pxRequest);
}
#endif


static void _naMgr_cakMgr_RequestSyncProgramInfo(void)
{
	TCaRequestStatus 		eStatus;
	TCaObjectStatus 		eObjStatus;
	TCaProcessingStatus	eProcStatus;

	TCaRequest 			*pxRequest = NULL;
	TCaProgram 			**ppCaPgmArray;
	TUnsignedInt32 		i, ulNumOfPgms = 0;
	TUnsignedInt32		ulRealNumOfPgms = 0;
	TCaAccess			eCaAccess;
	TTransportSessionId	ulSessionId;
	NaAccessAlarm_t		*pxAccessAlarm = NULL;

	NaCasMsg_t 			stMsg;
	TSignedInt32 			nErrCode = 0;

#if (CONFIG_DSC_METADATA == 0)
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	TUnsignedInt8			*pucData;
	TUnsignedInt8			*pucMetaData;
	TUnsignedInt32		ulMetaDataLen = 0;
	TUnsignedInt8			ucDrmSize;
#endif
	TUnsignedInt8			ucDrmData[2];
	TBoolean				bFoundDrm;
#endif

	eStatus = caRequestCreate(CA_REQUEST_TYPE_PROGRAMS, &pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || ( eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pxRequest);
		return;
	}

	eStatus = caRequestGetObjects(pxRequest, &ulNumOfPgms, (void***)&ppCaPgmArray);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (ulNumOfPgms <= 0) )
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfPgms);
		caRequestDispose(pxRequest);
		return;
	}

	if ((((s_ulAlarmInBuff + 1) == ALARM_BUFFER_SIZE) ? 0 : (s_ulAlarmInBuff + 1)) == s_ulAlarmOutBuff)
	{
		HxLOG_Error("Alarm buffer overflow\n");
		caRequestDispose(pxRequest);
		return;
	}

	HxLOG_Print("Program number (%d)\n", (int)ulNumOfPgms);
	ulRealNumOfPgms = ulNumOfPgms;

	for (i=0; i<ulNumOfPgms; i++)
	{
		eObjStatus = caProgramGetTransportSessionId((const TCaProgram*)ppCaPgmArray[i], &ulSessionId);
		if (eObjStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Error("error in caProgramGetTransportSessionId (0x%x)\n", eObjStatus);
		}

		HxLOG_Print("Transport_session (0x%x)\n", (unsigned int)ulSessionId);

		eObjStatus = caProgramGetAccess((const TCaProgram*)ppCaPgmArray[i], &eCaAccess);
		if (eObjStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Error("error in caProgramGetAccess (0x%x)\n", eObjStatus);
		}

		HxLOG_Print("Access (0x%x)\n", eCaAccess);

#if (CONFIG_DSC_METADATA == 0)
		bFoundDrm = FALSE;
		HxSTD_memset(ucDrmData, 0xFF, 2);

	#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
		if(eCaAccess == CA_ACCESS_GRANTED || eCaAccess == CA_ACCESS_FREE)
		{
			TCaContent *pxContent;

			eObjStatus = caProgramGetContent((const TCaProgram*)ppCaPgmArray[i], &pxContent);
			if (eObjStatus == CA_OBJECT_NO_ERROR)
			{
				eObjStatus = caContentGetMetadata(pxContent, (TSize *)&ulMetaDataLen, &pucMetaData);
				if (eObjStatus == CA_OBJECT_NO_ERROR)
				{
					if (_naMgr_cakMgr_GetMetaDescriptor(pucMetaData, ulMetaDataLen, (TUnsignedInt8)eTagCopyCtlInfo, &pucData, &ucDrmSize) == ERR_OK)
					{
						bFoundDrm = TRUE;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
						local_HexPrint(32, ucDrmSize, pucData);
#endif
						NAMGR_SetMetaDescriptor(ulSessionId, eTagCopyCtlInfo, pucData, ucDrmSize);
					}

					if (_naMgr_cakMgr_GetMetaDescriptor(pucMetaData, ulMetaDataLen, (TUnsignedInt8)eTagUriInfo, &pucData, &ucDrmSize) == ERR_OK)
					{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
						local_HexPrint(32, ucDrmSize, pucData);
#endif
						NAMGR_SetMetaDescriptor(ulSessionId, eTagUriInfo, pucData, ucDrmSize);
					}
				}
			}
		}
	#elif defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		eObjStatus = caProgramGetMetadata((const TCaProgram*)ppCaPgmArray[i], &pucMetaData, (TSize*)&ulMetaDataLen);
		if (eObjStatus == CA_OBJECT_NO_ERROR)
		{
			if (_naMgr_cakMgr_GetMetaDescriptor(pucMetaData, ulMetaDataLen, (TUnsignedInt8)eTagCopyCtlInfo, &pucData, &ucDrmSize) == ERR_OK)
			{
				bFoundDrm = TRUE;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
				local_HexPrint(32, ucDrmSize, pucData);
#endif
			}
			//불안정하다. 일단 cak test 만....
			#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
			if (_naMgr_cakMgr_GetMetaDescriptor(pucMetaData, ulMetaDataLen, (TUnsignedInt8)eTagUriInfo, &pucData, &ucDrmSize) == ERR_OK)
			{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
				local_HexPrint(32, ucDrmSize, pucData);
#endif
				NAMGR_SetMetaDescriptor(ulSessionId, eTagUriInfo, pucData, ucDrmSize);
			}
			#endif
		}
	#endif

		#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		if (bFoundDrm == TRUE)
		{
			HxSTD_memcpy(ucDrmData, pucData, 2);
			HxLOG_Print("DRM Info (A:0x%x, B:0x%x)\n", ucDrmData[0], ucDrmData[1]);
		}
		#endif

	#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST [ 테스트 완료후 open한다 ]
		CA_NA_CopReceivedData(ulSessionId, ucDrmData);	// _naMgr_procDrmCopInfo
	#endif
#endif

#if !defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
		if (eCaAccess == CA_ACCESS_DENIED_DIALOG_REQUIRED)
		{
			_naMgr_cakMgr_RequestEnablingConsumption((const TCaProgram*)ppCaPgmArray[i], FALSE);
			ulRealNumOfPgms--;
			continue;
		}
#endif

		pxAccessAlarm = &s_stStreamAlarmItem[s_ulAlarmInBuff][i];
		pxAccessAlarm->transportSessionId = ulSessionId;
		pxAccessAlarm->alarm = eCaAccess;
		pxAccessAlarm->changed = 1;
		pxAccessAlarm->serviceId = 0;
		pxAccessAlarm->elementaryStreamPid = 0;
		pxAccessAlarm->iccSessionId = 0;
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
		NAMGR_CAKSVC_SetProgramAccess((Handle_t)ulSessionId, eCaAccess);
#endif
	}

	caRequestDispose(pxRequest);

	if (ulRealNumOfPgms > 0)
	{
#if (1)
		_naMgr_cakMgr_GetSmartCardStatus();
#endif

		stMsg.wMsg 		= NA_CAK_ID;
		stMsg.lParam2 	= CAK_ALARM_ACCESS;
		stMsg.lParam3 	= NO_SMC_SLOT;
		stMsg.lParam1 	= (TSignedInt32)s_ulAlarmInBuff;
		stMsg.info 		= ulRealNumOfPgms;

		s_ulAlarmInBuff++;
		if (s_ulAlarmInBuff == ALARM_BUFFER_SIZE)
			s_ulAlarmInBuff = 0;

		nErrCode = VK_PostMessage(s_ulCakQueueId, (void *)&stMsg, QUEUE_SIZE_1);
		if (nErrCode != ERR_OK)
		{
			HxLOG_Error("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
		}
	}
}

static void _naMgr_cakMgr_CaIrdCmdListener(const TCaListener *pxListener, TCaExportation *pxExportation)
{
	TCaObjectStatus 	eObjStatus;
	TUnsignedInt32 	i, ulNumIrdCmds;
	TCaIrdCommand	**ppIrdCommands;
	TSize 			ulIrdCmdSize;
	TUnsignedInt8		*pIrdCmdData;

	HxLOG_Error("+++ (>>> IRD cmd entry point...) \n");

	if (pxExportation != NULL)
	{
		if (pxListener != s_pIrdCmdListener)
		{
			HxLOG_Error("wrong listener (0x%x, 0x%x)\n", (unsigned int)pxListener, (unsigned int)s_pIrdCmdListener);
		}
		else
		{
			eObjStatus = caExportationGetObjects(pxExportation, &ulNumIrdCmds, (void***)&ppIrdCommands);
			if (eObjStatus == CA_OBJECT_NO_ERROR)
			{
				for (i=0; i<ulNumIrdCmds; i++)
				{
					if (ppIrdCommands[i] != NULL)
					{
						eObjStatus = caIrdCommandGetData(ppIrdCommands[i], &ulIrdCmdSize, &pIrdCmdData);
						if (eObjStatus == CA_OBJECT_NO_ERROR)
						{
							_naMgr_cakMgr_ExecIrdCmdExportation((TUnsignedInt8)ulIrdCmdSize, pIrdCmdData);
						}
						else
						{
							HxLOG_Error("error in caIrdCommandGetData.. (0x%x)\n", eObjStatus);
						}
					}
				}
			}
			else
			{
				HxLOG_Error("error in caExportationGetObjects.. (0x%x)\n", eObjStatus);
			}
		}
		caExportationDispose(pxExportation);
	}
}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
static void _naMgr_cakMgr_RequestSyncOperatorsInfo(void)
{
	TCaRequest *pxRequest;
	TUnsignedInt32 xNumOfObjs;
	void **ppxObjArray;
	TOperatorFlags xFlags;
	TCaProcessingStatus eProcStatus;
	TCaObjectStatus eCaObjStatus;
	TCaRequestStatus eStatus;
	NaPopupUI_t		*pstPopupUi;
	NaCasMsg_t		stMsg;
	int i;

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	eStatus = caRequestCreate(CA_REQUEST_TYPE_OPERATORS, &pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		goto _caRequestDispose;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || ( eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		goto _caRequestDispose;
	}

	eStatus = caRequestGetObjects(pxRequest, &xNumOfObjs, (void***)&ppxObjArray);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (xNumOfObjs <= 0) )
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, xNumOfObjs);
		goto _caRequestDispose;
	}

	HxLOG_Print("[%s:%d] xNumOfObjs (%d)\n", __FUNCTION__, __HxLINE__, xNumOfObjs);

	for(i = 0 ; i < xNumOfObjs ; i++)
	{
		eCaObjStatus = caOperatorGetFlags((const TCaOperator *)ppxObjArray[i], &xFlags);
		if(eCaObjStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Error("error in caOperatorGetFlags(%d, 0x%x)\n", i, eCaObjStatus);
			continue;
		}

		HxLOG_Print("[%s:%d] Operator Flags (0x%x)\n", __FUNCTION__, __HxLINE__, xFlags);

		if(xFlags & CA_OPERATOR_ADMINISTRATOR)
		{
			HxLOG_Print("[%s:%d] CA_OPERATOR_ADMINISTRATOR flag is raised\n", __FUNCTION__, __HxLINE__);

			if(xFlags & CA_OPERATOR_INCONSISTENT)
			{
				HxLOG_Warning("Catastrophic failure!!!\n");

				pstPopupUi = (NaPopupUI_t *)OxCAS_Calloc(sizeof(NaPopupUI_t));
				if(pstPopupUi == NULL)
				{
					HxLOG_Error("pstPopupUi is NULL\n");
					continue;
				}

				pstPopupUi->eType = ePopup_Catastrophic;

				NAMGR_CAKMGR_SendPopupMessage(pstPopupUi);
			}
		}
	}

_caRequestDispose:
	caRequestDispose(pxRequest);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static void _naMgr_cakMgr_CaAccessRightListener(const TCaListener *pxListener, TCaExportation *pxExportation)
{
	TCaObjectStatus 	eObjStatus;
	NaCasMsg_t 			stMsg;
	HxLOG_Print("CaAccessRightListener Enter \n");

	if (pxListener != s_pAccessRightListener)
	{
		HxLOG_Error("wrong listener (0x%x)\n", (unsigned int)pxListener);
	}
	else
	{
		//여기다 액션 집어넣기...
		stMsg.wMsg		= NA_CAK_ID;
		stMsg.lParam2		= CAK_ACCESSRIGHT_LISTENER_NOTIFICATION;
		VK_Print("[_naMgr_cakMgr_CaAccessRightListener] CAK_ACCESSRIGHT_LISTENER_NOTIFICATION Message Send \n");
		eObjStatus = VK_PostMessage(s_ulCakQueueId, (void *)&stMsg, QUEUE_SIZE_1);
		if (eObjStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Error("error in _naMgr_cakMgr_CaAccessRightListener.. (0x%x)\n", eObjStatus);
		}
	}

}
#endif

static void _naMgr_cakMgr_ExecIrdCmdExportation(TSize xIrdCmdLength, TUnsignedInt8 *pxIrdCmd)
{
	 NaCasMsg_t		stMsg;
	 TSignedInt32		nErrCode;
	 TBoolean		bValidCmd = FALSE;

	 if ( (xIrdCmdLength == 0)
	 	|| (xIrdCmdLength > IRDCMD_MAX_CMD_LEN)
	 	|| (pxIrdCmd == NULL)  )
	 {
		 HxLOG_Error("xIrdCmdLength == 0 || pxIrdCmd == NULL\n");
		 return;
	 }

	 switch (*(pxIrdCmd + 6))
	 {
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_PINCTL)
		 case 0x12:  // Reset PIN Code
		 case 0xC8:  // Set PIN Code
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
		 case 0xC0:  // Mail
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_TUNE)
		 case 0xC1:  // Force Tune
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FINGER)
		 case 0xC2:  // Force Identification
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_STANDBY)
		 case 0xC9:  // Force Standby
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_DEFSETTING)
		 case 0xCC:  // Force Default Setting
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
		 case 0xCF:  // Pop-up message
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_USAGE)
		 case 0xD3:  // Change Usage ID
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FORMATDISK)
		 case 0xD5:  // logical disk format
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
		case 0xD8:
			bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
		case 0xD9:	// EMM wake-up
			bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_REBOOT)
		 case 0xDB:  // Reboot stb
		 	bValidCmd = TRUE;
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		 case 0x53:	 // APS HD(DisplayProfile & Message
		 	HxLOG_Print("Command Id = [0x%x]\n", *(pxIrdCmd + 6));
		 	bValidCmd = TRUE;
		 	break;
#endif
	 }

	 if (bValidCmd == FALSE)
	 {
		 HxLOG_Error("Not valid Command (0x%x)!!!!\n", *(pxIrdCmd + 6));
		 return;
	 }

	 HxSTD_memset(s_ucIrdCmdBuff[s_ulIrdcmdInBuff], 0x00, IRDCMD_MAX_CMD_LEN);
	 HxSTD_memcpy(s_ucIrdCmdBuff[s_ulIrdcmdInBuff], pxIrdCmd, xIrdCmdLength);
	 s_ulIrdcmdInBuff = ((s_ulIrdcmdInBuff + 1) >= IRDCMD_BUFFER_SIZE) ? 0 : s_ulIrdcmdInBuff + 1;

	 HxSTD_memset(&stMsg, 0, sizeof(NaCasMsg_t));
	 stMsg.wMsg = NA_CAK_ID;
	 stMsg.lParam2 = CAK_IRD_COMMAND;

	 nErrCode = VK_PostMessage(s_ulCakQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	 if (nErrCode != ERR_OK)
	 {
		 HxLOG_Error("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	 }
}


static HERROR _naMgr_cakMgr_CheckPinCode(TCaPinCodeId xPinCodeId, TPinCode xPinCode, TPinCodeStatus *xStatus)
{
	TCaRequestStatus 		eStatus;
	TCaObjectStatus 		eObjStatus;
	TCaProcessingStatus	eProcStatus;
	TCaRequest			*pCaReqInfo = NULL;
	TCaRequest			*pCaReqCheck = NULL;

	TCaPinCodeId			xPinId;
	TPinCodeFlags			xPinFlags;
	TFlagsMask 			xFlagsMask;
	TCaPinCode			**ppCaPinArray;
	TCaPinCode			*pMyPinCode = NULL;

	TUnsignedInt32 		i, ulNumOfPins = 0;
	TUnsignedInt8 		aucPinCode[4];

	if (xStatus == NULL)
		return ERR_FAIL;

//	Please check PIN# as follows.
//	(Pincode: 1234) 0x 00 00 00 00 00 00 04 D2
//	(1234=0x4D2)

	HxSTD_memset(aucPinCode, 0x00, 4);
	HxSTD_memcpy(aucPinCode,&xPinCode, 4);

	aucPinCode[2] = aucPinCode[1];
	aucPinCode[3] = aucPinCode[0];
	aucPinCode[0] = 0;
	aucPinCode[1] = 0;


	HxLOG_Print("Pincode [%d]\n", (int)xPinCode);
	HxLOG_Print("PinCode : 0x%02x%02x%02x%02x\n", aucPinCode[0], aucPinCode[1], aucPinCode[2], aucPinCode[3]);

	*xStatus = CA_PIN_CODE_ERROR;
	/* step 1 : Get PinCode object */
	eStatus = caRequestCreate(CA_REQUEST_TYPE_PIN_CODES_INFO, &pCaReqInfo);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return ERR_FAIL;
	}

	eStatus = caRequestSend(pCaReqInfo);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	eStatus = caRequestGetProcessingStatus(pCaReqInfo, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	eStatus = caRequestGetObjects(pCaReqInfo, &ulNumOfPins, (void***)&ppCaPinArray);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (ulNumOfPins <= 0) )
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfPins);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	for (i=0; i<ulNumOfPins; i++)
	{
		eObjStatus = caPinCodeGetId(ppCaPinArray[i], &xPinId);
		if (eObjStatus != CA_OBJECT_NO_ERROR)
		{
			break;
		}

		if (xPinId == xPinCodeId)
		{
			pMyPinCode = ppCaPinArray[i];
			break;
		}
	}

	if ( (eObjStatus != CA_OBJECT_NO_ERROR) || (pMyPinCode == NULL) )
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfPins);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	HxLOG_Print("CaPinCode Object = 0x%x\n", pMyPinCode);

	eObjStatus = caPinCodeGetFlags(pMyPinCode, &xPinFlags);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caPinCodeGetFlags (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	eObjStatus = caPinCodeGetFlagsMask(pMyPinCode, &xFlagsMask);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caPinCodeGetFlagsMask (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	if (xFlagsMask & xPinFlags & CA_PIN_CODE_SUSPENDED)
	{
		HxLOG_Error("Your PIN code is suspended. Call the operator to reactivate it\n");
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	if (xFlagsMask & xPinFlags & CA_PIN_CODE_BLOCKED)
	{
		HxLOG_Error("Your PIN code is blocked. Call the operator to reactivate it.\n");
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	/* step 2 : Check PinCode with the object */
	eStatus = caRequestCreate(CA_REQUEST_TYPE_PIN_CODE_CHECK, &pCaReqCheck);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	eStatus = caRequestSetPinCode(pCaReqCheck, pMyPinCode);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSetPinCode (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		caRequestDispose(pCaReqCheck);
		return ERR_FAIL;
	}

	eStatus = caRequestSetCurrentPinCode(pCaReqCheck, 4, aucPinCode);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSetCurrentPinCode (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		caRequestDispose(pCaReqCheck);
		return ERR_FAIL;
	}

	eStatus = caRequestSend(pCaReqCheck);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		caRequestDispose(pCaReqCheck);
		return ERR_FAIL;
	}

	eStatus = caRequestGetProcessingStatus(pCaReqCheck, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pCaReqInfo);
		caRequestDispose(pCaReqCheck);
		return ERR_FAIL;
	}

	caRequestDispose(pCaReqInfo);
	caRequestDispose(pCaReqCheck);
	*xStatus = CA_PIN_CODE_PIN_ACCEPTED;
	return ERR_OK;
}


static HERROR _naMgr_cakMgr_ChangePinCode(TCaPinCodeId xPinCodeId, TPinCode xOldPinCode, TPinCode xNewPinCode, TPinCodeStatus *xStatus)
{
	TCaRequestStatus 		eStatus;
	TCaObjectStatus 		eObjStatus;
	TCaProcessingStatus	eProcStatus;
	TCaRequest			*pCaReqInfo = NULL;
	TCaRequest			*pCaReqChang = NULL;

	TCaPinCodeId			xPinId;
	TPinCodeFlags			xPinFlags;
	TFlagsMask 			xFlagsMask;
	TCaPinCode			**ppCaPinArray;
	TCaPinCode			*pMyPinCode = NULL;

	TUnsignedInt32 		i, ulNumOfPins = 0;
	TUnsignedInt8 		aucOldPinCode[4], aucNewPinCode[4];

	if (xStatus == NULL)
		return ERR_FAIL;

	HxLOG_Print("xPinCodeId[%d]\n", xPinCodeId);
	HxLOG_Print("Old PinCode : %d, New PinCode : %d\n", (int)xOldPinCode, (int)xNewPinCode);

	//	Please check PIN# as follows.
	//	(Pincode: 1234) 0x 00 00 00 00 00 00 04 D2
	//	(1234=0x4D2)

	HxSTD_memset(aucOldPinCode, 0x00, 4);
	HxSTD_memcpy(aucOldPinCode,&xOldPinCode, 4);

	aucOldPinCode[2] = aucOldPinCode[1];
	aucOldPinCode[3] = aucOldPinCode[0];
	aucOldPinCode[0] = 0;
	aucOldPinCode[1] = 0;

	HxSTD_memset(aucNewPinCode, 0x00, 4);
	HxSTD_memcpy(aucNewPinCode,&xNewPinCode, 4);

	aucNewPinCode[2] = aucNewPinCode[1];
	aucNewPinCode[3] = aucNewPinCode[0];
	aucNewPinCode[0] = 0;
	aucNewPinCode[1] = 0;

	HxLOG_Print("OldPinCode : 0x%02x%02x%02x%02x\n", aucOldPinCode[0], aucOldPinCode[1], aucOldPinCode[2], aucOldPinCode[3]);
	HxLOG_Print("NewPinCode : 0x%02x%02x%02x%02x\n", aucNewPinCode[0], aucNewPinCode[1], aucNewPinCode[2], aucNewPinCode[3]);

	*xStatus = CA_PIN_CODE_ERROR;
	/* step 1 : Get PinCode object */
	eStatus = caRequestCreate(CA_REQUEST_TYPE_PIN_CODES_INFO, &pCaReqInfo);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return ERR_FAIL;
	}

	eStatus = caRequestSend(pCaReqInfo);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	eStatus = caRequestGetProcessingStatus(pCaReqInfo, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	eStatus = caRequestGetObjects(pCaReqInfo, &ulNumOfPins, (void***)&ppCaPinArray);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (ulNumOfPins <= 0) )
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfPins);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	HxLOG_Print("Num Of Pins = %d\n", ulNumOfPins);

	for (i=0; i<ulNumOfPins; i++)
	{
		eObjStatus = caPinCodeGetId(ppCaPinArray[i], &xPinId);
		if (eObjStatus != CA_OBJECT_NO_ERROR)
		{
			break;
		}

		if (xPinId == xPinCodeId)
		{
			pMyPinCode = ppCaPinArray[i];
			break;
		}
	}

	if ( (eObjStatus != CA_OBJECT_NO_ERROR) || (pMyPinCode == NULL) )
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfPins);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	HxLOG_Print("CaPinCode Object = 0x%x\n", pMyPinCode);

	eObjStatus = caPinCodeGetFlags(pMyPinCode, &xPinFlags);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caPinCodeGetFlags (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	eObjStatus = caPinCodeGetFlagsMask(pMyPinCode, &xFlagsMask);
	if (eObjStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Error("error in caPinCodeGetFlagsMask (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	if (xFlagsMask & xPinFlags & CA_PIN_CODE_SUSPENDED)
	{
		HxLOG_Error("Your PIN code is suspended. Call the operator to reactivate it\n");
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	if (xFlagsMask & xPinFlags & CA_PIN_CODE_BLOCKED)
	{
		HxLOG_Error("Your PIN code is blocked. Call the operator to reactivate it.\n");
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	/* step 2 : Change PinCode with the object */
	eStatus = caRequestCreate(CA_REQUEST_TYPE_PIN_CODE_CHANGE, &pCaReqChang);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		return ERR_FAIL;
	}

	eStatus = caRequestSetPinCode(pCaReqChang, pMyPinCode);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSetPinCode (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		caRequestDispose(pCaReqChang);
		return ERR_FAIL;
	}

	eStatus = caRequestSetCurrentPinCode(pCaReqChang, 4, aucOldPinCode);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSetCurrentPinCode (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		caRequestDispose(pCaReqChang);
		return ERR_FAIL;
	}

	eStatus = caRequestSetNewPinCode(pCaReqChang, 4, aucNewPinCode);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSetNewPinCode (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		caRequestDispose(pCaReqChang);
		return ERR_FAIL;
	}

	eStatus = caRequestSend(pCaReqChang);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pCaReqInfo);
		caRequestDispose(pCaReqChang);
		return ERR_FAIL;
	}

	eStatus = caRequestGetProcessingStatus(pCaReqChang, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pCaReqInfo);
		caRequestDispose(pCaReqChang);
		return ERR_FAIL;
	}

	caRequestDispose(pCaReqInfo);
	caRequestDispose(pCaReqChang);
	*xStatus = CA_PIN_CODE_PIN_ACCEPTED;
	return ERR_OK;
}


static HERROR _naMgr_cakMgr_GetSyncValidityHdProduct(TRightType eType, TUnsignedInt32 ucDscLen, TUnsignedInt8 *pucDesc, TBoolean *pbExpired, TCaDuration *pulDuration)
{
	TCaRequestStatus 		eStatus;
	TCaProcessingStatus 	eProcStatus;
	TCaRequest				*pCaRequest = NULL;

	TCaEvent 				**ppCaEvtArray;
	TUnsignedInt32 			ulNumOfEvts = 0;

	if ( (pucDesc == NULL) || (pbExpired == NULL) || (pulDuration == NULL) )
	{
		HxLOG_Error("targets are null\n");
		return ERR_FAIL;
	}

	*pbExpired = TRUE;
	*pulDuration = 0;
	eStatus = caRequestCreate(CA_REQUEST_TYPE_EVENT, &pCaRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return ERR_FAIL;
	}
	eStatus = caRequestSetDescriptors(pCaRequest, ucDscLen, pucDesc);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSetDescriptors (0x%x)\n", eStatus);
		caRequestDispose(pCaRequest);
		return ERR_FAIL;
	}
	eStatus = caRequestSend(pCaRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pCaRequest);
		return ERR_FAIL;
	}
	eStatus = caRequestGetProcessingStatus(pCaRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		caRequestDispose(pCaRequest);
		return ERR_FAIL;
	}
	eStatus = caRequestGetObjects(pCaRequest, &ulNumOfEvts, (void***)&ppCaEvtArray);
	if ( (eStatus == CA_REQUEST_NO_ERROR) && (ulNumOfEvts > 0) )
	{
		TCaObjectStatus eObjStatus;
		if (eType == ePre_Paid)
		{
			TCaProduct 			**ppxProductArray;
			TCaDuration			ulTmpDur;
			TUnsignedInt32 		i, pNumOfProducts = 0;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			TCaAccess pxAccess;
#endif
			eObjStatus = caEventGetRelatedProducts((const TCaEvent*)ppCaEvtArray[0], (TUnsignedInt32*)&pNumOfProducts, (const TCaProduct***)&ppxProductArray);
			if ( (eObjStatus == CA_OBJECT_NO_ERROR) && (pNumOfProducts > 0) )
			{
				for (i=0; i<pNumOfProducts; i++)
				{
					eObjStatus = caProductGetRemainingRentalTime((const TCaProduct*)ppxProductArray[i], &ulTmpDur);
					if (eObjStatus == CA_OBJECT_NO_ERROR)
					{
						// if (*pulDuration < ulTmpDur)	// get longest duarion
						{
							*pulDuration = ulTmpDur;
							HxLOG_Error("ulTmpDur (%ld)\n", ulTmpDur);
							s_ulProductDuration = ulTmpDur;
						}
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
						eObjStatus = caEventGetAccess((const TCaEvent*)ppCaEvtArray[0], &pxAccess);
						if (eObjStatus == CA_OBJECT_NO_ERROR)
						{
							s_ulAccessStatus = pxAccess;
							HxLOG_Print("pxAccess (%d)\n", pxAccess);
						}
#endif
					}
					else
					{
						HxLOG_Error("error in caProductGetRemainingRentalTime (%d)(0x%x)\n", (int)i, eObjStatus);
					}
				}
				*pbExpired = FALSE;
				caRequestDispose(pCaRequest);
				return ERR_OK;
			}
			else
			{
				HxLOG_Error("error in caEventGetRelatedProducts (0x%x, %d)\n", eObjStatus, (int)pNumOfProducts);
			}
		}
		else
		{
			TCaAccess pxAccess;
			eObjStatus = caEventGetAccess((const TCaEvent*)ppCaEvtArray[0], &pxAccess);
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			s_ulAccessStatus = pxAccess;
			HxLOG_Print("pxAccess (%d)\n", pxAccess);
#endif
			if (eObjStatus == CA_OBJECT_NO_ERROR)
			{
				if ( (pxAccess == CA_ACCESS_CLEAR) || (pxAccess == CA_ACCESS_FREE) || (pxAccess == CA_ACCESS_GRANTED) )
				{
					*pbExpired = FALSE;
				}
				caRequestDispose(pCaRequest);
				return ERR_OK;
			}
		}
	}
	else
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfEvts);
	}

	caRequestDispose(pCaRequest);
	return ERR_FAIL;
}


static TCaStatus _naMgr_cakMgr_InitValidityHdProduct(void)
{
	TSignedInt32 nErrCode;

	nErrCode = VK_CreateMessageQueue(32, (unsigned long*)&s_ulValidityHdQueueId, NAME_VALIDITY_HD_QUEUE, QUEUE_SIZE_2);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("error in creating s_ulValidityHdQueueId 0x%x\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
static void _naMgr_cakMgr_RequestSyncSetParentalControl(void)
{
	TCaRequestStatus			eStatus;
	TCaRequest					*pxRequest;
	TCaProcessingStatus			eProcStatus;
	TCaParentalControlThreshold	xThreshold;
	TCaParentalControlBitmap	xBitmap = 0;
	DxRatingAge_e				eParentalSetup = eDxRATING_AGE_LOCK_ALL;

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	if(DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING, (HUINT32 *)&eParentalSetup, 0) != ERR_OK)
		eParentalSetup = eDxRATING_AGE_LOCK_ALL;

	if(eParentalSetup == eDxRATING_VIEW_ALL)
	{
		xThreshold = ALWAYS_CA_PARENTAL_CONTROL_OFF;
	}
	else if(eParentalSetup == eDxRATING_AGE_LOCK_ALL)
	{
		xThreshold = ALWAYS_CA_PARENTAL_CONTROL_ON;
	}
	else
	{
		/*	AGE_18이 15인데 NA SYSTEM상 ALWAYS OFF 이다.
			참고로 STB > H/E or STB >= H/E 경우일 때 DESCRAMBLE 동작은 OP에 따라 달라진다. */
	#if defined(CONFIG_MW_CAS_NAGRA_OP_ERT)	// CONAX와 Simulcrypt로 제공되며, STB > H/E 경우임
		if(eParentalSetup >= eDxRATING_AGE_17)
			xThreshold = eDxRATING_AGE_16;
		else if(eParentalSetup == eDxRATING_AGE_4)
			xThreshold = eParentalSetup;// 무의미하다.
		else
			xThreshold = eParentalSetup - 1;
	#else
		if(eParentalSetup >= eDxRATING_AGE_17)
			xThreshold = eDxRATING_AGE_17;
		else
			xThreshold = eParentalSetup;
	#endif
	}

	eStatus = caRequestCreate(CA_REQUEST_TYPE_SET_PARENTAL_CONTROL, &pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return;
	}

	eStatus = caRequestSetParentalControl(pxRequest, xThreshold, xBitmap);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		goto _caRequestDispose;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		goto _caRequestDispose;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || ( eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		goto _caRequestDispose;
	}

	HxLOG_Print("[%s:%d] Done...\n", __FUNCTION__, __HxLINE__);

_caRequestDispose:
	caRequestDispose(pxRequest);
}
#endif


/**************************************************************************************************/
#define ________CA_CAK_MGR_Public_Functions_Body________________________________________________
/**************************************************************************************************/
HERROR CA_NA_CakMgrInitialization(void)
{
	TSignedInt32 nErrCode;

	_naMgr_cakMgr_InitGlobalInfo();

	nErrCode = VK_CreateMessageQueue(256, (unsigned long*)&s_ulCakQueueId, NA_CAK_MSG_Q_NAME, QUEUE_SIZE_1);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("error in VK_CreateMessageQueue (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	nErrCode = _naMgr_cakMgr_InitValidityHdProduct();
	if (nErrCode != ERR_OK)
	{
	 	HxLOG_Error("error in local_caCak_InitValidityHdProduct (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}


/**************************************************************************************************/
#define ________CA_CAK_MGR_External_Functions_Body____________________________________________
/**************************************************************************************************/

HERROR CA_NA_CakMgrStartUp(void)
{
	return _naMgr_cakMgr_NagraStartup();
}


HERROR CA_NA_CakRegisterAlarmExportation(TAccessAlarmCallback callback)
{
	s_pfnAlarmCallBack = callback;
	return ERR_OK;
}


HERROR CA_NA_CakRegisterSystemInfoCallback(TSystemInfoCallback callback)
{
	s_pfnSysInfoCallBack = callback;
	return ERR_OK;
}


HERROR CA_NA_CakRegisterLocInfoCallback(TLocationInfoCallback callback)
{
	s_pfnLocInfoCallBack = callback;
	return ERR_OK;
}


HERROR CA_NA_CakRegisterScAlarmCallback(TSmartcardAlarmCallback callback)
{
	s_pfnScAlarmCallback = callback;
	return ERR_OK;
}

HERROR CA_NA_CakRegisterCaPopUpProc(TCaPopupCallback callback)
{
	s_pfnCaPopupCallback = callback;
	return ERR_OK;
}

HERROR CA_NA_CakRequest(TUnsignedInt32 ulReqId)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 		nErrCode;

	stMsg.wMsg 		= NA_APP_ID;
	stMsg.lParam1 	= (TUnsignedInt32)ulReqId;
	stMsg.lParam2 	= 0;
	stMsg.lParam3 	= 0;

	nErrCode = VK_PostMessage(s_ulCakQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Error("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
HERROR CA_NA_CopRegisterFunc(TCopInfoCallback pfnCopCallback)
{
	if (pfnCopCallback == NULL)
	{
		HxLOG_Error("Targets are null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("(%p)\n", pfnCopCallback);

	s_pfnCopCallback = pfnCopCallback;
	return ERR_OK;
}

HERROR CA_NA_CopUnregisterFunc(void)
{
	s_pfnCopCallback = NULL;
	return ERR_OK;
}

HERROR CA_NA_CopReceivedData(TTransportSessionId xTransportSessionId,  /*TCopyProtectionData*/TUnsignedInt8 xCopyProtectionData[2])
{
	HxLOG_Print("Action (0x%x), CopData[%x, %x]\n", (unsigned int)xTransportSessionId, xCopyProtectionData[0], xCopyProtectionData[1]);

	if (s_pfnCopCallback)
	{
		(*s_pfnCopCallback)(xTransportSessionId, PID_NULL, xCopyProtectionData);
		return ERR_OK;
	}
	return ERR_FAIL;
}
#endif


HERROR CA_NA_CakGetSystemInfo(TSystemInfoStatus* xStatus, TIrdInfo** pxIrdInfo, TUnsignedInt8* xNumberOfSmartcards, TSmartcardInfoItem** pxSmartcardInfoArray)
{
	if (xStatus)
		*xStatus = s_eSysInfoStatus;

	if (pxIrdInfo)
	{
		*pxIrdInfo = &s_stSysIrdInfo;
	}

	if (xNumberOfSmartcards)
		*xNumberOfSmartcards = s_usSysScInfoNb;

	if (pxSmartcardInfoArray)
		*pxSmartcardInfoArray = s_stSysScInfo;

	return ERR_OK;
}


HERROR CA_NA_CakGetChipsetInfo(NaChipsetInfo_t** pxChipsetInfo)
{
	if (pxChipsetInfo)
		*pxChipsetInfo = &s_stChipsetInfo;

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
HERROR CA_NA_GetCscMaxIndex(TUnsignedInt8* pxCscMaxIndex)
{
	if (pxCscMaxIndex)
		*pxCscMaxIndex = s_ucCscMaxIndex;

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
HERROR CA_NA_GetNumberOfIcc(TUnsignedInt8* pxNumberOfIcc)
{
	if (pxNumberOfIcc)
		*pxNumberOfIcc = s_xMaxNumberOfIcc;

	return ERR_OK;
}
#endif

HERROR CA_NA_CakSameCasIdInSc(TUnsignedInt8 ucSlotId, TUnsignedInt16 usCaSysId)
{
	if (ucSlotId >= NO_SMC_SLOT)
	{
		HxLOG_Error("over of slot (%d)\n", ucSlotId);
		return ERR_FAIL;
	}

	if (s_usSysScInfoNb == 0)
	{
		HxLOG_Error("Not inserted s/c");
		return ERR_FAIL;
	}

	HxLOG_Print("Slot(%d), Ecm CaSysID(0x%x)\n", ucSlotId, s_stSysScInfo[ucSlotId].ecmCaSystemId);

	if (s_stSysScInfo[ucSlotId].ecmCaSystemId == (TCaSystemId)usCaSysId)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}


HERROR CA_NA_CakGetZipCode(TUnsignedInt8 ucSlotId, TUnsignedInt32 * pulZipCode)
{
	if(pulZipCode)
	{
		*pulZipCode = s_ulZipCodeInfo[ucSlotId];
	}

	return ERR_OK;
}


HERROR CA_NA_GetValidityHdProduct(TRightType eType, TUnsignedInt32 ucDscLen, TUnsignedInt8 *pucDesc, TBoolean *pbExpired, TCaDuration *ulDuration)
{
	return _naMgr_cakMgr_GetSyncValidityHdProduct(eType, ucDscLen, pucDesc, pbExpired, ulDuration);
}


HERROR CA_NA_GetScExpirationDate(TUnsignedInt8 ucSlotId, TBoolean *pbEndOfTime, TCalendarTime *pstDataTime)
{
	if ( (pbEndOfTime == NULL) || (pstDataTime == NULL) )
		return ERR_FAIL;

	if (ucSlotId >= NO_SMC_SLOT)
		return ERR_FAIL;

	*pbEndOfTime = FALSE;

	if (s_stScExpireInfo[ucSlotId].bGotScExpireDate == TRUE)
	{
		if (s_stScExpireInfo[ucSlotId].bEndOfTime)
		{
			*pbEndOfTime = TRUE;
		}
		else
		{
			HxSTD_memcpy(pstDataTime, &s_stScExpireInfo[ucSlotId].stScExpireDate, sizeof(TCalendarTime));
		}
		return ERR_OK;
	}
	return ERR_FAIL;
}


HERROR CA_NA_GetScExpirationDuration(TCaDuration *expireDuraion)
{
		if(expireDuraion == NULL)
		{
			HxLOG_Error("FAIL !!!!\n");
			return ERR_FAIL;
		}
		*expireDuraion = s_ulProductDuration;
		return ERR_OK;
}
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR CA_NA_CakGetAccessStatus(TCaAccess *accessStatus)
{

		if(accessStatus == NULL)
		{
			HxLOG_Error("accessStatus can not received.\n");
			return ERR_FAIL;
		}
		*accessStatus = s_ulAccessStatus;
		return ERR_OK;
}
#endif

HERROR CA_NA_CakGetLocInfo(TLocationInfoStatus* xStatus, TUnsignedInt8* xNumberOfSmartcards, TLocationInfoItem** pxLocationInfoArray)
{
	if (xStatus)
		*xStatus = s_eLocInfoStatus;

	if (xNumberOfSmartcards)
		*xNumberOfSmartcards = s_usLocaInfoNb;

	if (pxLocationInfoArray)
		*pxLocationInfoArray = s_stLocInfo;

	return ERR_OK;
}


HERROR CA_NA_CakCheckPincode(TPinIndex xPinIndex, TPinCode xPinCode, TUnsignedInt8 *pucPinCode, TPinCodeStatus *xStatus)
{
	return _naMgr_cakMgr_CheckPinCode((TCaPinCodeId)xPinIndex, xPinCode, xStatus);
}


HERROR CA_NA_CakChangePincode(TPinIndex xPinIndex, TPinCode xOldPinCode, TPinCode xNewPinCode, TUnsignedInt8 *pucOldPinCode, TUnsignedInt8 *pucNewPinCode, TPinCodeStatus *xStatus)
{
	return _naMgr_cakMgr_ChangePinCode((TCaPinCodeId)xPinIndex, xOldPinCode, xNewPinCode, xStatus);
}


#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
HERROR CA_NA_Cak7_EnablingConsumptionForced(TBoolean bOneShot)
{
	_naMgr_cakMgr_EnablingConsumptionForced(bOneShot);
	return ERR_OK;
}
#endif

HERROR CA_NA_CakGetProviderId(TUnsignedInt8 ucSlotId, TPpid *pProviderId)
{
	if (ucSlotId > NO_SMC_SLOT)
	{
		HxLOG_Error("over of slot (%d)\n", ucSlotId);
		return ERR_FAIL;
	}

	if (s_usSysScInfoNb == 0)
	{
		HxLOG_Error("Not inserted s/c");
		return ERR_FAIL;
	}

	// CONFIG_MW_CAS_NUM_SC_SLOT = 1 경우에만 여기로 내려온다.
	// 즉, s_stSysScInfo[] 배열은 오직 한개뿐이다.
	ucSlotId = 0;
	HxLOG_Print("Slot(%d), provider(0x%x)\n", ucSlotId, s_stSysScInfo[ucSlotId].provider);

	*pProviderId = s_stSysScInfo[ucSlotId].provider;

	return ERR_OK;
}


HERROR CA_NA_CakGetEcmCaSystemId(TUnsignedInt8 ucSlotId, TUnsignedInt16 *usCaSysId)
{
	if (ucSlotId > NO_SMC_SLOT)
	{
		HxLOG_Error("over of slot (%d)\n", ucSlotId);
		return ERR_FAIL;
	}

	if (s_usSysScInfoNb == 0)
	{
		HxLOG_Error("Not inserted s/c\n");
		return ERR_FAIL;
	}

	// CONFIG_MW_CAS_NUM_SC_SLOT = 1 경우에만 여기로 내려온다.
	// 즉, s_stSysScInfo[] 배열은 오직 한개뿐이다.
	ucSlotId = 0;
	HxLOG_Print("Slot(%d), Ecm CaSysID(0x%x)\n", ucSlotId, s_stSysScInfo[ucSlotId].ecmCaSystemId);

	*usCaSysId = s_stSysScInfo[ucSlotId].ecmCaSystemId;

	return ERR_OK;
}


HERROR CA_NA_CakResetSysInfoTigger(void)
{
	s_bOccuredTriggerOfSysInfo = (TBoolean)FALSE;
	return ERR_OK;
}


HBOOL CA_NA_CakIsOccuredSysInfoTigger(void)
{
	return (HBOOL)s_bOccuredTriggerOfSysInfo;
}


HERROR CA_NA_CakResetSmarcardInfoTigger(void)
{
	s_bOccuredTriggerOfScInfo = (TBoolean)FALSE;
	return ERR_OK;
}


HBOOL CA_NA_CakIsOccuredSmarcardInfoTigger(void)
{
	return (HBOOL)s_bOccuredTriggerOfScInfo;
}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
HERROR CA_NA_GetAccessErrorCode(TUnsignedInt32 ulCurrentSessionId, TUnsignedInt32* pAccessErrorCode)
{
	TCaRequestStatus 		eStatus;
	TCaObjectStatus 		eObjStatus;
	TCaProcessingStatus	eProcStatus;

	TCaRequest 			*pxRequest = NULL;
	TCaProgram 			**ppCaPgmArray = NULL;
	TUnsignedInt32 		i, ulNumOfPgms = 0;
	TCaAccess			eCaAccess;
	TTransportSessionId	ulSessionId;
	TUnsignedInt32		uAccessErrorCode = 0;

	eStatus = caRequestCreate(CA_REQUEST_TYPE_PROGRAMS, &pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestCreate (0x%x)\n", eStatus);
		return ERR_FAIL;
	}

	eStatus = caRequestSend(pxRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Error("error in caRequestSend (0x%x)\n", eStatus);
		caRequestDispose(pxRequest);
		return ERR_FAIL;
	}

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || ( eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Error("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		caRequestDispose(pxRequest);
		return ERR_FAIL;
	}

	eStatus = caRequestGetObjects(pxRequest, &ulNumOfPgms, (void***)&ppCaPgmArray);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || (ulNumOfPgms <= 0) )
	{
		HxLOG_Error("error in caRequestGetObjects (0x%x, %d)\n", eStatus, (int)ulNumOfPgms);
		caRequestDispose(pxRequest);
		return ERR_FAIL;
	}

	HxLOG_Print("Program number (%d)\n", (int)ulNumOfPgms);

	for (i=0; i<ulNumOfPgms; i++)
	{
		eObjStatus = caProgramGetTransportSessionId((const TCaProgram*)ppCaPgmArray[i], &ulSessionId);
		if (eObjStatus != CA_OBJECT_NO_ERROR)
		{
			HxLOG_Error("error in caProgramGetTransportSessionId (0x%x)\n", eObjStatus);
		}

		HxLOG_Print("Transport_session (0x%x)\n", (unsigned int)ulSessionId);
		if(ulCurrentSessionId == ulSessionId)
		{
			eObjStatus = caProgramGetAccess((const TCaProgram*)ppCaPgmArray[i], &eCaAccess);
			if (eObjStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("error in caProgramGetAccess (0x%x)\n", eObjStatus);
			}

			HxLOG_Print("Access (0x%x)\n", eCaAccess);
			
			eObjStatus = caProgramGetAccessErrorCode((const TCaProgram*)ppCaPgmArray[i], &uAccessErrorCode);
			if (eObjStatus != CA_OBJECT_NO_ERROR)
			{
				HxLOG_Error("error in caProgramGetAccessErrorCode (0x%x)\n", eObjStatus);
			}
			*pAccessErrorCode = uAccessErrorCode;
			caRequestDispose(pxRequest);
			return ERR_OK;
		}
	}

	caRequestDispose(pxRequest);
	return ERR_FAIL;
}
#endif


HERROR NAMGR_CAKMGR_SendPopupMessage(NaPopupUI_t *pstPopupUi)
{
	NaCasMsg_t	stMsg;

	HxSTD_memset(&stMsg, 0, sizeof(NaCasMsg_t));
	stMsg.wMsg		= NA_CAK_ID;
	stMsg.lParam2	= CAK_CA_POPUP_COMMAND;
	stMsg.lParam3	= (TUnsignedInt32)pstPopupUi;

	if(VK_PostMessage(s_ulCakQueueId, &stMsg, sizeof(NaCasMsg_t)) != VK_OK)
	{
		HxLOG_Error("error in VK_PostMessage\n");

		if(pstPopupUi != NULL)
		{
			if(pstPopupUi->pucMsg != NULL)
			{
				OxCAS_Free(pstPopupUi->pucMsg);
			}

			OxCAS_Free(pstPopupUi);	// 호출하는 곳에서 pstPopupUi MemAlloc하여 call한다.
		}
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
HERROR NAMGR_CAKMGR_SetParentalControlLevel(void)
{
	return CA_NA_CakRequest(CAK_PARENTAL_CONTROL_REQ_ID);
}
#endif

/**************************************************************************************************/
#define ________CA_CAK_MGR_DEBUG_Functions____________________________________________
/**************************************************************************************************/
#if defined(CONFIG_DEBUG)
// for_test
TUnsignedInt8 emmwakeup1[] = { 0x64, 0x17, 0x00, 0x00, 0x01, 0x27, 0xD9, 0x01,	// header		// emm wakeup 0xD9 - current channel 로 튜닝
					      0x00, 			// reserved
					      0x00, 			// reserved 5, network_flag, ts_flag, svc_flag
					      0x00, 0x06, 	// waking_duration (6min)
					      0x001,  		// reserved
					      0x00, 0x04, 	// sleeping_duration (4min)
					      0x10 }; //Enable EMM Wakeup Management

TUnsignedInt8 emmwakeup2[] = { 0x64, 0x17, 0x00, 0x00, 0x01, 0x27, 0xD9, 0x01,	// header			// emm wakeup 0xD9닝 - tripelID 채널로 튜닝
					      0x00,  			// reserved
					      0x07,  			// reserved 5, network_flag, ts_flag, svc_flag
					      0x00, 0x85, 		// network_id
					      0x00, 0x03, 		// ts_id
					      0x00, 0x11,			// svc_id
					      0x00, 0x05, 		//waking_duration (5min)
					      0x00, 				// reserved
					      0x00, 0x0a, 		// sleeping_duration (10min)
					      0x10 }; //Enable EMM Wakeup Management
					      
TUnsignedInt8 emmwakeup3[] = { 0x64, 0x17, 0x00, 0x00, 0x01, 0x26, 0xD9, 0x01, 0x02, 0x00, 0x00, 0x05, 0x07, 0x00, 0xC8, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0A, 0x01, 0x00, 0x2D, 0x10 }; //Enable EMM Wakeup Management
TUnsignedInt8 emmwakeup4[] = { 0x64, 0x07, 0x00, 0x00, 0x01, 0x25, 0xD9, 0x00, 0x27 }; // Disable EMM Wakeup Management
TUnsignedInt8 forceStandby[] = { 0x64, 0x07, 0x12, 0x34, 0x56, 0x78, 0xC9, 0x01, 0x36 }; // force standby
TUnsignedInt8 PushVod1[] = { 0x64, 0x0F, 0x00, 0x00, 0x00, 0x07, 0xD1, 0x04, 0x02, 0x94, 0x04, 0x74, 0x04, 0xEC, 0x01, 0x2C, 0x00 };
TUnsignedInt8 PushVod2[] = { 0x64, 0x0F, 0x00, 0x00, 0x00, 0x07, 0xD1, 0x04,	0x02, 0x94, 0x04, 0x74, 0x04, 0x7C, 0x01, 0x4C, 0x50 };
TUnsignedInt8 PushVod3[] = { 0x64, 0x17, 0x00, 0x00, 0x00, 0x07, 0xD1, 0x04,	0x08, 0x78, 0x09, 0x2C, 0x18, 0x3C, 0x19, 0x2C, 0x1D, 0x46, 0x28, 0xB4, 0x01, 0x68, 0x01, 0xE0, 0x54 };
TUnsignedInt8 PushVod4[] = { 0x64, 0x17, 0x00, 0x00, 0x00, 0x07, 0xD1, 0x04,	0x08, 0x78, 0x09, 0x2C, 0x18, 0x3C, 0x19, 0x2C, 0x1D, 0x46, 0x28, 0xB4, 0x01, 0x68, 0x00, 0x3C, 0x54 };
TUnsignedInt8 PushVod5[] = { 0x64, 0x21, 0x00, 0x00, 0x00, 0x05, 0xD1, 0x03, 0x41, 0x2f, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x32, 0x46, 0x2f, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x32, 0x46, 0x2e, 0x70, 0x6d, 0x74, 0x2d, 0x30, 0x31, 0x4C };// Erase Meta file
TUnsignedInt8 PushVod6[] = { 0x64, 0x0B, 0x12, 0x34, 0x56, 0x78, 0xD1, 0x02, 0x30, 0x32, 0x32, 0x46, 0x53 }; // Erase Asset
TUnsignedInt8 ZoneId1[] = {0x64, 0x0D, 0x00, 0x02, 0x20, 0x17, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x25};
TUnsignedInt8 CountryCode1[] = {0x64, 0x09, 0x00, 0x00, 0x01, 0x05, 0xD8, 0x03, 0x53, 0x4C, 0x56, 0x30};	// SLV    	[EL SALVADOR]
TUnsignedInt8 CountryCode2[] = {0x64, 0x09, 0x00, 0x00, 0x01, 0x07, 0xD8, 0x03, 0x47, 0x54, 0x4D, 0x3D};	//GTM 	[GUATEMALA]
TUnsignedInt8 ResetPin[] = {0x64, 0x07, 0x00, 0x00, 0x00, 0x07, 0x12, 0x01, 0xED};
TUnsignedInt8 FactorySet[] = {0x64, 0x07, 0x00, 0x00, 0x00, 0x09, 0xCC, 0x01, 0x33};
TUnsignedInt8 aucIrdRemovePopup[] = {0x64, 0x07, 0x00, 0x02, 0x20, 0x1f, 0xcf, 0x01, 0x30};
TUnsignedInt8 aucIrdMail[] = {0x64, 0x0e, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x01, 0x00, 0x41, 0x80, 0x98, 0x82, 0x83, 0x64, 0x7d};

HERROR CA_NA_CakTestIrdPupupCmd()
{
	TUnsignedInt8 *aucData = NULL;
	TUnsignedInt8 ucLength = 22;
	int i =0;
	TUnsignedInt8 aucIrdPopup[] = {0x64, 0x14, 0x00, 0x00, 0x00, 0x00, 0xcf, 0x00, 0x01, 0x3f, 0x00, 0x61, 0x61, 0x61, 0x61, 0x61, 
									0x61, 0x61, 0x61, 0x61, 0x61, 0x27};

	for(i = 0 ; i < 64 ; i++)
	{
		aucIrdPopup[5] = i;
		aucIrdPopup[10] = i;
		if((0x27 - i) >= 0)
			aucIrdPopup[21] = (0x27 - i);
		else
			aucIrdPopup[21] = 0xFF - (i - 0x28);
		_naMgr_cakMgr_ExecIrdCmdExportation((TSize)ucLength, aucIrdPopup);
		VK_TASK_Sleep(400);
	}
	
	return ERR_OK;
}

HERROR CA_NA_CakTestIrdCmd(HUINT32 nCmd)
{
	TUnsignedInt8 *aucSrc = NULL;
	TUnsignedInt8 *aucData = NULL;
	TUnsignedInt8 ucLength = 0;

	switch(nCmd)
	{
		case 0:
			// reset IRD cmd
			CA_NA_IRDCMD_ResetIRDCmdData();
			break;

		case 1:
			//set Zone ID
			aucSrc = ZoneId1;
			ucLength = sizeof(ZoneId1);
			break;
			
		case 2:
			//set CountryCode 1
			aucSrc = CountryCode1;
			ucLength = sizeof(CountryCode1);
			break;
			
		case 3:
			//set CountryCode 2
			aucSrc = CountryCode2;
			ucLength = sizeof(CountryCode2);
			break;

		case 4:
			//reset PIN
			aucSrc = ResetPin;
			ucLength = sizeof(ResetPin);
			break;

		case 5:
			//factory set
			aucSrc = FactorySet;
			ucLength = sizeof(FactorySet);
			break;
			
		case 6:
			// show IRD Popup
			CA_NA_CakTestIrdPupupCmd();
			return ERR_OK;
			
		case 7:
			// remove IRD Popup
			aucSrc = aucIrdRemovePopup;
			ucLength = sizeof(aucIrdRemovePopup);
			break;

		case 8:
			// MAIL
			aucSrc = aucIrdMail;
			ucLength = sizeof(aucIrdMail);
			break;
			
		case 10:
			// emmwakeup
			aucSrc = emmwakeup1;
			ucLength = sizeof(emmwakeup1);
			break;
		default:
			break;
	}
	aucData = (TUnsignedInt8 *)OxCAS_Malloc(ucLength);
	if (aucData == NULL)
		return ERR_FAIL;

	HxSTD_memcpy(aucData, aucSrc, ucLength);
	_naMgr_cakMgr_ExecIrdCmdExportation((TSize)ucLength, aucData);
	OxCAS_Free(aucData);
	return ERR_OK;
}
#endif

#if defined(CONFIG_DEBUG) || defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
HERROR NAMGR_CAKMGR_SendIrdCommand(HUINT8 ucIrdCmd)
{
	TUnsignedInt8	*pxIrdCmd;
	TSize			xIrdCmdLen;

	static TUnsignedInt8 s_aucIrdMail[] =
	{
		0x64, 0x67, 0x00, 0x01, 0xf0, 0xc0, 0xc0, 0x01, 0x00, 0x41, 0x00, 0x10, 0x00, 0x05, 0x48, 0x65,
		0x6c, 0x6c, 0x6f, 0x21, 0x20, 0x49, 0x52, 0x44, 0x20, 0x43, 0x6f, 0x6d, 0x6d, 0x61, 0x6e, 0x64,
		0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x28, 0x4d, 0x41, 0x49, 0x4c, 0x29, 0x2e, 0x20, 0xa0, 0xa1,
		0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1,
		0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1,
		0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1,
		0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0x00, 0x78
	};

	switch(ucIrdCmd)
	{
		case 0x64:
			pxIrdCmd = s_aucIrdMail;
			xIrdCmdLen = sizeof(s_aucIrdMail);
			break;

		default:
			return ERR_FAIL;
	}

	_naMgr_cakMgr_ExecIrdCmdExportation(xIrdCmdLen, pxIrdCmd);

	return ERR_OK;
}

static void _naMgr_cakMgr_caLogEnable(void)
{
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	caLogEnable();

	caLogSetRole("EMM", "INT");
	caLogSetRole("PROCESS_EMM", "INT");

	caLogSetRole("ECM", "INT");
	caLogSetRole("PROCESS_ECM", "INT");

	caLogSetRole("IRD_COMMAND", "DEV");
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	// debug 용 릴리즈에서 사용
	caLogSetRole("IRD_COMMAND","DEV");
	caLogSetRole("PMT","dev");
	caLogSetRole("API", "INT");

	// 필요시 열어서 사용
//	caLogSetRole("EMM", "DEV");
//	caLogSetRole("PMEM", "DEV");
//	caLogSetRole("ECM", "DEV");
//	caLogSetRole("DAL_DMX", "DEV");

	/* CAK TEST위해 LOG 출력하도록 setting함 */
	//caDebugControlRole(MOD_ALL, VERBOSITY_HIGH, CLASS_ALL, DEBUG_ROLE_IRD_COMMAND);
	//caDebugControlRole(MOD_CAK, VERBOSITY_LOW, CLASS_GENERIC, DEBUG_ROLE_API);
//	caDebugControlRole(MOD_CAK, VERBOSITY_HIGH, CLASS_ALL, DEBUG_ROLE_ALL);
#endif
}
#endif

/*** End of File ***/
