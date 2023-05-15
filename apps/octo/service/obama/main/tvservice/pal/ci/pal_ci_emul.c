/********************************************************************
*	Including Headers
********************************************************************/

#ifdef	WIN32

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include <htype.h>
#include <vkernel.h>
//#include <emu_common.h>

#include <ci.h>
#include <di_ci.h>

#if defined(CONFIG_MW_CI_PLUS)
#include <ci_plus.h>
#endif

/********************************************************************
*	Definitions (for this source file)
********************************************************************/



#define	EMUL_CI_RESET_SLOT			0x0100
#define	EMUL_CI_CAM_STATE				0x0200
#define	EMUL_CI_APP_INFO				0x0300
#define	EMUL_CI_CA_SYS_ID				0x0400
#define	EMUL_CI_CAS_STATE				0x0500
#define	EMUL_CI_MMI_ANSWER_STRING	0x0600
#define	EMUL_CI_MMI_REQ				0x0700

#define	EMUL_CI_MMI_REQ_OPEN			(EMUL_CI_MMI_REQ|0x0001)
#define	EMUL_CI_MMI_REQ_CLOSE		(EMUL_CI_MMI_REQ|0x0002)
#define	EMUL_CI_MMI_REQ_MENU			(EMUL_CI_MMI_REQ|0x0003)
#define	EMUL_CI_MMI_REQ_LIST			(EMUL_CI_MMI_REQ|0x0004)
#define	EMUL_CI_MMI_REQ_TEXT			(EMUL_CI_MMI_REQ|0x0005)
#define	EMUL_CI_MMI_REQ_ENQ			(EMUL_CI_MMI_REQ|0x0006)

#define EMUL_CI_MAX_SS				16
#define EMUL_MMI_SSID					0

#define MAX_CI_EMUL_MSG			16

/********************************************************************
*	External variables & function prototypes
********************************************************************/


/********************************************************************
* Global variables and structures
********************************************************************/


/********************************************************************
*	Global variables
********************************************************************/

HBOOL g_bMmiOpened;

/********************************************************************
*	Static variables and typedef
********************************************************************/

static CI_NOTIFY_FUNC_LIST	AppCiNotiFuncList;

static char g_pszCiAppInfo1[] = "Viaccess";
static char g_pszCiAppInfo2[] = "Iredeto";

static CI_APP_INFO _CiAppInfo[2] =
{
	{
		1,			// nType;
		0x0B00,		// usManufacturer;
		0x0B00,		// usManufacturerCode;
		"Viaccess",	// szText[MAX_CI_STR_LEN+1];
		1
	},
	{
		1,
		0x0A00,
		0x0A00,
		"Iredeto",
		1
	}
};

static HUINT16 s_CaSysId[2] =
{
	0x0B00,
	0x0A00
};

static MMI_MENU_LIST_PARAM	*pciMenuParam;
static MMI_MENU_LIST_PARAM	*pciListParam;
static HUINT8				*pciTextParam;
static MMI_ENQ_PARAM		*pciEnqParam;

static HUINT32				_testIndex = 0;

static MMI_MENU_LIST_PARAM	_ciMenuParam[] =
{
	{
		4,				//unsigned char ucNumOfItem;
		"Viaccess",			//unsigned char *pszTitle;
		"02/24/99 - V1.20.uk - 1",	//unsigned char *pszSubTitle;
		"Menu Bottom Title - 1",	//unsigned char *pszBottom;
		NULL				//unsigned char **ppszItem;
	},
	{
		12,				//unsigned char ucNumOfItem;
		"Viaccess",			//unsigned char *pszTitle;
		"02/24/99 - V1.20.uk - 2",	//unsigned char *pszSubTitle;
		"Viaccess Bottom Title - 2",	//unsigned char *pszBottom;
		NULL				//unsigned char **ppszItem;
	},
	{
		4,				//unsigned char ucNumOfItem;
		"Viaccess",			//unsigned char *pszTitle;
		"02/24/99 - V1.20.uk - 3",	//unsigned char *pszSubTitle;
		"Viaccess Bottom Title - 3",	//unsigned char *pszBottom;
		NULL				//unsigned char **ppszItem;
	},
	{
		4,				//unsigned char ucNumOfItem;
		"Viaccess",			//unsigned char *pszTitle;
		"02/24/99 - V1.20.uk - 4",	//unsigned char *pszSubTitle;
		"Viaccess Bottom Title - 4",	//unsigned char *pszBottom;
		NULL				//unsigned char **ppszItem;
	}
};

