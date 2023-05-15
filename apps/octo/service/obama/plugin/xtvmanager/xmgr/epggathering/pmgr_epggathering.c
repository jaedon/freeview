/**
@file     pmgr_epggathering.c
@brief

Description:  \n
Module: \n
Remarks : \n

Copyright (c) 2009 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

#include <bus.h>

#include <mgr_epggathering.h>
#include "./local_include/_xmgr_epggathering.h"

HERROR	pmgr_epggathering_Start(void)
{
#if defined(CONFIG_OP_AUS_DTT)
	return xmgr_epggathering_Start_Base();
#elif defined(CONFIG_MW_EPG_GATHERING_BARKER)
	return xmgr_epggathering_Start_BgBarker();
#else
	HxLOG_Warning("You need to define epg gathering spec. \n");
	return xmgr_epggathering_Start_Base();
#endif
}

HERROR	pmgr_epggathering_Stop(void)
{
#if defined(CONFIG_OP_AUS_DTT)
	return xmgr_epggathering_Stop_Base();
#elif defined(CONFIG_MW_EPG_GATHERING_BARKER)
	return xmgr_epggathering_Stop_BgBarker();
#else
	HxLOG_Warning("You need to define epg gathering spec. \n");
	return xmgr_epggathering_Stop_Base();
#endif
}

// for batch
BUS_Callback_t		pmgr_epggathering_GetProc(void)
{
#if defined(CONFIG_OP_AUS_DTT)
	return xmgr_epggathering_GetProc_Base();
#elif defined(CONFIG_MW_EPG_GATHERING_BARKER)
	return xmgr_epggathering_GetProc_BgBarker();
#else
	HxLOG_Warning("You need to define epg gathering spec. \n");
	return xmgr_epggathering_GetProc_Base();
#endif
}

