/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bastmlib_presenter_priv.h $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 7/14/10 7:44p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/astmlib/noarch/bastmlib_presenter_priv.h $
* 
* Hydra_Software_Devel/4   7/14/10 7:44p bandrews
* SW3548-1161: expose default configs as public
* 
* Hydra_Software_Devel/3   11/21/08 8:35p bandrews
* PR49568: Fix presenter naming capability
* 
* Hydra_Software_Devel/2   4/9/08 2:31p bandrews
* PR41524: Removed erroneous instance declarations
* 
* Hydra_Software_Devel/1   3/24/08 3:08p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/1   2/15/08 10:00p bandrews
* PR36148: Updated ASTM based on reviews
***************************************************************************/

#include "bstd.h"
#include "bastmlib.h"

#ifndef BASTMLIB_PRESENTER_PRIV_H__
#define BASTMLIB_PRESENTER_PRIV_H__

/* TODO: tuning for these */
#define BASTMLIB_PRESENTER_P_DEFAULT_MIN_TIME_BETWEEN_EVENTS 10
#define BASTMLIB_PRESENTER_P_DEFAULT_EVENT_QUEUE_CAPACITY 500
#define BASTMLIB_PRESENTER_P_DEFAULT_PASS_EVENT_COUNT_THRESHOLD 5
#define BASTMLIB_PRESENTER_P_DEFAULT_FAIL_EVENT_COUNT_THRESHOLD 10

struct BASTMlib_Presenter_Impl
{
	BASTMlib_Handle hAstm;
	char * pcName;
	unsigned int uiId;

	BASTMlib_Presenter_Settings sSettings;
	BASTMlib_Presenter_Config sConfig;

	unsigned int uiMaximumAcquisitionTime;
	unsigned int uiPassEventCount;
	unsigned int uiFailEventCount;

	struct
	{
		BASTMlib_Presenter_Event * asEvents;
		unsigned int uiSize;
		unsigned int uiCapacity;
		unsigned int uiWrite;
		unsigned int uiRead;
	} sEventQueue;
};

#endif /* BASTMLIB_PRESENTER_PRIV_H__ */