static MMI_MENU_LIST_PARAM	_ciListParam[] =
{
	{
		4,				//unsigned char ucNumOfItem;
		"About OCAM",			//unsigned char *pszTitle;
		"Open Conditional Access Module",	//unsigned char *pszSubTitle;
		"List Bottom Title - 1",//unsigned char *pszBottom;
		NULL				//unsigned char **ppszItem;
	},
	{
		12,				//unsigned char ucNumOfItem;
		"Viaccess",			//unsigned char *pszTitle;
		"02/24/99 - V1.20.uk - List2",	//unsigned char *pszSubTitle;
		"Viaccess Bottom Title - List2",//unsigned char *pszBottom;
		NULL				//unsigned char **ppszItem;
	},
	{
		4,				//unsigned char ucNumOfItem;
		"Viaccess",			//unsigned char *pszTitle;
		"02/24/99 - V1.20.uk - List3",	//unsigned char *pszSubTitle;
		"Viaccess Bottom Title - List3",//unsigned char *pszBottom;
		NULL				//unsigned char **ppszItem;
	},
	{
		4,				//unsigned char ucNumOfItem;
		"Viaccess",			//unsigned char *pszTitle;
		"02/24/99 - V1.20.uk - List4",	//unsigned char *pszSubTitle;
		"Viaccess Bottom Title - List4",//unsigned char *pszBottom;
		NULL				//unsigned char **ppszItem;
	}
};

static char szCiMenuMainTitle[]	= "Viaccess";
static char szCiMenuSubtitle[]	= "02/24/99 - V1.20.uk";
static char szCiMenuBottom[]		= "Viaccess Bottom Title";
static char *pszCiMenuItem[]		=
{
	"Consultations",
	"Prebooking",
	"Authorizations",
	"About OCAM",
	"Common Interface Item 5",
	"Common Interface Item 6",
	"Common Interface Item 7",
	"Common Interface Item 8",
	"Common Interface Item 9",
	"Common Interface Item 10",
	"Common Interface Item 11",
	"Common Interface Item 12"
};

static char *pszCiList[] =
{
	"CI List 1",
	"CI List 2",
	"CI List 3",
	"CI List 4",
	"CI List 5",
	"CI List 6",
	"CI List 7",
	"CI List 8",
	"CI List 9",
	"CI List 10",
	"CI List 11",
	"CI List 12"
};

typedef struct
{
	HBOOL bState;
	HUINT32 hMmi;
} EMUL_MMI_STATE;

static CI_CAM_STATE nSlotState[2];
static EMUL_MMI_STATE s_mmiState[2];
static int MmiHandle = 0;
static MMI_INSTANCE s_MmiInstance[EMUL_CI_MAX_SS];
static unsigned char nSlotPool[2][100];

typedef struct CI_EMUL_MSG_t
{
	unsigned int	type;
	unsigned int	nSlotId;
	unsigned int	hMmi;
	unsigned int	lParam1;
	unsigned int	lParam2;
	unsigned int	lParam3;
} CI_EMUL_MSG;

static unsigned long CiEmulMsgQId;

/********************************************************************
*	Static functions Prototypes
********************************************************************/

static void _CiEmulTask(void *param);
static void _CiEmulPostMessage(HUINT32 type, HUINT32 nSlotId, HUINT32 lParam1, HUINT32 lParam2);
static CI_SLOT_ID _MmiToSlotId(int hMmi);

static HERROR _CamState(CI_SLOT_ID nSlotId, CI_CAM_STATE nCamState);
static HERROR _AppInfo(CI_SLOT_ID nSlotId);
static HERROR _CaSysId(CI_SLOT_ID nSlotId);
static HERROR _CasState(CI_SLOT_ID nSlotId);

static HERROR _ReqMmiOpen(CI_SLOT_ID nSlotId);
static HERROR _ReqMmiClose(CI_SLOT_ID nSlotId);
static HERROR _ReqMmiMenu(CI_SLOT_ID nSlotId);
static HERROR _ReqMmiList(CI_SLOT_ID nSlotId);
static HERROR _ReqMmiText(CI_SLOT_ID nSlotId);
static HERROR _ReqMmiEnq(CI_SLOT_ID nSlotId);

/********************************************************************
*   Debugging functions
********************************************************************/

#define EMUL_CI_DEBUG

#ifdef EMUL_CI_DEBUG

static char _szCiEmulFileLine[100];
static char _szCiEmulPrintBuffer[1024];

static void	_debug_ci_emul_fileline(char *pszFile, int nLine)
{
	sprintf(_szCiEmulFileLine, "%15s-%04d:", pszFile, nLine);
}

static void	_debug_ci_emul_print(char *format, ...)
{
	va_list vl;

	va_start(vl, format);
	vsprintf(_szCiEmulPrintBuffer, format, vl);
	va_end(vl);

	HxLOG_Print("%s %s", _szCiEmulFileLine, _szCiEmulPrintBuffer);
}

