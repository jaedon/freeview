/****************************************************************************
*
* File Name   : xmgr_action_base.c
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


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
/** @brief global header file  */
#include <octo_common.h>

#include <db_param.h>
#include <svc_sys.h>

#include <bus.h>
#include <papi.h>

#include <mgr_common.h>
#include <mgr_rscmap.h>

#include <mgr_action.h>
#include <pmgr_action.h>

#include <mgr_live.h>

#include "../local_include/_xmgr_action.h"

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

#define _____Static_Function_ProtoType_____
#define _____Static_Function_____

#define _____Module_Function_____

HUINT32 xmgr_action_GetWakeupRcukey_Base(void)
{
	return (HUINT32)(eWAKEUP_RCUKEY_GROUP_NUMBER | eWAKEUP_RCUKEY_GROUP_CH_UP_DOWN);
}

HBOOL xmgr_action_CheckWaitShutdown_Base(void)
{
#if	defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
	MGR_ACTION_WaitShutDown(1000000);
#elif defined(CONFIG_MW_SYS_ACTIVESTBY_BY_PWRSAVINGONSTBY)
	HBOOL	bPowerSavingStandby = FALSE;

	if(PAPI_GetPowerSavingOnStandby(&bPowerSavingStandby) != ERR_OK)
	{
		HxLOG_Error("[%s] PAPI_GetPowerSavingOnStandby() Error\n",__FUNCTION__);
	}

	if(bPowerSavingStandby == FALSE)
	{
		MGR_ACTION_WaitShutDown(1000000);
	}
#endif
	return TRUE;
}


