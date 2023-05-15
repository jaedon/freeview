#ifndef __AP_CAS_VA_SC_H__
#define __AP_CAS_VA_SC_H__

#include "va_def.h"
#include "va_ui_adapt.h"

typedef struct
{
	HUINT32 uiOpNum;
	VA_UI_OPERATOR_LIST *pOpList;
}VA_MENU_OP_LIST;

typedef struct
{
	HUINT32 uiNum;
	VA_UI_CLASS_LIST *pList;
}VA_MENU_CLASS_CONSULTATION;

typedef struct
{
	HUINT32 uiNum;
	VA_UI_THEME_LEVEL_LIST *pList;
}VA_MENU_THEME_CONSULTATION;

typedef struct
{
	HUINT32	uiNum;
	VA_UI_PREBOOKED_LIST *pList;
}VA_MENU_PREBOOK_CONSULTATION;

typedef VA_UI_OP_MESSAGE VA_MENU_OP_MESSAGE;

typedef struct
{
	HUINT32 uiPairMode;
	HUINT32 uiPairNum;
	HUINT32 uiUaNum;
	VA_UI_UA *pUaList;
}VA_MENU_SMARTCARD_INFO;

typedef struct
{
	HUINT32 uiManufId;
	HUINT32 uiModelId;
	HUINT8	*pszTerminalSn;
	HUINT8	*pszHwVer;
	HUINT8	*pszBlVer;
	HUINT8	*pszSwVer;
	HUINT8	*pszStbId;
#ifdef	CONFIG_MW_CAS_VIACCESS_PVR
	HUINT8	*pszHddSn;
#endif
	HUINT8	*pszUA;
	HUINT8	*pszAcsVer;
#ifdef CONFIG_MW_CAS_VIACCESS_PURPLE_BOX
	HUINT8	*pszTerminalId;
#endif
	HUINT8	*pszCamlock;
}VA_MENU_STB_INFO;

#ifdef CONFIG_MW_CAS_VIACCESS_PVR
typedef struct
{
	HUINT8	szSvcName[TEXT_SIZE128];
	VA_UI_PLAYBACK_INFO	stInfo;
}VA_MENU_PLAYBACK_INFO;
#endif

HERROR xmgr_cas_VaGetOpList(VA_MENU_OP_LIST *pOp);
HERROR xmgr_cas_VaGetOpClass(VA_UI_OPERATOR_LIST *pOp, VA_MENU_CLASS_CONSULTATION *pClass, HINT32 *pResult);
HERROR xmgr_cas_VaGetOpTheme(VA_UI_OPERATOR_LIST *pOp, VA_MENU_THEME_CONSULTATION *pTheme, HINT32 *pResult);
HERROR xmgr_cas_VaGetOpPrebook(VA_UI_OPERATOR_LIST *pOp, VA_MENU_PREBOOK_CONSULTATION *pPrebook, HINT32 *pResult);
HERROR xmgr_cas_VaGetOpLastMessage(VA_UI_OP_MESSAGE **ppOpMsg);
HERROR xmgr_cas_VaGetSmartcardInfo(VA_MENU_SMARTCARD_INFO *pInfo);
HERROR xmgr_cas_VaGetStbInfo(VA_MENU_STB_INFO *pInfo);
HERROR xmgr_cas_VaGetPinCodeAttemps(HUINT8 *pucRemain, HUINT8 *pucMax, HINT32 *pResult);
HERROR xmgr_cas_VaChangePinCode(HUINT8 *pszOld, HUINT8 *pszNew, HINT32 *pResult);
HERROR xmgr_cas_VaGetMaturityRate(HUINT8 *pucCurrentMR, HUINT8 *pucMaxMR, HINT32 *pResult);
HERROR xmgr_cas_VaChangeMaturityRate(HUINT8 *pszPin, HUINT8 ucMaturityRate, HINT32 *pResult);

#endif