#define 	CI_EMUL_Print(x) {_debug_ci_emul_fileline(__FILE__, __LINE__); _debug_ci_emul_print x;}

#else

#define 	CI_EMUL_Print(x)

#endif

//=========================================================================
//===================== CI Stack APIs for CI Emulator =====================
//=========================================================================

#define ___CI_Stack_APIs_For_Emul___ /* 포팅 필요한 CI 스택 API 들은 여기에 추가할 것 !! */

#define ___CI_Stack_APIs_From_ci_c_file___ /* from ci.h and ci.c */

HERROR xsvc_cas_CiInit(void)
{
	HINT32	i, ret;
	HUINT32	CiTaskId;

/*
	HINT32	j;
	for (i=0; i<2; i++)
		for (j=0; j<100; j++)
			nSlotPool[i][j] = 0;
*/

	for (i=0; i<2; i++)
	{
		s_mmiState[i].bState = 0;
		s_mmiState[i].hMmi = HANDLE_NULL;
	}

	ret = VK_MSG_Create( MAX_CI_EMUL_MSG, sizeof(CI_EMUL_MSG), "CiEmulMsgQ", &CiEmulMsgQId, VK_SUSPENDTYPE_FIFO);
	if(ret)
		CI_EMUL_Print(("Error::xsvc_cas_ci_init - VK_MSG_Create ret=0x%x\n", ret));

	ret = VK_TASK_Create(
		_CiEmulTask,
		1,
		1024,
		"_CiEmulTask",
		NULL,
		&CiTaskId,
		0);
	if(ret)
		CI_EMUL_Print(("Error::xsvc_cas_ci_init - VK_TASK_Create ret=0x%x\n", ret));

	VK_TASK_Start(CiTaskId);

	return 1;
}

int CI_ResetSlot(CI_SLOT_ID nSlotId)
{
	CI_EMUL_Print(("CI_ResetSlot : Slot Id = %02d\n", nSlotId));

	_CiEmulPostMessage(EMUL_CI_RESET_SLOT, nSlotId, 0, 0);

	return ERR_OK;
}

int CI_GetAppInfo(CI_SLOT_ID nSlotId, CI_APP_INFO **ppInfo)
{
	if(ppInfo == NULL)
	{
		CI_EMUL_Print(("CI_GetAppInfo---> Error: Null ppInfo pointer!!!\n"));
		return -1;
	}

	if (nSlotId==0)
	{
		*ppInfo = &_CiAppInfo[0];
	}
	else
	{
		*ppInfo = &_CiAppInfo[1];
	}

	return 0;
}

int CI_EnterMenu(CI_SLOT_ID usSlotId)
{
	_CiEmulPostMessage(EMUL_CI_MMI_REQ_OPEN, usSlotId, 0, 0);
	_CiEmulPostMessage(EMUL_CI_MMI_REQ_MENU, usSlotId, 0, 0);

	CI_EMUL_Print(("CI_EnterMenu : Slot Id = %2d\n", usSlotId));

	return ERR_OK;
}

int CI_MmiClose(MMI_HANDLE hMmi)
{
	int nSlotId = _MmiToSlotId(hMmi);

	_CiEmulPostMessage(EMUL_CI_MMI_REQ_CLOSE, nSlotId, 0, 0);

	CI_EMUL_Print(("CI_MmiClose : MMI handle = %08X\n", hMmi));

	return ERR_OK;
}

int CI_MmiAnswer(MMI_HANDLE hMmi, MMI_REQ nMmiReq, unsigned char ucChoiceNo)
{
	int nSlotId = _MmiToSlotId(hMmi);

	if (ucChoiceNo == 0)	// Anaswer Cancel
	{
		_CiEmulPostMessage(EMUL_CI_MMI_REQ_CLOSE, nSlotId, 0, 0);
	}
	else if (ucChoiceNo == 1)
	{
		_CiEmulPostMessage(EMUL_CI_MMI_REQ_MENU, nSlotId, 0, 0);
	}
	else if (ucChoiceNo == 2)
	{
		_CiEmulPostMessage(EMUL_CI_MMI_REQ_LIST, nSlotId, 0, 0);
	}
	else if (ucChoiceNo == 3)
	{
		_CiEmulPostMessage(EMUL_CI_MMI_REQ_MENU, nSlotId, 0, 0);
	}

	CI_EMUL_Print(("CI_MmiAnswer : Handle = %08X, MMI_REQ = %08X, Choice Number : %1X\n", hMmi, nMmiReq, ucChoiceNo));

	return ERR_OK;
}

