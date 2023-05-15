/**
@file     pmgr_frontcontroal.c
@brief Installation wizard install option(simple/advanced) configuration file.

Description: snmp mib data set to DI \n
Module: Application(Layer), util(module) \n
Remarks : \n

Copyright (c) 2009 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

#include <bus.h>

#include "./local_include/_xmgr_frontcontrol.h"

HERROR	pmgr_frontcontrol_Start (void)
{
	return ERR_FAIL;
}

HERROR	pmgr_frontcontrol_GetConfiguration (HBOOL bPowerOnOff, HBOOL bPwrSaving, PanelDimmingLevel_t *peDimLevel, HBOOL *pbDisplayTime)
{
	return xmgr_frontcontrol_GetConfiguration_Base (bPowerOnOff, bPwrSaving, peDimLevel, pbDisplayTime);
}

