/**
@file     pmgr_epgbgtracking.c
@brief

Description:  \n
Module: \n
Remarks : \n

Copyright (c) 2009 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

#include <bus.h>

#include <mgr_epgbgtracking.h>
#include "./local_include/_xmgr_epgbgtracking.h"

HERROR	pmgr_epgbgtracking_Start(void)
{
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME_ACCURATE_RECORD)
	return xmgr_epgbgtracking_Start_Base();
#else
	return ERR_FAIL;
#endif
}

HERROR	pmgr_epgbgtracking_Stop(void)
{
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME_ACCURATE_RECORD)
	return xmgr_epgbgtracking_Stop_Base();
#else
	return ERR_FAIL;
#endif
}

// for batch
BUS_Callback_t		pmgr_epgbgtracking_GetProc(void)
{
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME_ACCURATE_RECORD)
	return xmgr_epgbgtracking_GetProc_Base();
#else
	return xmgr_epgbgtracking_GetProc_Base();
#endif
}