int CI_MmiAnswerString(MMI_HANDLE hMmi, unsigned char *pszStr, unsigned short usStrSize)
{
	int nSlotId = _MmiToSlotId(hMmi);

	if (pszStr == NULL)	// Anaswer Cancel
	{
		_CiEmulPostMessage(EMUL_CI_MMI_REQ_CLOSE, nSlotId, 0, 0);
	}
	else
	{
		_CiEmulPostMessage(EMUL_CI_MMI_REQ_MENU, nSlotId, 0, 0);
	}

	return ERR_OK;
}

int CI_MmiGetSlotId(MMI_HANDLE hMmi, CI_SLOT_ID *pusSlotId, unsigned short	*pusSsNo)
{
	int i;

	for (i=0; i<EMUL_CI_MAX_SS; i++)
	{
		if (hMmi == &s_MmiInstance[i])
		{
			*pusSlotId = 0;
			*pusSsNo = s_MmiInstance[i].usSsNo;
		}
	}

	return ERR_OK;
}

MMI_INSTANCE * CI_MmiGetInstBySlotId(CI_SLOT_ID usSlotId)
{
	return &s_MmiInstance[usSlotId];
}

int CI_SendCaPmt(CI_SLOT_ID usSlotId, unsigned char *pucCaPmt, unsigned short usCaPmtLen)
{
	return ERR_OK;
}

int CI_AddNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList)
{
	if(notifyList.pfnNotifyCamState != NULL)
	{
		AppCiNotiFuncList.pfnNotifyCamState = notifyList.pfnNotifyCamState;
	}

	if(notifyList.pfnNotifyAppInfo != NULL)
	{
		AppCiNotiFuncList.pfnNotifyAppInfo = notifyList.pfnNotifyAppInfo;
	}

	if(notifyList.pfnNotifyCaInfo != NULL)
	{
		AppCiNotiFuncList.pfnNotifyCaInfo = notifyList.pfnNotifyCaInfo;
	}

	if(notifyList.pfnNotifyMmiReq != NULL)
	{
		AppCiNotiFuncList.pfnNotifyMmiReq = notifyList.pfnNotifyMmiReq;
	}

	if(notifyList.pfnNotifyCasState != NULL)
	{
		AppCiNotiFuncList.pfnNotifyCasState = notifyList.pfnNotifyCasState;
	}

	return ERR_OK;
}

int CI_DeleteNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList)
{
	if(notifyList.pfnNotifyCamState != NULL)
		AppCiNotiFuncList.pfnNotifyCamState = NULL;

	if(notifyList.pfnNotifyAppInfo != NULL)
		AppCiNotiFuncList.pfnNotifyAppInfo = NULL;

	if(notifyList.pfnNotifyCaInfo != NULL)
		AppCiNotiFuncList.pfnNotifyCaInfo = NULL;

	if(notifyList.pfnNotifyMmiReq != NULL)
		AppCiNotiFuncList.pfnNotifyMmiReq = NULL;

	if(notifyList.pfnNotifyCasState != NULL)
		AppCiNotiFuncList.pfnNotifyCasState = NULL;

	return ERR_OK;
}

#define ___CI_Stack_APIs_From_ci_adapt_c_file___ /* from ci_adapt.h and ci_adapt.c */

int CiAdapt_Init(void)
{
	return ERR_OK;
}

#define ___CI_Stack_APIs_From_ci_plus_c_file___ /* from ci_plus.h and ci_plus.c */

#if defined(CONFIG_MW_CI_PLUS)

