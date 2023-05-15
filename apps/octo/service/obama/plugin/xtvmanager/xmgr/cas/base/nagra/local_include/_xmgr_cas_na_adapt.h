#ifndef	__XMGR_CAS_NA_ADAPT_INT_H__
#define	__XMGR_CAS_NA_ADAPT_INT_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eCAS_NA_CA_ALARM_TYPE_ERR = 0,

	//reated by smartcard
	eCAS_NA_CA_SMARTCARD_OK = 0x1000,
	eCAS_NA_CA_SMARTCARD_COM_ERROR,
	eCAS_NA_CA_SMARTCARD_MUTE,
	eCAS_NA_CA_SMARTCARD_INVALID,
	eCAS_NA_CA_SMARTCARD_BLACKLISTED,
	eCAS_NA_CA_SMARTCARD_SUSPENDED,
	eCAS_NA_CA_SMARTCARD_NEVER_PAIRED,
	eCAS_NA_CA_SMARTCARD_NOT_PAIRED,
	eCAS_NA_CA_SMARTCARD_EXPIRED,
	eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED,
	eCAS_NA_CA_SMARTCARD_REMOVED,

	//reated by access
	eCAS_NA_CA_ACCESS_FREE,
	eCAS_NA_CA_ACCESS_GRANTED,
	eCAS_NA_CA_ACCESS_GRANTED_FOR_PREVIEW,
	eCAS_NA_CA_ACCESS_GRANTED_BUT_WARNING_PREVIEW,
	eCAS_NA_CA_ACCESS_GRANTED_PPT,
	eCAS_NA_CA_ACCESS_DENIED,
	eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD,
	eCAS_NA_CA_ACCESS_DENIED_BUT_PREVIEW,
	eCAS_NA_CA_ACCESS_BLACKOUT,
	eCAS_NA_CA_ACCESS_DENIED_BUT_PPT,
	eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT,
	eCAS_NA_CA_ACCESS_DENIED_FOR_PARENTAL_RATING,
	eCAS_NA_CA_ACCESS_DENIED_COPY_PROTECTED,
	eCAS_NA_CA_ACCESS_DENIED_PARENTAL_CONTROL,
	eCAS_NA_CA_ACCESS_DENIED_DIALOG_REQUIRED,
	eCAS_NA_CA_ACCESS_DENIED_PAIRING_REQUIRED,
	eCAS_NA_CA_ACCESS_DENIED_CS_PAIRING_REQUIRED
} XmgrCas_NaAlamType_e;

typedef enum
{
	eCAS_NA_POPUPCMD = 0,
	eCAS_NA_BDCCMD,
	eCAS_NA_IRDCMD,
	eCAS_NA_COPCMD
} XmgrCas_NaUiCmdType_e;

typedef enum
{
	eCAS_NA_POPUP_DISPLAY = 0,
	eCAS_NA_POPUP_CLOSE
} XmgrCas_NaUiPopupCmd_e;

typedef enum
{
	eCAS_NA_POPUP_MODE_ALWAYS_ON_TOP = 0,
	eCAS_NA_POPUP_MODE_USER_EXIT,
	eCAS_NA_POPUP_MODE_TIMEOUT,
	eCAS_NA_POPUP_MODE_PPT
} XmgrCas_NaUiPopupMode_e;

typedef struct
{
	XmgrCas_NaUiPopupCmd_e		popup_cmd;
	XmgrCas_NaUiPopupMode_e		popup_mode;
	HUINT8						*pszPopup_msg;
} XmgrCas_NaUiPopupData_t;

typedef enum
{
	eCAS_NA_ALARM_BANNER,
	eCAS_NA_ALARM_PAIRING
} XmgrCas_NaUiAlarmType_e;

typedef struct
{
	XmgrCas_NaUiAlarmType_e	alarm_type;
	HINT32				msgId;
} XmgrCas_NaUiAlramData_t;


enum
{
	eCAS_NA_CMD_ERR_OK = ERR_OK,
	eCAS_NA_CMD_ERR_NULL,
	eCAS_NA_CMD_ERR_BDC_NULL,
	eCAS_NA_CMD_ERR_BDC_LOADING_CC,
	eCAS_NA_CMD_ERR_IRD_NULL,
	eCAS_NA_CMD_ERR_FAIL
};

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR	xmgr_cas_NaAlarmDataProcessing(void *pDataNa, XmgrCas_NaAlamType_e *peCasAlarmType);
HERROR	xmgr_cas_NaGetPopupData(XmgrCas_NaUiPopupCmd_e *popup_cmd, XmgrCas_NaUiPopupMode_e *popup_mode, HUINT8 **pszMsg);
HERROR	xmgr_cas_NaCommandProcessing(void *pNA_data, XmgrCas_NaUiCmdType_e *pCmdType);
HERROR	xmgr_cas_NaGetSmartCardStatus(XmgrCas_NaAlamType_e *peScAlarm);
HBOOL 	xmgr_cas_NaIsSmartCardStatusOK(void);
HERROR	xmgr_cas_NaGetSmartCardStatusBySlotID(HUINT8 ucSlotId, XmgrCas_NaAlamType_e *peScAlarm);


/*******************************************************************/
/********************      For debug     ********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
void	xmgr_cas_NaDbgCmdIrdCmdMail(void);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FORMATDISK)
void	xmgr_cas_NaDbgCmdIrdCmdFormatDisk(void);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_PINCTL)
void	xmgr_cas_NaDbgCmd_IrdCmdPincode(void);
#endif
#endif
#endif /* !__XMGR_CAS_NA_ADAPT_INT_H__ */

