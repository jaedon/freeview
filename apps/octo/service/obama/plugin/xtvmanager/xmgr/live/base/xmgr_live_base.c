/**
	@file     xmgr_live_base.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/**
  * Character encoding.
  *
  * MS949
  *
  * This source file that uses MS949 encoding.
  * MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
  * MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
  * Quelldatei, die MS949-Codierung verwendet.
  *
**/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <mgr_common.h>
#include <db_param.h>
#include <db_svc.h>

#include <svc_ch.h>
#include <svc_si.h>
#include <svc_av.h>
#include <svc_cas.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_transcoder.h>
#include <vkernel.h>
#include <bus.h>
#include <get_enum_str.h>

#include <mgr_output.h>
#include <mgr_live.h>
#include <mgr_rscmap.h>

// Local/Member Header Files
#include <xmgr_live.h>
#include "../local_include/_xmgr_live.h"
#include "../local_include/_xmgr_live_view.h"
#include "../local_include/_xmgr_live_lock.h"
#include "../local_include/_xmgr_live_comp.h"

#define ___LOCAL_DEFINITIONS___
#define ___LOCAL_DATATYPES___
#define ___LOCAL_VARIABLES___

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


#define _____CONTEXT_FUNCTIONS_____


#define _____LOCAL_FUNCTION_____

#define _____API_FUNCTIONS___
HERROR xmgr_live_Init_Base(void)
{
	XMGR_LiveView_BASE_Init();
	XMGR_LiveLock_BASE_Init();
	XMGR_LiveComp_BASE_Init();

	return ERR_OK;
}

HERROR xmgr_live_ResetUiState_Base(HUINT32 ulActionId, HBOOL bSvcStart)
{
	XMGR_LiveView_BASE_ResetUiState(ulActionId, bSvcStart);
	XMGR_LiveLock_BASE_ResetUiState(ulActionId, bSvcStart);

	return ERR_OK;
}

HERROR xmgr_live_GetState_Base(HUINT32 ulActionId, MgrLive_ProcType_e eProcType, HUINT32 *pulUiState)
{
	HUINT32 	ulState = 0;
	HERROR		hErr = ERR_FAIL;

	switch (eProcType)
	{
		case eLiveProc_View:
			hErr = XMGR_LiveView_BASE_GetState(ulActionId, (MgrLiveView_UiState_e *)&ulState);
			break;

		case eLiveProc_Lock:
			hErr = XMGR_LiveLock_BASE_GetState(ulActionId, (MgrLiveLock_UiState_e *)&ulState);
			break;

		default:
			HxLOG_Error ("ulActionId(%d) invalid procType(%d) \n", ulActionId, eProcType);
			return ERR_FAIL;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error ("GetState() Error !! ulActionId(%d) eProcType(%d) \n", ulActionId, eProcType);
	}

	*pulUiState = ulState;

	return ERR_OK;
}

/* End Of File */