int CI_RegisterAppMmiCallback (CI_AMMI_CALLBACK_T pfnCallback)
{
	return CiAmmi_RegisterCallback(pfnCallback);
}
int CI_RequestFileorData (unsigned short usSessionId, CI_AMMI_REQUEST_TYPE enReqType, unsigned char *pucReqName, unsigned long ulReqNameLen) {return ERR_OK;}
int CI_RequestAppAbort (unsigned short usSessionId, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode) {return ERR_OK;}
int CI_LoadHostLicenseConstants (CI_HOST_LICENSE_CONST *pstCiHostLicenseConst) {return ERR_OK;}
int CI_InitAuthContexts (CI_SLOT_ID usSlotId) {return ERR_OK;}
int CI_LoadAuthContexts (CI_SLOT_ID usSlotId, CI_AUTH_CTXs *pstCiAuthCtxs) {return ERR_OK;}
int CI_SaveAuthContexts (CI_SLOT_ID usSlotId, CI_AUTH_CTXs *pstCiAuthCtxs) {return ERR_OK;}
int CI_GetCurrentUtcTime (unsigned char szUtcTime[13]) {return ERR_OK;}
int CI_CompareUtcTimes (unsigned char szUtcTime1[13], unsigned char szUtcTime2[13]) {return ERR_OK;}
int CI_SetCckToDsc (CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER enKeyCipher, CI_CC_KEY_MODE enKeyMode, unsigned char *pucCck, unsigned char *pucCiv) {return ERR_OK;}
int CI_ApplyUriToExternalOutput (CI_SLOT_ID usSlotId,CIPLUS_UriInfo_t stUriInfo) {return ERR_OK;}
int CI_ApplySrmData (CI_SLOT_ID usSlotId, unsigned char *pucSrmData, unsigned short usSrmDataLen) {return ERR_OK;}
int CI_SetPlusMode (CI_SLOT_ID usSlotId, CI_PLUS_MODE enPlusMode) {return ERR_OK;}
int CI_GetPlusMode (CI_SLOT_ID usSlotId, CI_PLUS_MODE *penPlusMode) {return ERR_OK;}
void CI_SetHostMode (CI_HOST_MODE enHostMode) {return;}
CI_HOST_MODE CI_GetHostMode (void) {return 0;}
int CI_GetCountryCode (unsigned char *pucCode) {return ERR_OK;}
int CI_GetLanguageCode (unsigned char *pucCode) {return ERR_OK;}
int CI_GetHostId (unsigned char ucHostId[8]) {return ERR_OK;}
int CI_GetBindedCamId (CI_SLOT_ID usSlotId, unsigned char *pucBindedCamId) {return ERR_OK;}
int CI_GetBindedCamBrandId (CI_SLOT_ID usSlotId, unsigned short *pusBindedCamBrandId) {return ERR_OK;}
int CI_GetNegotiatedCckCipher (CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER *penNegotiatedCckCipher) {return ERR_OK;}
void CI_ReqSetCckToDsc (CI_SLOT_ID usSlotId) {return;}
void CI_SetUriDefault (CI_SLOT_ID usSlotId, HBOOL bOn) {return;}
int CI_IsCiPlusCam (CI_SLOT_ID usSlotId) {return 0;}
HBOOL CI_OperatorProfileSupported (CI_SLOT_ID usSlotId) {return 0;}
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
int CI_RecordStart (CI_SLOT_ID usSlotId, Handle_t hAction, CI_OPERATING_MODE eOperatingMode, unsigned short usProgramNumber) {return ERR_OK;}
int CI_ChangeOperatingMode (CI_SLOT_ID usSlotId, unsigned int unSessionId, CI_OPERATING_MODE enOperatingMode, unsigned short usProgramNumber) {return ERR_OK;}
int CI_RecordStop (CI_SLOT_ID usSlotId, unsigned short usProgramNumber) {return ERR_OK;}
//int CI_Playback (CI_SLOT_ID usSlotId, unsigned int unSessionId, unsigned short usProgramNumber, unsigned char *pucLicenseData, unsigned int unLicenseDataLen) {return ERR_OK;}
int CI_CheckLicense (CI_SLOT_ID usSlotId, unsigned int unSessionId, unsigned char *pucLicenseData, unsigned int unLicenseDataLen) {return ERR_OK;}
int CI_GetPinCapaInfo (CI_SLOT_ID usSlotId, CI_PIN_CAPA_INFO *pstPinCapaInfo) {return ERR_OK;}
int CI_StoreCicamPinInfo (CI_SLOT_ID usSlotId, HUINT8 *pucPinInfo, UNIXTIME utPinSavedTime) {return ERR_OK;}
int CI_PinPlayback (CI_SLOT_ID usSlotId, HUINT32 unMetaIndex, HUINT8 ucAgeLimit, HUINT8 *pucPrivateData) {return ERR_OK;}
int CI_SendHostLicense (CI_SLOT_ID usSlotId, HUNIT32 unMetaIndex, Handle_t hPbAction, HUINT16 usProgramNum, HUINT8 *pucLicenseData) {return ERR_OK;)
#endif
#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
int CI_TuneReply (CI_SLOT_ID usSlotId, HUINT32 ulTargetResource, CI_TUNE_STATUS eTuneResult) {return ERR_OK;}
#endif

