#ifndef	__XMGR_CAS_CI_INT_H__
#define	__XMGR_CAS_CI_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_cas.h>

#include <mgr_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if 0
/* Object ID */
#define MMI_WINDOW_ID				(1)
#define MMI_MAINTITLE_ID			(100)
#define MMI_SUBTITLE_ID				(200)
#define MMI_SUBTITLE1_ID			(201)
#define MMI_SUBTITLE2_ID			(202)
#define MMI_MENULIST_ID				(300)
#define MMI_BOTTOMTEXT_ID			(400)
#define MMI_ENQOBJ_ID				(500)
#define MMI_TOP_TITLE_ID			(301)

#define MMI_NULL_TEXT 				" "
#define MMI_TOP_TEXT 				"Common Interface"

#define XMGR_MMI_CREATE_BY_MENU		(3)
#define XMGR_MMI_CREATE_BY_WTV		(1)
#endif
#define XMGR_MMI_MAX_ENQ_STRLEN		(40) // MAX_CI_STR_LEN

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eCiCas_CamRemoved,
	eCiCas_CamInserted,
	eCiCas_CamInitialized,

	eCiCasUi_CamStatusMax
} XmgrCas_CiCamStatus_e;

#if 0
typedef enum
{
	eCiCasUi_SessionZapMmi = eDxCAS_SESSIONTYPE_CUSTOM,
	eCiCasUi_SessionSystemMessage,
	eCiCasUi_SessionCiPlusMessage,
	eCiCasUi_SessionCiPlusUpgrade,

	eCiCasUi_SessionMax
} XmgrCas_CiCasUiCustomSession_e;
#endif

typedef struct
{
	HUINT8			pucEnqStr[XMGR_MMI_MAX_ENQ_STRLEN];
	HUINT32			ulAnswerLen;
	HBOOL			bBlind;
	Handle_t		hMmi;
} XmgrCas_CiMmiEnqParam_t;

typedef enum
{
	eCiMmiType_Text,
	eCiMmiType_Menu,
	eCiMmiType_List,
	eCiMmiType_Enq,
	eCiMmiType_Max
} XmgrCas_CiMmiType_e;

typedef struct
{
	unsigned long			hMmi;						/* See : MMI_HANDLE */
	HBOOL					bMmiWindow;					/* if mmi is valid(displayed) then this flag is TRUE */
	HBOOL					bFromMenu;
	XmgrCas_CiMmiType_e			curMmiType;
} XmgrCas_CiMmiStatus_t;

#define XMGR_CAS_CI_SLOT_MAX			(2)

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
/* ap_cas_mmi.c */
HERROR 			xmgr_cas_CiMmiGetCurrentMmiType(HUINT32 *pulType);
HBOOL 			xmgr_cas_CiMmiIsCreatedByMenu(HUINT16 usSlotId);
HUINT32 		xmgr_cas_CiMmiGetCurrentSlotId(void);
BUS_Result_t 	xproc_cas_CiMmi(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

HERROR			xproc_cas_CiDestroyCamTune(HUINT32 unSlotNumber);

BUS_Result_t	xproc_cas_CiUiSysMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CiPlusSysMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CiPlusConfirmMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CiPlusBannerMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CiPlusPinMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	xproc_cas_CiTuneMgr(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	xproc_cas_CiCamUpgrade(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	xproc_cas_CiOperatorProfile(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CiOpSearchOperatorProfile(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#endif /* !__XMGR_CAS_CI_INT_H__ */

/* EOF */

