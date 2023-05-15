/**
@file     pmgr_setting.c
@brief

Description:			\n
Module :				\n
Remarks :				\n

Copyright (c) 2013 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

#include <bus.h>

#include "./local_include/_xmgr_setting.h"

HERROR pmgr_setting_Start(void)
{
#if defined(CONFIG_OP_FREESAT)
	return xmgr_setting_Start_Freesat();
#else
	return xmgr_setting_Start_Base();
#endif
}

HERROR	pmgr_setting_ResetUserInfo (void)
{
	HERROR		 hErr = ERR_OK;

	return hErr;
}

HERROR	pmgr_setting_ExportDb (HBOOL bSync)
{
	HERROR	hError = ERR_OK;

#if defined(CONFIG_OP_MIDDLE_EAST)
	hError = xmgr_setting_ExportDb_MeSat(bSync);
#endif

	return hError;
}

HERROR	pmgr_setting_ImportDb (HBOOL bSync)
{
	HERROR	hError = ERR_OK;

#if defined(CONFIG_OP_MIDDLE_EAST)
	hError = xmgr_setting_ImportDb_MeSat(bSync);
#endif

	return hError;
}