int CI_NotifyHostCountrySettingChangedToCam (CI_SLOT_ID usSlotId) {return ERR_OK;}
int CI_NotifyHostLanguageSettingChangedToCam (CI_SLOT_ID usSlotId) {return ERR_OK;}
int CI_MES_GetHostIdFromLicenseConstDataFile (char* hostId) {return ERR_OK;}
HERROR  CI_MES_LicenseDownCbFunc (HINT32 lResult) {return ERR_OK;}
void 	CI_MES_SetLicenseDownCbRegState (HBOOL bFlag) {return;}
HBOOL 	CI_MES_IsLicenseDownCbRegSuccess (void) {return ERR_OK;}
HBOOL 	CI_MES_IsLicenseDownTaskEnd (void) {return ERR_OK;}
int CIPLUS_Init(void) {return ERR_OK;}
int CIPLUS_AddNotifyFunc(CIPLUS_NOTIFY_FUNC_LIST notifyList) {return ERR_OK;}
int CIPLUS_DeleteNotifyFunc(CIPLUS_NOTIFY_FUNC_LIST notifyList) {return ERR_OK;}

#endif

#define ___CI_Stack_APIs_From_ci_reshandler_c_file___ /* from ci_reshandler.h and ci_reshandler.c */

int CIRH_SendApdu(unsigned short usSsNo, unsigned char *pucApdu, unsigned long ulApduLen) {return ERR_OK;}

#define ___CI_Stack_APIs_From_other_ci_files___ /* from ci xxx files */

int CiMmi_CheckWaitingUserInput(CI_SLOT_ID usSlotId, unsigned long* IsWaiting)
{
	return 0;
}

//=========================================================================
//===================== CI Emulator API  ==================================
//=========================================================================

#define ___CI_Emul_APIs___

static void _CiEmulPostMessage(HUINT32 type, HUINT32 nSlotId, HUINT32 lParam1, HUINT32 lParam2)
{
	CI_EMUL_MSG		msg;

	msg.type	= type;
	msg.lParam1	= lParam1;
	msg.lParam2	= lParam2;
	msg.nSlotId	= nSlotId;

	VK_MSG_Send(CiEmulMsgQId, (void*)&msg, sizeof(CI_EMUL_MSG));
}

static CI_SLOT_ID _MmiToSlotId(int hMmi)
{
/*
	if ( nSlotPool[0][hMmi] == 1)
		return 0;
	else if( nSlotPool[1][hMmi] == 1)
		return 1;
*/

	if (s_mmiState[0].hMmi == hMmi && s_mmiState[0].bState == 1)
	{
		return 0;
	}
	else if(s_mmiState[1].hMmi == hMmi && s_mmiState[1].bState == 1)
	{
		return 1;
	}
	return -1;
}

static const HINT8 *local_emulci_CamState2Str(CI_CAM_STATE cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(CI_CAM_EXTRACTED);
		ENUM_TO_STR(CI_CAM_INSERTED);
		ENUM_TO_STR(CI_CAM_INITIALIZING);
		ENUM_TO_STR(CI_CAM_INITIALIZED);
	}

	return ("local_emulci_CamState2Str : Unknown state");
}

static HINT32 _CamState(CI_SLOT_ID nSlotId, CI_CAM_STATE nCamState)
{
	CI_EMUL_Print((" %s, CamState [%s] \n", __FUNCTION__, local_emulci_CamState2Str(nCamState)));

	nSlotState[nSlotId] = nCamState;

	if(AppCiNotiFuncList.pfnNotifyCamState != NULL)
	{
		AppCiNotiFuncList.pfnNotifyCamState(nSlotId, nCamState);
	}

	return ERR_OK;
}

static HINT32 _AppInfo(CI_SLOT_ID nSlotId)
{
	CI_CAM_STATE cam_state;

	cam_state = nSlotState[nSlotId];

	if(cam_state != CI_CAM_INITIALIZED)
	{
		return ERR_OK;
	}

	if(AppCiNotiFuncList.pfnNotifyAppInfo)
	{
		AppCiNotiFuncList.pfnNotifyAppInfo(nSlotId, &_CiAppInfo[nSlotId]);
	}

	return ERR_OK;
}

static HINT32 _CaSysId(CI_SLOT_ID nSlotId)
{
	HUINT16	*pusCaSysId;
	HUINT8 	usCaInfoCnt;

	pusCaSysId = &s_CaSysId[nSlotId];
	usCaInfoCnt = 1;

	if(AppCiNotiFuncList.pfnNotifyCaInfo != NULL)
		AppCiNotiFuncList.pfnNotifyCaInfo(nSlotId, pusCaSysId, usCaInfoCnt);

	return ERR_OK;
}

static HINT32 _CasState(CI_SLOT_ID nSlotId)
{
	CI_CAS_STATE CiCasState;
	static int _state=0;

	CiCasState = _state;
	if(AppCiNotiFuncList.pfnNotifyCasState != NULL)
		AppCiNotiFuncList.pfnNotifyCasState(nSlotId, CiCasState);

	_state = (_state + 1) % 3;

	return ERR_OK;
}

