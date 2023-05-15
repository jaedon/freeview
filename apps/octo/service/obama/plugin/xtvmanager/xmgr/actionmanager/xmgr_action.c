/****************************************************************************
*
* File Name   : manual_action_base.c
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

#include <svc_pipe.h>
#include <svc_sys.h>
#include <svc_resources.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_rscmap.h>
#include <mgr_action.h>

#include "./local_include/_xmgr_action.h"

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
STATIC HUINT32		s_ulMainViewId = eActionId_VIEW_0;
STATIC MgrAction_State_e	s_eAmState = eAmState_NORMAL;
STATIC MgrAction_SvcDB_e	s_SvcDBState = eSvcDBState_NotSet;	// NoSvcDB º°·Î check


/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */

#define	_______GLOBAL_API_______

HUINT32		xmgr_action_GetMainViewId(void)
{
	return s_ulMainViewId;
}

void		xmgr_action_SetMainViewId(HUINT32 ulViewId)
{
	s_ulMainViewId = ulViewId;
}

MgrAction_State_e	xmgr_action_GetState(void)
{
	return s_eAmState;
}

void		xmgr_action_SetState(MgrAction_State_e eState)
{
	s_eAmState = eState;
}
MgrAction_SvcDB_e		xmgr_action_GetSvcDbState(void)
{
	return s_SvcDBState;
}

void		xmgr_action_SetSvcDbState(MgrAction_SvcDB_e eSvcDB)
{
	s_SvcDBState = eSvcDB;
}

/*		Action Normal
 */
BUS_Callback_t xmgr_action_NormalGetProc (void)
{
	BUS_Callback_t		pfnAction	= NULL;

	pfnAction = xproc_action_Normal_Base;

	return pfnAction;
}


/*		Action Manual
 */

HERROR	xmgr_action_GetLocalTime (UNIXTIME *pulLocalTime)
{
	HERROR ulRet = 0;
	HULONG ulSysTime = 0;

	if (pulLocalTime == NULL)
		return ERR_FAIL;

	ulRet = VK_CLOCK_GetTime (&ulSysTime);
	if (ulRet != ERR_OK)
	{
		HxLOG_Info("VK_CLOCK_GetTime Error:0x%X\n", ulRet);
		return ERR_FAIL;
	}

	*pulLocalTime = (UNIXTIME) ulSysTime;

	return ERR_OK;
}

void xmgr_action_PrintDateTime (UNIXTIME ulTime)
{
	HxDATETIME_t stTime;

	HLIB_DATETIME_ConvertUnixTimeToDateTime (ulTime, &stTime);

	HxLOG_Info("      (%04d/%02d/%02d  %02d:%02d:%02d)\n",
			stTime.stDate.usYear, stTime.stDate.ucMonth, stTime.stDate.ucDay,
			stTime.stTime.ucHour, stTime.stTime.ucMinute, stTime.stTime.ucSecond);
}

void xmgr_action_PrintDuration (UNIXTIME ulStartTime, UNIXTIME ulEndTime)
{
	HxDATETIME_t stStart, stEnd;

	HLIB_DATETIME_ConvertUnixTimeToDateTime (ulStartTime, &stStart);
	HLIB_DATETIME_ConvertUnixTimeToDateTime (ulEndTime, &stEnd);

	HxLOG_Info("      (%04d/%02d/%02d  %02d:%02d:%02d  ~  %04d/%02d/%02d  %02d:%02d:%02d)\n",
			stStart.stDate.usYear, stStart.stDate.ucMonth, stStart.stDate.ucDay,
			stStart.stTime.ucHour, stStart.stTime.ucMinute, stStart.stTime.ucSecond,
			stEnd.stDate.usYear, stEnd.stDate.ucMonth, stEnd.stDate.ucDay,
			stEnd.stTime.ucHour, stEnd.stTime.ucMinute, stEnd.stTime.ucSecond);
}

HERROR xmgr_action_GetSiTableTimeout(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	HERROR	hErr = ERR_FAIL;

	// Check Priority Step1 : CAS
#if defined(CONFIG_MW_CAS_IRDETO)
	hErr = xmgr_action_SiGetDefaultTimeout_Irdeto(eTableType, pulTimeout);
#endif
	if (ERR_OK == hErr)
		return ERR_OK;

	// Check Priority Step2 : Country, Region, Modulation
	// TODO: Get Country, Region or Modulation from hAction

	// Check Priority Step3 : Operator
	if (ERR_OK != hErr)
	{
#if defined(CONFIG_OP_MIDDLE_EAST)
		hErr = xmgr_action_SiGetDefaultTimeout_Mesat(eTableType, pulTimeout);
#elif defined(CONFIG_OP_ZIGGO)
		hErr = xmgr_action_SiGetDefaultTimeout_Ziggo(eTableType, pulTimeout);
#elif defined(CONFIG_OP_MOSTELECOM)
		hErr = xmgr_action_SiGetDefaultTimeout_Mostelecom(eTableType, pulTimeout);
#elif defined(CONFIG_OP_RUSSIA_TVT)
		hErr = xmgr_action_SiGetDefaultTimeout_Tvt(eTableType, pulTimeout);
#elif defined(CONFIG_OP_NTVPLUS)
		hErr = xmgr_action_SiGetDefaultTimeout_NtvPlus(eTableType, pulTimeout);
#elif defined(CONFIG_OP_UK_DTT)
		hErr = xmgr_action_SiGetDefaultTimeout_UkDtt(eTableType, pulTimeout);
#elif defined(CONFIG_OP_JAPAN)
		hErr = xmgr_action_SiGetDefaultTimeout_Japan(eTableType, pulTimeout);
#endif
		if (ERR_OK == hErr)
			return ERR_OK;
	}

	// Check Priority Step3 : Default
	if (ERR_OK != hErr)
	{
		hErr = xmgr_action_SiGetDefaultTimeout_Base(eTableType, pulTimeout);
	}

	return hErr;
}

