/**
@file     pmgr_liveota.c
@brief

Description:  \n
Module: \n
Remarks : \n

Copyright (c) 2009 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

#include <bus.h>

#include <mgr_liveota.h>
#include "./local_include/_xmgr_liveota.h"

HERROR	pmgr_liveota_Start(void)
{
#if defined(CONFIG_OTA_LIVE_ENABLE)
	return xmgr_liveota_Start();
#else
	return ERR_FAIL;
#endif
}

HERROR	pmgr_liveota_Stop(void)
{
#if defined(CONFIG_OTA_LIVE_ENABLE)
	return xmgr_liveota_Stop();
#else
	return ERR_FAIL;
#endif
}

BUS_Callback_t pmgr_liveota_GetProc(void)
{
#if defined(CONFIG_OTA_LIVE_ENABLE)
	return xmgr_liveota_GetProc();
#else
	return NULL;
#endif
}