static HINT32 _ReqMmiOpen(CI_SLOT_ID nSlotId)
{
	s_MmiInstance[EMUL_MMI_SSID].usSsNo = EMUL_MMI_SSID;


	MmiHandle = &s_MmiInstance[EMUL_MMI_SSID];

	//nSlotPool[nSlotId][MmiHandle] = 1;
	s_mmiState[nSlotId].bState = 1;
	s_mmiState[nSlotId].hMmi = MmiHandle;

	if (AppCiNotiFuncList.pfnNotifyMmiReq != NULL)
	{
		AppCiNotiFuncList.pfnNotifyMmiReq((MMI_HANDLE)MmiHandle, MMI_OPEN, NULL, 0);
	}

	return ERR_OK;
}

static HINT32 _ReqMmiClose(CI_SLOT_ID nSlotId)
{
	unsigned char ucDelaySec = 0;

	ucDelaySec = 1;	//Sec

	if (AppCiNotiFuncList.pfnNotifyMmiReq != NULL)
	{
		AppCiNotiFuncList.pfnNotifyMmiReq((MMI_HANDLE)MmiHandle, MMI_CLOSE, NULL, ucDelaySec);
	}

	//nSlotPool[nSlotId][MmiHandle] = 0;
	s_mmiState[nSlotId].bState = 0;
	s_mmiState[nSlotId].hMmi = HANDLE_NULL;
	//MmiHandle--;

	MmiHandle = HANDLE_NULL;

	return ERR_OK;
}

static HINT32 _ReqMmiMenu(CI_SLOT_ID nSlotId)
{
	int i;

	pciMenuParam = OxAP_Malloc(sizeof(MMI_MENU_LIST_PARAM));
	memset(pciMenuParam, 0, sizeof(MMI_MENU_LIST_PARAM));
	memcpy(pciMenuParam, &_ciMenuParam[_testIndex++], sizeof(MMI_MENU_LIST_PARAM));
	_testIndex %= 4;
	pciMenuParam->ppszItem = OxAP_Malloc(pciMenuParam->ulNumOfItem*sizeof(int));

	for (i=0; i<pciMenuParam->ulNumOfItem; i++)
	{
		pciMenuParam->ppszItem[i] = pszCiMenuItem[i];
	}

	if(AppCiNotiFuncList.pfnNotifyMmiReq != NULL)
		AppCiNotiFuncList.pfnNotifyMmiReq(MmiHandle, MMI_MENU, (void*) pciMenuParam, sizeof(MMI_MENU_LIST_PARAM));

	return ERR_OK;
}

static HINT32 _ReqMmiList(CI_SLOT_ID nSlotId)
{
	int i;

	pciListParam = OxAP_Malloc(sizeof(MMI_MENU_LIST_PARAM));
	memset(pciListParam, 0, sizeof(MMI_MENU_LIST_PARAM));
	memcpy(pciListParam, &_ciListParam[_testIndex++], sizeof(MMI_MENU_LIST_PARAM));
	_testIndex %= 4;
	//AP_DBG_ASSERT(pciListParam->ucNumOfItem > 0);
	pciListParam->ppszItem	 = OxAP_Malloc(pciListParam->ulNumOfItem*sizeof(int));

	for (i=0; i<pciListParam->ulNumOfItem; i++)
	{
		pciListParam->ppszItem[i] = pszCiList[i];
	}

	if(AppCiNotiFuncList.pfnNotifyMmiReq != NULL)
		AppCiNotiFuncList.pfnNotifyMmiReq(MmiHandle, MMI_LIST, (void*) pciListParam, sizeof(MMI_MENU_LIST_PARAM));

	return ERR_OK;
}

static HINT32 _ReqMmiText(CI_SLOT_ID nSlotId)
{
	pciTextParam = OxAP_Malloc(30);

	strcpy(pciTextParam, "CI Text Test Message");

	if(AppCiNotiFuncList.pfnNotifyMmiReq != NULL)
	{
		AppCiNotiFuncList.pfnNotifyMmiReq(MmiHandle, MMI_TEXT, (void*) pciTextParam, sizeof(HUINT8));
	}

	return ERR_OK;
}

static HINT32 _ReqMmiEnq(CI_SLOT_ID nSlotId)
{
	pciEnqParam = OxAP_Malloc(sizeof(MMI_ENQ_PARAM));
	memset(pciEnqParam, 0, sizeof(MMI_ENQ_PARAM));

	pciEnqParam->pszPrompt = "CI Enquiry Test Message";
	pciEnqParam->ucAnswerLen = 4;
	pciEnqParam->ucBlindOnOff = 0;

	if(AppCiNotiFuncList.pfnNotifyMmiReq != NULL)
	{
		AppCiNotiFuncList.pfnNotifyMmiReq(MmiHandle, MMI_ENQ, (void*) pciEnqParam, sizeof(MMI_ENQ_PARAM));
	}

	return ERR_OK;
}

