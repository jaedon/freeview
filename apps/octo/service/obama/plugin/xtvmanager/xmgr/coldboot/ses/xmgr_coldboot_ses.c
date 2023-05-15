/****************************************************************************
*
* File Name   : xmgr_coldboot_ses.c
*
* Description : AP cold boot processing application. No cold boot operation.
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
*
****************************************************************************/

#if defined(CONFIG_OP_SES)
/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
/** @brief global header file  */
#include <octo_common.h>

#include <db_param.h>

#include <svc_output.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_pipe.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_time.h>
#include <mgr_coldboot.h>
#include <mgr_swup.h>
#include <mgr_action.h>

#include <xmgr_swup.h>
#include <xmgr_module_search.h>

#include "../local_include/_xmgr_coldboot.h"

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */
/** high priority ~ low priority */
typedef enum
{
	eSesColdbootProcess_SesBoot						= 0x00000001,		// Time Check, OTA, LCN 의 일련 체크 작업

	eSesColdbootProcess_Etc1						= 0x00000002,
	eSesColdbootProcess_Etc2						= 0x00000004,
	eSesColdbootProcess_Etc3						= 0x00000008,
	eSesColdbootProcess_Etc4						= 0x00000010,

	/**  ............. */

	eSesColdbootProcess_EpgSave						= 0x00010000,
	eSesColdbootProcess_TvtvLoad					= 0x00020000,
	eSesColdbootProcess_Max,
} SesColdBoot_Process_t;


typedef struct tagSesColdbootContents
{
	HUINT32				ulProcessMask;

} SesColdBoot_Contents_t;

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
STATIC SesColdBoot_Contents_t		s_stColdBoot_Contents;

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */
#define _____Static_Function_ProtoType_____
static INLINE SesColdBoot_Contents_t *xmgr_coldboot_GetContents_Ses (void);
static INLINE HBOOL xmgr_coldboot_checkAllProcessFinished_Ses (SesColdBoot_Contents_t *pstContents);
static HERROR xmgr_coldboot_getNextProcess_Ses (SesColdBoot_Contents_t *pstContents, SesColdBoot_Process_t *peNextProcess);
static HERROR xmgr_coldboot_resetProcessFlag_Ses (SesColdBoot_Contents_t *pstContents, SesColdBoot_Process_t eProcess);
static HERROR xmgr_coldboot_startBootProcess_Ses (SesColdBoot_Contents_t *pstContents);
static BUS_Result_t xmgr_coldboot_Create_Ses (SesColdBoot_Contents_t *pstContents);
static BUS_Result_t xmgr_coldboot_GoToNextProcess_Ses (SesColdBoot_Contents_t *pstContents);


/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */
#define _____Static_Function_____

/**
 * @description     get SES Coldboot contents
 * @param     	void
 * @return     	SES Coldboot content variable.
 */
static INLINE SesColdBoot_Contents_t *xmgr_coldboot_GetContents_Ses (void)
{
	return &s_stColdBoot_Contents;
}

#define _____MANAGE_PROCESSES_FUNCTIONS_____

/**
 * @description     check all process is finished.
 * @param     	SES Coldboot content
 * @return     	if process mask is 0 -> ERR_OK, else ERR_FAIL
 */
static INLINE HBOOL xmgr_coldboot_checkAllProcessFinished_Ses (SesColdBoot_Contents_t *pstContents)
{
	return (pstContents->ulProcessMask == 0) ? TRUE : FALSE;
}


/**
 * @description     get next process of coldboot. but icordpro have one step for SES Boot process.
 * @param     	SES Coldboot content
 * @param     	pointer for next process
 * @return     	if success -> ERR_OK, else ERR_FAIL
 */
