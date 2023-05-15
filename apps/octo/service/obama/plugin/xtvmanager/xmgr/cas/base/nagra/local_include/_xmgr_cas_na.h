#ifndef	__XMGR_CAS_NA_INT_H__
#define	__XMGR_CAS_NA_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <bus.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA_PPV)
#define EPG_EVENT_DUMMY_LEN			20
#define EPG_EVENT_IGNORE_MIN		(SECONDS_PER_MIN/2)
#define MAX_EVENTNAME_LEN			(DxEPG_SHORT_EVENT_NAME + EPG_LANG_ISO_639_LEN + EPG_EVENT_DUMMY_LEN + 1)
#define MAX_EVENTTEXT_LEN			(DxEPG_EVENT_TEXT + EPG_LANG_ISO_639_LEN + EPG_EVENT_DUMMY_LEN + 1)
#define MAX_SERVICENAME_LEN			64
#endif


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eNaCasUiType_CasMenu,
	eNaCasUiType_SystemMessage,
	eNaCasUiType_CaPopup,
	eNaCasUiType_Bdc,
	eNaCasUiType_MailIcon,
	eNaCasUiType_MailMessage,
	eNaCasUiType_FingerPrint,
	eNaCasUiType_PinDialog,
	eNaCasUiType_PpvDialog,
	eNaCasUiType_PurchaseList,
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	eNaCasUiType_EntitlementMessage,
#endif
	eNaCasUiType_Max
} XmgrCas_NaUiType_e;

typedef enum
{
	eNA_MODE_EPG_EVENT_INVALID,
	eNA_MODE_EPG_EVENT_VALID,
#if	defined(CONFIG_MW_MM_PVR)
	eNA_MODE_EPG_EVENT_PVR_VALID,
#endif
	eNA_MODE_EPG_EVENT_MEDIA_VALID,

	eNA_MODE_EPG_EVENT_MAX
} XmgrCas_NaModeEpgEvent_e;

typedef struct tagNaEpgApEvent
{
	HUINT32					ulEventState;
	Handle_t				hSvcHandle;

	UNIXTIME				ulExtDuration; // ignore event duration
#if defined(CONFIG_MW_CAS_NAGRA_PPV)
	HUINT8					aucEventText[MAX_EVENTTEXT_LEN];
	HUINT8					aucShortEventName[MAX_EVENTNAME_LEN];
	SvcEpg_Event_t			stEventInfo;
#endif
#if	defined(CONFIG_MW_MM_PVR)
	Handle_t				hActionHandle;
	HUINT32					ulOffsetOfPlayTime;
#endif
	struct tagNaEpgApEvent		*pNext;
} XmgrCas_NaEpgEvent_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HBOOL			xmgr_cas_NaIsActiveCasSystemMessage(void);

//cas banner message
HUINT8*			xmgr_cas_NaGetMessage (HINT32 nAlarmType);
BUS_Result_t 	xproc_cas_NaMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_NaUiSysMsg (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

//cas bdc message
BUS_Result_t	xproc_cas_NaUiBdc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_NaUiPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR			xmgr_cas_NaRequestCasUiBdcSessionClose (void);
HERROR			xmgr_cas_NaBcdSetData (HINT32 popup_mode, HUINT8 *pszMsg);
BUS_Result_t	xproc_cas_NaUiMailIcon(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_NaUiMailMessage(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t	xproc_cas_NaUiSysInfo (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_NaMenuPincode (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR			xmgr_cas_NaIrdCmdEmmGetWakeupTime (UNIXTIME *pulTime);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
BUS_Result_t	xproc_cas_NaUiEntitlementMessage(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
BUS_Result_t	xproc_cas_NaUiPinDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
BUS_Result_t 	xproc_cas_NaManagement_Mediaset (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_NaPpvBanner_Pangea (HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
HBOOL			xmgr_cas_NaPpvBannerCheckStepConfirm_Mediaset (void);
HBOOL			xmgr_cas_NaPpvPangeaBannerCheckStepConfirm (void);
HBOOL			xmgr_cas_NaPpvPangeaBannerCheckStepConfirm (void);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
BUS_Result_t	xproc_cas_NaAps (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	xproc_cas_NaManagement_Aps (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
void			xmgr_cas_NaCmd_Aps (HUINT8 *pszCmd, HINT32 ulParam1, HINT32 ulParam2, HUINT8 *pszmsg);
HUINT8*			xmgr_cas_NaGetMessage_Aps (HINT32 nMessage);
HERROR			xmgr_cas_NaDefaultSetting_Aps (void);
HERROR			xmgr_cas_NaCheckIrdCommand_Aps (void);
HERROR			xmgr_cas_NaIrdCommandUpdated_Aps (HUINT32 cmdType);
HERROR			xmgr_cas_NaCheckMsgProcessing_Aps (Handle_t hAction, void *pMessage);
HERROR			xmgr_cas_NaGetMessageTypeAndString_Aps (HUINT8 *pMessageType, HUINT8 *pszMessage);
HERROR			xmgr_cas_NaUpdateCasMessage_Aps (eCAS_NA_UI_APSUpdate type);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST) && defined(CONFIG_MW_CAS_NAGRA_OP_SES) // BDC_TEST
void 			xmgr_cas_NaCasUiRequestSessionStopFromUi(void);
#endif

#if defined(CONFIG_OP_RCS)
BUS_Result_t 	xproc_cas_NaManagement_Rcs (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_DEBUG)
HUINT8			*xmgr_cas_NaGetMessageName(HINT32 message);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
BUS_Result_t 	xproc_cas_NaPrmMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP) && defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
BUS_Result_t 	xproc_cas_NaPrmNovaMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

BUS_Callback_t xmgr_cas_Na_GetUiProc(XmgrCas_NaUiType_e eNaUiType);

#endif /* !__XMGR_CAS_NA_INT_H__ */

