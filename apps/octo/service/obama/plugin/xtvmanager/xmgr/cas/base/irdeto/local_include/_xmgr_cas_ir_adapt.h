#ifndef	__XMGR_CAS_IR_ADAPT_INT_H__
#define	__XMGR_CAS_IR_ADAPT_INT_H__


#include <ir_api.h>
#include <mgr_live.h>

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
	eCAS_IR_CA_ALARM_TYPE_ERR = 0,

	//reated by smartcard
	eCAS_IR_CA_SMARTCARD_OK = 0x1000,
	eCAS_IR_CA_SMARTCARD_COM_ERROR,
	eCAS_IR_CA_SMARTCARD_MUTE,
	eCAS_IR_CA_SMARTCARD_INVALID,
	eCAS_IR_CA_SMARTCARD_BLACKLISTED,
	eCAS_IR_CA_SMARTCARD_SUSPENDED,
	eCAS_IR_CA_SMARTCARD_NEVER_PAIRED,
	eCAS_IR_CA_SMARTCARD_NOT_PAIRED,
	eCAS_IR_CA_SMARTCARD_EXPIRED,
	eCAS_IR_CA_SMARTCARD_NOT_CERTIFIED,
	eCAS_IR_CA_SMARTCARD_REMOVED,

	//reated by access
	eCAS_IR_CA_ACCESS_FREE,
	eCAS_IR_CA_ACCESS_GRANTED,
	eCAS_IR_CA_ACCESS_GRANTED_PPT,
	eCAS_IR_CA_ACCESS_GRANTED_FOR_PREVIEW,
	eCAS_IR_CA_ACCESS_GRANTED_BUT_WARNING_PREVIEW,
	eCAS_IR_CA_ACCESS_DENIED,
	eCAS_IR_CA_ACCESS_DENIED_BUT_PREVIEW,
	eCAS_IR_CA_ACCESS_DENIED_FOR_PARENTAL_RATING,
	eCAS_IR_CA_ACCESS_BLACKOUT,
	eCAS_IR_CA_ACCESS_DENIED_BUT_PPT,
	eCAS_IR_CA_ACCESS_DENIED_NO_VALID_CREDIT,
	eCAS_IR_CA_ACCESS_DENIED_COPY_PROTECTED,

	// Event Msg
	eCAS_IR_CA_EVT_TIME_RESET,			// Finger Print Time Reset

	eCAS_IR_CA_TYPE_NULL
} XmgrCas_IrAlarmType_e;

#if 0
enum
{
	eCAS_IR_CMD_ERR_OK = ERR_OK,
	eCAS_IR_CMD_ERR_NULL,
	eCAS_IR_CMD_ERR_BDC_NULL,
	eCAS_IR_CMD_ERR_BDC_LOADING_CC,
	eCAS_IR_CMD_ERR_IRD_NULL
};

typedef enum
{
	eCAS_IR_POPUP_DISPLAY = 0,
	eCAS_IR_POPUP_CLOSE
} IR_UI_POPUP_CMD;

typedef enum
{
	eCAS_IR_POPUP_MODE_ALWAYS_ON_TOP = 0,
	eCAS_IR_POPUP_MODE_USER_EXIT,
	eCAS_IR_POPUP_MODE_TIMEOUT
} IR_UI_POPUP_MODE;

typedef struct
{
	IR_UI_POPUP_CMD		popup_cmd;
	IR_UI_POPUP_MODE	popup_mode;
	HUINT8				*pszPopup_msg;
} IR_UI_POPUP_DATA;
#endif

typedef enum
{
	eCAS_IR_ALARM_BANNER,
	eCAS_IR_ALARM_PAIRING
} XmgrCas_IrUiAlarmType_e;

typedef struct
{
	XmgrCas_IrUiAlarmType_e	alarm_type;
	IR_API_SC_INFO			smartcard_info;
} XmgrCas_IrUiAlarmData_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#if 0
void	xmgr_cas_ir_GetErrorText(HINT32	nIrErrCode, HUINT8 *szErrorText, HUINT32 ulTextBufSize);
#endif
void	xmgr_cas_ir_GetErrorCodeText(HINT32 nIrErrCode, HUINT8 *szErrorCodeText, HUINT32 ulTextBufSize);
void	xmgr_cas_ir_GetErrorText(HINT32	nIrErrCode, HUINT8 *szErrorText);
HERROR	xmgr_cas_ir_AlarmDataProcessing(void *pDataNa, HUINT8 *alarm_type);

#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
void	xmgr_cas_ir_GetPreviewTimeText(HUINT8	nLeftTime, HUINT8 *szErrorText);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)	// ewlee 20100720
void	xmgr_cas_iruc_GetErrorText(HINT32	nIrErrCode, HUINT8 *szErrorText);
#endif


/* Command msg Process */
void 	xmgr_cas_IrCMD(HUINT8 *pszCmd, HUINT8 *pszMsgType, HUINT8 *pszmsg1, HUINT8 *pszmsg2);


#endif /* !__XMGR_CAS_IR_ADAPT_INT_H__ */