static HERROR xmgr_coldboot_getNextProcess_Ses (SesColdBoot_Contents_t *pstContents, SesColdBoot_Process_t *peNextProcess)
{
	SesColdBoot_Process_t	 eProcess;
	HUINT32					 ulIndex;

	if (peNextProcess == NULL)			{ return ERR_BUS_TARGET_NULL; }

	for (ulIndex = 0, eProcess = 1; ulIndex < 32; ulIndex++, eProcess <<= 1)
	{
		if (pstContents->ulProcessMask & eProcess)
		{
			*peNextProcess = eProcess;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

/**
 * @description     remove process mask.
 * @param     	SES Coldboot content
 * @param     	pointer for next process
 * @return     	ERR_OK
 */
static HERROR xmgr_coldboot_resetProcessFlag_Ses (SesColdBoot_Contents_t *pstContents, SesColdBoot_Process_t eProcess)
{
	HUINT32		 ulValue;

	ulValue = (HUINT32)eProcess;
	pstContents->ulProcessMask &= ~ulValue;
	return ERR_OK;
}


/**
 * @description     Start SES Boot process
 * @param     	SES Coldboot content
 * @return     	if start Boot process, return ERR_OK, else ERR_FAIL
 */
static HERROR xmgr_coldboot_startBootProcess_Ses (SesColdBoot_Contents_t *pstContents)
{
	HBOOL			 bBootProcess = TRUE;
//	HINT32			 nValue;
	WakeUpReason_t	 eWakeupReason;
	HERROR			 hErr;

	hErr = SVC_SYS_GetWakeUpReason (&eWakeupReason);

	if (hErr != ERR_OK)
	{
		eWakeupReason = eWAKEUP_BY_ACPOWER;
	}

	// SES Boot Procedure 를 반드시 해야 하는 Reason
	// eWAKEUP_BY_ACPOWER : 완전히 Power Down 된 상황. Micom Time조차 Reset되었다
	// eWAKEUP_BY_REBOOT : APS Boot Procedure를 Standby에서 하고 왔을 수가 없다.

	// APS Boot Procedure 를 반드시 하지 말아야 하는 Reason
	// eWAKEUP_BY_TIMER : Boot Procedure를 할 '시간'이 없다. 반드시 Skip 해야 한다.

	if (eWakeupReason == eWAKEUP_BY_ACPOWER || eWakeupReason == eWAKEUP_BY_REBOOT)
	{
		bBootProcess = TRUE;
	}
	else if (eWakeupReason == eWAKEUP_BY_TIMER)
	{
		bBootProcess = FALSE;
	}
	// TODO: DB에 저장된 STANDBYPROCESS value를 읽어 bBootProcess 를 결정하고 DB Value를 reset하는 코드의 필요성 검토 및 추가.
	/*
	else
	{
		hErr = DB_SETUP_GetMenuItemValue (MENUCFG_ITEMNAME_APSHD_STANDBYPROCESS, &nValue);
		if (hErr != ERR_OK)			{ nValue = FALSE; }

		bBootProcess = (HBOOL)(nValue == TRUE) ? FALSE : TRUE;
	}

	// Reset the flag :
	nValue = FALSE;
	DB_SETUP_SetMenuItemValue (MENUCFG_ITEMNAME_APSHD_STANDBYPROCESS, nValue);
	DB_SETUP_SyncMenuItem (MENUCFG_ITEMNAME_APSHD_STANDBYPROCESS);
	*/

	if (bBootProcess == TRUE)
	{
		BUS_MGR_Create("xmgr_coldboot_SesBoot", APP_ACTION_MGR_PRIORITY, xproc_coldboot_SesBoot, 0, 0, 0, 0);
		return ERR_OK;
	}

	// BOOT Process를 실행하지 않았다 : ERR_FAIL로 Skip한다.
	return ERR_FAIL;
}

#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

//eMEVT_BUS_CREATE
/**
 * @description	handle eMEVT_BUS_CREATE. init process mask and send eMEVT_COLDBOOT_NEXTPROCESS
 *				process is set on step (eSesColdbootProcess_SesBoot). but it is necessary more process, you can add ulProcessMask.
 * @param     	SES Coldboot content
 * @return     	MESSAGE_BREAK
 */
static BUS_Result_t xmgr_coldboot_Create_Ses (SesColdBoot_Contents_t *pstContents)
{
//	HUINT32				 ulActionId;

	HxSTD_memset (pstContents, 0, sizeof(SesColdBoot_Contents_t));

	// Spec 별로 process masking을 한다.
	pstContents->ulProcessMask |= eSesColdbootProcess_SesBoot;
	//	pstContents->ulProcessMask |= eApsColdbootProcess_EpgSave;

	XMGR_SEARCH_INFO_SetSesHdFuncMode (eSesHdFuncMode_ColdBoot);

	// Execute the next process
	BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);

	return MESSAGE_BREAK;
}


// MSG_APP_SHUTDOWN_NEXTPROCESS
/**
 * @description	handle MSG_APP_SHUTDOWN_NEXTPROCESS. check next process mask and send start process.
 				if next process is not exist, send eMEVT_COLDBOOT_NOTIFY_DONE.
 * @param     	SES Coldboot content
 * @return     	MESSAGE_BREAK
 */
static BUS_Result_t xmgr_coldboot_GoToNextProcess_Ses (SesColdBoot_Contents_t *pstContents)
{
	SesColdBoot_Process_t	eNextProcess;
	HERROR	 				hErr = ERR_OK;

	hErr = xmgr_coldboot_getNextProcess_Ses (pstContents, &eNextProcess);
	if (hErr != ERR_OK)
	{
		// Next Process가 없다 : 작업을 끝낸다.
		BUS_PostMessage(NULL, eMEVT_COLDBOOT_NOTIFY_DONE, 0, 0, 0, 0);
		return MESSAGE_BREAK;
	}

	HxLOG_Print("[xmgr_coldboot_GoToNextProcess_Ses] Next Process : (0x%08x) \n", eNextProcess);

	switch ( eNextProcess )
	{
		case eSesColdbootProcess_SesBoot:
			hErr = xmgr_coldboot_startBootProcess_Ses (pstContents);
			break;
		default:
			break;
	}

	if(hErr != ERR_OK)
	{
		xmgr_coldboot_resetProcessFlag_Ses(pstContents, eNextProcess);

		if(xmgr_coldboot_checkAllProcessFinished_Ses(pstContents) == TRUE)
			BUS_PostMessage(NULL, eMEVT_COLDBOOT_NOTIFY_DONE, 0, 0, 0, 0);
		else
			BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);
	}

	return MESSAGE_BREAK;

}

#define _____Module_Function_____

/**
 * @description	SES Coldboot Proc
 * @param     	message, action handle, param 1,2,3
 * @return     	MESSAGE_BREAK
 */
BUS_Result_t xproc_coldboot_Ses(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t 			eRes;
	SesColdBoot_Contents_t	*pstContents;

	pstContents = xmgr_coldboot_GetContents_Ses();
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print("\n\n\t############ SES Cold Boot Processing Start !!! ##############\n");
			HxLOG_Print("[eMEVT_BUS_CREATE] 0x%08x, (%08x:%08x:%08x)\n", hAct, p1, p2, p3);
			eRes = xmgr_coldboot_Create_Ses(pstContents);
			break;

		case eMEVT_COLDBOOT_NEXTPROCESS:
			HxLOG_Print("[eMEVT_COLDBOOT_NEXTPROCESS] 0x%08x, (%08x:%08x:%08x)\n", hAct, p1, p2, p3);
			eRes = xmgr_coldboot_GoToNextProcess_Ses (pstContents);
			break;

		case eMEVT_COLDBOOT_NOTIFY_DONE:
			HxLOG_Print("[eMEVT_COLDBOOT_NOTIFY_DONE] 0x%08x, (%08x:%08x:%08x)\n", hAct, p1, p2, p3);
			BUS_MGR_Destroy(NULL);
			eRes = MESSAGE_PASS;
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print("[eMEVT_BUS_DESTROY] 0x%08x, (%08x:%08x:%08x)\n", hAct, p1, p2, p3);
			XMGR_SEARCH_INFO_SetSesHdFuncMode (eSesHdFuncMode_Normal);
			xproc_coldboot_Base(message, hAct, p1, p2, p3);
			return MESSAGE_BREAK;

		default:
			//	Warning : PAMA Message processed in coldboot base module
			eRes = xproc_coldboot_Base(message, hAct, p1, p2, p3);
			break;
	}
	return eRes;
}

HERROR	xmgr_coldboot_Start_Ses(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("xmgr_coldboot_ses", APP_ACTION_MGR_PRIORITY, xproc_coldboot_Ses, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;

}

#endif
/* End of File. ---------------------------------------------------------- */