HERROR EMUL_CI_CamState(CI_SLOT_ID nSlotId)
{
	static int nCamState = 0;

	_CiEmulPostMessage(EMUL_CI_CAM_STATE, nSlotId, nCamState, 0);
	nCamState++;
	nCamState %= 4;

	return ERR_OK;
}

HERROR EMUL_CI_AppInfo(CI_SLOT_ID nSlotId)
{
	_CiEmulPostMessage(EMUL_CI_APP_INFO, nSlotId, 0, 0);

	return ERR_OK;
}

HERROR EMUL_CI_CaSysId(CI_SLOT_ID nSlotId)
{
	_CiEmulPostMessage(EMUL_CI_CA_SYS_ID, nSlotId, 0, 0);

	return ERR_OK;
}

HERROR EMUL_CI_CasState(CI_SLOT_ID nSlotId)
{
	_CiEmulPostMessage(EMUL_CI_CAS_STATE, nSlotId, 0, 0);

	return ERR_OK;
}

HERROR EMUL_CI_ReqMmiOpen(CI_SLOT_ID nSlotId)
{
	_CiEmulPostMessage(EMUL_CI_MMI_REQ_OPEN, nSlotId, 0, 0);
	return ERR_OK;
}


HERROR EMUL_CI_ReqMmiClose(CI_SLOT_ID nSlotId)
{
	_CiEmulPostMessage(EMUL_CI_MMI_REQ_CLOSE, nSlotId, 0, 0);

	return ERR_OK;
}

HERROR EMUL_CI_ReqMmiMenu(CI_SLOT_ID nSlotId)
{
	_CiEmulPostMessage(EMUL_CI_MMI_REQ_MENU, nSlotId, 0, 0);

	return ERR_OK;
}

HERROR EMUL_CI_ReqMmiList(CI_SLOT_ID nSlotId)
{
	_CiEmulPostMessage(EMUL_CI_MMI_REQ_LIST, nSlotId, 0, 0);

	return ERR_OK;
}

HERROR EMUL_CI_ReqMmiText(CI_SLOT_ID nSlotId)
{
	_CiEmulPostMessage(EMUL_CI_MMI_REQ_TEXT, nSlotId, 0, 0);
	return ERR_OK;
}

HERROR EMUL_CI_ReqMmiEnq(CI_SLOT_ID nSlotId)
{
	_CiEmulPostMessage(EMUL_CI_MMI_REQ_ENQ, nSlotId, 0, 0);

	return ERR_OK;
}

//=========================================================================
//===================== CI Emulator Task ==================================
//=========================================================================

#define ___CI_Emul_TASK___

static void _CiEmulTask(void *param)
{
	CI_EMUL_MSG		msg;
	int 			ret;

	while( 1 )
	{
		ret = VK_MSG_Receive( CiEmulMsgQId, &msg, sizeof(CI_EMUL_MSG));
		if(ret)
		{
			CI_EMUL_Print(("Error:: CiTask- VK_MSG_Receive ret=0x%x\n", ret));
		}

		switch (msg.type)
		{
			case EMUL_CI_RESET_SLOT :
				break;

			case EMUL_CI_CAM_STATE :
				_CamState(msg.nSlotId, msg.lParam1);
				break;

			case EMUL_CI_APP_INFO :
				_AppInfo(msg.nSlotId);
				break;

			case EMUL_CI_CA_SYS_ID :
				_CaSysId(msg.nSlotId);
				break;

			case EMUL_CI_CAS_STATE :
				_CasState(msg.nSlotId);
				break;

			case EMUL_CI_MMI_REQ_OPEN :
				_ReqMmiOpen(msg.nSlotId);
				break;

			case EMUL_CI_MMI_REQ_CLOSE :
				_ReqMmiClose(msg.nSlotId);
				break;

			case EMUL_CI_MMI_REQ_MENU :
				_ReqMmiMenu(msg.nSlotId);
				break;

			case EMUL_CI_MMI_REQ_LIST :
				_ReqMmiList(msg.nSlotId);
				break;

			case EMUL_CI_MMI_REQ_TEXT :
				_ReqMmiText(msg.nSlotId);
				break;

			case EMUL_CI_MMI_REQ_ENQ :
				_ReqMmiEnq(msg.nSlotId);
				break;

			default :
				break;
		}
	}
}

#endif	// #ifdef	WIN32

