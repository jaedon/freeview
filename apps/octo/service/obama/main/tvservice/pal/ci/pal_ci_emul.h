/**
	@file	  pal_ci_emul.h
	@brief	  																					\n

	Description: 																				\n
	Module: PAL/CI/EMUL				 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.															\n
	All rights reserved.																		\n
*/


#ifndef	__PAL_CI_EMUL_H__
#define	__PAL_CI_EMUL_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "ci.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
HERROR EMUL_CI_CamState(CI_SLOT_ID nSlotId);
HERROR EMUL_CI_AppInfo(CI_SLOT_ID nSlotId);
HERROR EMUL_CI_CaSysId(CI_SLOT_ID nSlotId);
HERROR EMUL_CI_CasState(CI_SLOT_ID nSlotId);

HERROR EMUL_CI_ReqMmiOpen(CI_SLOT_ID nSlotId);
HERROR EMUL_CI_ReqMmiClose(CI_SLOT_ID nSlotId);
HERROR EMUL_CI_ReqMmiMenu(CI_SLOT_ID nSlotId);
HERROR EMUL_CI_ReqMmiList(CI_SLOT_ID nSlotId);
HERROR EMUL_CI_ReqMmiText(CI_SLOT_ID nSlotId);
HERROR EMUL_CI_ReqMmiEnq(CI_SLOT_ID nSlotId);


#endif	//__PAL_CI_EMUL_H__

