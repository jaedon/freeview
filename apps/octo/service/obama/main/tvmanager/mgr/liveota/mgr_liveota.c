/**
	@file     mgr_liveota.c
	@brief    file_name & simple comment.

	Description:  \n
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n


	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_liveota.h>
#include <pmgr_liveota.h>

#include <svc_pipe.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <svc_si.h>

#include "local_include/_mgr_liveota.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Module internal		 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      	Global Function 	API    ********************/
/*******************************************************************/

HERROR	MGR_LIVEOTA_Start(void)
{
	return pmgr_liveota_Start();
}

HERROR	MGR_LIVEOTA_Stop(void)
{
	return pmgr_liveota_Stop();
}

BUS_Callback_t	MGR_LIVEOTA_GetProc(void)
{
	return pmgr_liveota_GetProc();
}

/*********************** End of File ******************************/
