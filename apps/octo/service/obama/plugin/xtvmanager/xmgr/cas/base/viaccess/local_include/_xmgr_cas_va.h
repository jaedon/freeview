#ifndef	__AP_CAS_VA_H__
#define	__AP_CAS_VA_H__
#include <bus.h>
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
	eCAS_VA_UI_None,
	eCAS_VA_UI_Error,
	eCAS_VA_UI_AgreeMaturityRate,
	eCAS_VA_UI_NewOpMessage,
	eCAS_VA_UI_PreView,
	eCAS_VA_UI_CamlockOk,
	eCAS_VA_UI_Max
}eCAS_VA_UI_STATE;

typedef enum
{
	eVaCasUi_CasMenu			= 0x00000001,
	eVaCasUi_SystemMessage 		= 0x00000002,
	eVaCasUi_CheckMessage		= 0x00000004,
	eVaCasUi_MailIcon			= 0x00000008,
	eVaCasUi_MailMessage		= 0x00000010,
	eVaCasUi_FingerPrint		= 0x00000020,
	eVaCasUi_PinDialog			= 0x00000040,
	eVaCasUi_PpvDialog			= 0x00000080,
	eVaCasUi_PurchaseList		= 0x00000100,
	eVaCasUi_CamlockScreen		= 0x00000200,
	eVaCasUi_Max
}VA_CAS_UI_e;

typedef enum
{
	eVaCasUi_FnCasMenu = 0,
	eVaCasUi_FnSystemMessage,
	eVaCasUi_FnCheckMessage,
	eVaCasUi_FnMailIcon,
	eVaCasUi_FnMailMessage,
	eVaCasUi_FnFingerPrint,
	eVaCasUi_FnPinDialog,
	eVaCasUi_FnPpvDialog,
	eVaCasUi_FnPurchaseList,
	eVaCasUi_FnCamlockScreen,

	eVaCasUi_FnMax
}VA_CAS_UI_FN_e;

typedef struct
{
	HINT32 nSecurity; /* TRUE or FALSE */
	HINT32 nCamLockCtrlLevel; /* VA_CAMLOCK_CONTROL_NO, VA_CAMLOCK_CONTROL_LOW, VA_CAMLOCK_CONTROL_MEDIUM, VA_CAMLOCK_CONTROL_HIGH */
	HINT32 nCamlock; /* VA_UI_CAMLOCK_ACTIVATED_OK, ..., VA_UI_CAMLOCK_DISABLED */
	HINT32 nState;	/* VA_UI_SC_INSERTED, VA_UI_SC_EXTRACTED, VA_UI_SC_READY, VA_UI_SC_FAILED, VA_UI_SC_NOT_ACCEPTED, VA_UI_SC_INVALIDATED */
}CAS_VA_SC_State_t;

/* xmgr_cas_va_GetCamlockState() 의 리턴값 */
#define AP_CAS_VA_SECURITY_CTRL_ON		0x10
#define AP_CAS_VA_CAMLOCK_CTRL_ON		0x01

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
//HUINT8 xmgr_cas_VaGetCamlockState(HBOOL bCheckSecurityRestriction);
//HINT32 xmgr_cas_VaGetCamlockLevel(void);
HERROR AP_CAS_VA_Start(void);

BUS_Result_t	xmgr_cas_VaOpMessageProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_cas_VaCamlockScreenProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
//BUS_Result_t	xmgr_cas_VaCheckMessageProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_cas_VaNotificationProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#if defined(CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG)
BUS_Result_t	xmgr_cas_VaOpMessageIconProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

BUS_Result_t	xmgr_cas_VaStatusListProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_cas_VaMaturityRatingProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#endif /* !__AP_CAS_VA_H__ */

