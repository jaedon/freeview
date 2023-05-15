/**
	@file     	xmgr_frontcontrol_base.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_frontcontrol.h>
#include "../local_include/_xmgr_frontcontrol.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR	xmgr_frontcontrol_GetConfiguration_Base (HBOOL bPowerOnOff, HBOOL bPwrSaving, PanelDimmingLevel_t *peDimLevel, HBOOL *pbDisplayTime)
{
	// Power Off
	if (bPowerOnOff == FALSE)
	{
#if defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
		if (bPwrSaving == TRUE)
		{
			if (peDimLevel)		*peDimLevel = ePANEL_DIM_LevelOff;
			if (pbDisplayTime)	*pbDisplayTime = FALSE;
		}
		else
		{
			if (peDimLevel)		*peDimLevel = ePANEL_DIM_Level20;
			if (pbDisplayTime)	*pbDisplayTime = TRUE;
		}
#endif
	}
	else
	{
	}

	return ERR_OK;
}



/*********************** End of File ******************************/
