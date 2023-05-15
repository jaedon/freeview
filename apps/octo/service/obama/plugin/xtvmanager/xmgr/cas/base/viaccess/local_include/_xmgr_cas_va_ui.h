
#ifndef __AP_CAS_VA_UI_H__
#define __AP_CAS_VA_UI_H__

#include <bus.h>

#if defined(CONFIG_OP_NORDIG_BOXER)
// model id assigned by BOXER
#define BOXER_MODELID_BXR_HD			152
#define BOXER_MODELID_BXR_HD_PLUS		153
#endif

#define SCREEN_SAVER_TIMER_ID		eDxCAS_GROUPID_VA

typedef enum
{
	eVA_PinCheckEnter_AcceptView = 0,
	eVA_PinCheckEnter_MR,
	eVA_PinCheckEnter_MR_CHG,
	eVA_PinCheckEnter_TokenDebit,
	eVA_PinCheckEnter_PPV_DEBIT0,
	eVA_PinCheckEnter_PPV_DEBIT1,

	eVA_PinCheckEnter_UnlockConsultation,
	eVA_PinCheckEnter_AgreeMaturityRate,
	eVA_PinCheckEnter_ModifyMaturityRate,
	eVA_PinCheckEnter_Max,
} eCasVa_PinCheckEnterCA_InDlg;

/*
	Check Pin-code Step
*/
typedef enum
{
	eVA_PinCheckStep_InDlg_InputPincode = 0,
	eVA_PinCheckStep_InDlg_WrongPincode,
	eVA_PinCheckStep_InDlg_NoPincode,
	eVA_PinCheckStep_InDlg_MAX,
} eCasVa_PinCheckStep_InDlg;

/*
	Change Pin-code Step
*/
typedef enum
{
	eVA_PinChangeStep_InDlg_InputCurPincode = 0,
	eVA_PinChangeStep_InDlg_InputNewPincode,
	eVA_PinChangeStep_InDlg_VerifyNewPincode,
	eVA_PinChangeStep_InDlg_MAX,
} eCasVA_PinChangeStep_InDlg;

/*
	Change Pin-code Error
*/
typedef enum
{
	eVA_PinChangeError_Normal = 0,
	eVA_PinChangeError_InputCodeAreNotMatch,
	eVA_PinChangeError_ConfirmationAreNotMatch,
	eVA_PinChangeError_ChangeFailed,
	eVA_PinChangeError_ScLocked,
	eVA_PinChangeError_NoPincode,
	eVA_PinChangeError_MAX,
} eCasVA_PinChangeError;

HERROR xmgr_cas_VaHideMessageBox();
HERROR xmgr_cas_VaShowMessageBox(HUINT8 *pszMsg);
HERROR xmgr_cas_VaShowMessageBoxByLoc(HINT32 nLocId);
HERROR xmgr_cas_VaSetScreenSaver(HINT32 message);

extern BUS_Result_t	xmgr_cas_VaSmartCardMenuProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t	xmgr_cas_VaNotificationProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

//BUS_Callback_t	xmgr_cas_VaCbGetUiProc(VA_CAS_UI_FN_e eUiType);

#endif
