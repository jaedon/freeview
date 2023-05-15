/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bastmlib_priv.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 7/17/09 6:56p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/astmlib/noarch/bastmlib_priv.h $
* 
* Hydra_Software_Devel/3   7/17/09 6:56p bandrews
* PR49215: playback support
* 
* Hydra_Software_Devel/2   6/5/09 5:32p bandrews
* PR52503: Move singleton counter/idgen to astmlib struct
* 
* Hydra_Software_Devel/1   3/24/08 3:08p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/3   2/15/08 10:00p bandrews
* PR36148: Updated ASTM based on reviews
* 
* Hydra_Software_Devel/2   1/25/08 9:22p bandrews
* PR36148: Updated based on simulation
* 
* Hydra_Software_Devel/1   10/19/07 9:30p bandrews
* PR36148: Initial implementation
***************************************************************************/

#include "bstd.h"
#include "bastmlib.h"
#include "bastmlib_presentation_task.h"
#include "bastmlib_clock_coupling_task.h"
#include "bsyslib.h"
#include "bsyslib_list.h"

#ifndef BASTMLIB_PRIV_H__
#define BASTMLIB_PRIV_H__

struct BASTMlib_Impl
{
	bool bEnabled;
	bool bStarted;

	BASTMlib_Handle hAstm;
	BASTMlib_Settings sSettings;
	BASTMlib_Config sConfig;
	BASTMlib_Status sStatus;

	unsigned int uiPresentersCreated; /* TODO: needs to be reworked with create/destroy add/remove presenter stuff */
	BASTMlib_PresentationTaskState sPresentation;
	BASTMlib_ClockCouplingTaskState sClockCoupling;
};

void BASTMlib_P_GetDefaultConfig(
	BASTMlib_Config * psConfig
);

BERR_Code BASTMlib_P_StartTimer_isr(
	BASTMlib_Handle hAstm,
	BSYSlib_Timer_Handle hTimer, 
	unsigned long ulTimeout,
	BSYSlib_Timer_ExpiryHandler pfTimerExpired,
	void * pvParm1,
	int iParm2
);

bool BASTMlib_P_Enabled(
	BASTMlib_Handle hAstm
);

bool BASTMlib_P_Enabled_isr(
	BASTMlib_Handle hAstm
);

#endif /* BASTMLIB_PRIV_H__ */

