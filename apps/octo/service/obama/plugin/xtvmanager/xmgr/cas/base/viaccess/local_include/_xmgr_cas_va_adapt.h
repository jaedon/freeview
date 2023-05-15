#ifndef	__AP_CAS_VA_ADAPT_H__
#define	__AP_CAS_VA_ADAPT_H__

#include <vamgr_main.h>
#include <va_ui_adapt.h>

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
	eCAS_VA_CA_ALARM_TYPE_ERR = 0,

	//reated by smartcard
	eCAS_VA_CA_SMARTCARD_OK = 0x1000,
	eCAS_VA_CA_SMARTCARD_COM_ERROR,
	eCAS_VA_CA_SMARTCARD_MUTE,
	eCAS_VA_CA_SMARTCARD_INVALID,
	eCAS_VA_CA_SMARTCARD_BLACKLISTED,
	eCAS_VA_CA_SMARTCARD_SUSPENDED,
	eCAS_VA_CA_SMARTCARD_NEVER_PAIRED,
	eCAS_VA_CA_SMARTCARD_NOT_PAIRED,
	eCAS_VA_CA_SMARTCARD_EXPIRED,
	eCAS_VA_CA_SMARTCARD_NOT_CERTIFIED,
	eCAS_VA_CA_SMARTCARD_REMOVED,

	//reated by access
	eCAS_VA_CA_ACCESS_FREE,
	eCAS_VA_CA_ACCESS_GRANTED,
	eCAS_VA_CA_ACCESS_GRANTED_PPT,
	eCAS_VA_CA_ACCESS_GRANTED_FOR_PREVIEW,
	eCAS_VA_CA_ACCESS_GRANTED_BUT_WARNING_PREVIEW,
	eCAS_VA_CA_ACCESS_DENIED,
	eCAS_VA_CA_ACCESS_DENIED_BUT_PREVIEW,
	eCAS_VA_CA_ACCESS_DENIED_FOR_PARENTAL_RATING,
	eCAS_VA_CA_ACCESS_BLACKOUT,
	eCAS_VA_CA_ACCESS_DENIED_BUT_PPT,
	eCAS_VA_CA_ACCESS_DENIED_NO_VALID_CREDIT,
	eCAS_VA_CA_ACCESS_DENIED_COPY_PROTECTED

} VA_CAS_ALARM_TYPE;

typedef enum
{
	eCAS_VA_POPUPCMD = 0,
	eCAS_VA_IRDCMD,
	eCAS_VA_COPCMD
} VA_UI_CMD_TYPE;

typedef enum
{
	eCAS_VA_POPUP_DISPLAY = 0,
	eCAS_VA_POPUP_CLOSE
} VA_UI_POPUP_CMD;


typedef enum
{
	eCAS_VA_POPUP_MODE_ALWAYS_ON_TOP = 0,
	eCAS_VA_POPUP_MODE_USER_EXIT,
	eCAS_VA_POPUP_MODE_TIMEOUT
} VA_UI_POPUP_MODE;


typedef struct
{
	VA_UI_POPUP_CMD		popup_cmd;
	VA_UI_POPUP_MODE	popup_mode;
	HUINT8				*pszPopup_msg;
} VA_UI_POPUP_DATA;


typedef enum
{
	eCAS_VA_ALARM_BANNER,
	eCAS_VA_ALARM_PAIRING
} VA_UI_ALARM_TYPE;

typedef struct
{
	VA_UI_ALARM_TYPE	alarm_type;
	HINT32				msgId;
} VA_UI_ALARM_DATA;


enum
{
	eCAS_VA_CMD_ERR_OK = ERR_OK,
	eCAS_VA_CMD_ERR_NULL,
	eCAS_VA_CMD_ERR_BDC_NULL,
	eCAS_VA_CMD_ERR_BDC_LOADING_CC,
	eCAS_VA_CMD_ERR_IRD_NULL
};


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR		xmgr_cas_VaAdaptMessageProcess(void *pData, void *pScState, void *pUiState);
HERROR		xmgr_cas_VaAdaptGetErrorMessage(HUINT8 *pszMsg);

#if defined(CONFIG_OP_NORDIG_BOXER)
HUINT32 		xmgr_cas_VaAdaptGetBoxerModel_ID(void);
HERROR 		xmgr_cas_VaAdaptGetBoxerScLockMsg(HUINT8 *pszLockMsg_512);
HERROR  		xmgr_cas_VaAdaptShowBoxerScLockMsg(void);
#endif

#endif /* !__AP_CAS_VA_ADAPT_H__ */

