/****************************************************************************
*
* File Name   : batch_clockrecovery_mesat.c
*
* Description :
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
*
****************************************************************************/

#if 0

/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
/** @brief global header file  */
#include <octo_common.h>

#include <db_param.h>

#include <svc_sys.h>

#include <bus.h>

#include <mgr_action.h>
#include <mgr_swup.h>
#include <mgr_common.h>
#include <mgr_rscmap.h>
#include <mgr_time.h>


/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */
#define _____Static_Function_ProtoType_____

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */

#define _____Static_Function_____

#define _____Module_Function_____

// eMEVT_BUS_CREATE : MgrAction_InitCb_t
BUS_Result_t xmgr_action_BatchClockRecoveryProcessInit_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void **ppvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulUnixTime;

	if (NULL == pstStepControl)
	{
		HxLOG_Error("pstStepControl is NULL\n");
		return ERR_BUS_INITIAL_FAILED;
	}
	HxLOG_Debug("== (+) \n");



	if (SVC_SYS_GetRtcTime(&ulUnixTime) != ERR_OK)
	{
		HxLOG_Debug("START MGR_TIME_ClockRecoverStart \n");
		MGR_TIME_ClockRecoverStart();
	}
	else
	{
		HxLOG_Debug("Current Time is Valid !!!\n");
		return ERR_BUS_INITIAL_FAILED;
	}

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY : MgrAction_DestroyCb_t
HERROR xmgr_action_BatchClockRecoveryProcessDestory_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("xmgr_action_BatchClockRecoveryProcessDestory_MeSat....\n");
	return ERR_OK;
}

// eMEVT_BUS_MGR_KILLED : MgrAction_MgrKilledCb_t
HERROR xmgr_action_BatchClockRecoveryProcessMgrKilled_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("xmgr_action_BatchClockRecoveryProcessMgrKilled_MeSat.....\n");
	return ERR_OK;

}

// eMEVT_ACTION_SHUTDOWN_NEXTPROCESS : MgrAction_DoNextProcessCb_t
BUS_Result_t xmgr_action_BatchClockRecoveryProcessDoProcess_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("xmgr_action_BatchClockRecoveryProcessDoProcess_MeSat.....\n");
	return ERR_BUS_NO_ERROR;
}

// eMEVT_ACTION_SHUTDOWN_DESTROY : MgrAction_BatchDestroyCb_t
HERROR xmgr_action_BatchClockRecoveryProcessShutdownDestroy_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Error("Nothing to do....\n");

	return ERR_OK;
}

// User Defined Message : MgrAction_MsgProcessCb_t
BUS_Result_t xmgr_action_BatchClockRecoveryProcessMsg_MeSat(HINT32 nMsg, Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(nMsg)
	{

	default:
		break;
	}
	return ERR_BUS_NO_ERROR;
}
#endif
/* End of File. ---------------------------------------------------------- */



