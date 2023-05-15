#ifndef	__XMGR_CAS_IR_UI_INT_H__
#define	__XMGR_CAS_IR_UI_INT_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	USE_WEB_UI_COORDINATE		1


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eCasIr_UI_ErrorMessage,
	eCasIr_UI_Menu,
	eCasIr_UI_MailIcon,
	eCasIr_UI_MailMessage,
	eCasIr_UI_Last
} XmgrCas_IrUiType_e;

// OxMgrCasMenuGroupId_e ¿Í mapping
typedef enum
{
	eCasIrMenuGroup_None = 0,
	eCasIrMenuGroup_SmartCardStatus,
	eCasIrMenuGroup_SoftCellServiceStatus,
	eCasIrMenuGroup_LoaderStatus,
	eCasIrMenuGroup_Components,
	eCasIrMenuGroup_MailMessages,
	eCasIrMenuGroup_MAX
} XmgrCas_IrMenuGroup_e;

typedef enum
{
	eIR_UI_SESSION_STATUS_Stopped,
	eIR_UI_SESSION_STATUS_Started,

	eIR_UI_SESSION_STATUS_Max
} XmgrCas_IrUiSessionStatus_e;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
BUS_Result_t	xmgr_cas_IrCreateUI (HUINT8 *szName, HUINT32 ulProperty, XmgrCas_IrUiType_e eUiType, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Callback_t	xmgr_cas_IrGetUiProc (XmgrCas_IrUiType_e eUiType);
IREVT_CLASSe	xmgr_cas_IrMailMessageGetCurrentEvtClass	(void);

BUS_Result_t	xproc_cas_IrSysMsg (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_IrMailIcon (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_IrMailMessage (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t	xproc_cas_IrMenu (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

HERROR 			xmgr_cas_IrSystemInfoUpdateMenu(void);

#endif /* !__XMGR_CAS_IR_UI_INT_H__ */

