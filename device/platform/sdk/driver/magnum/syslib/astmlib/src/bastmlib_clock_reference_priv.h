/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bastmlib_clock_reference_priv.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 7/14/10 7:44p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/astmlib/noarch/bastmlib_clock_reference_priv.h $
* 
* Hydra_Software_Devel/3   7/14/10 7:44p bandrews
* SW3548-1161: expose default configs as public
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

#ifndef BASTMLIB_CLOCK_REFERENCE_PRIV_H__
#define BASTMLIB_CLOCK_REFERENCE_PRIV_H__

/* TODO: tuning for these */
#define BASTMLIB_CLOCK_REFERENCE_P_DEFAULT_MIN_TIME_BETWEEN_EVENTS 20 /* ms */
#define BASTMLIB_CLOCK_REFERENCE_P_DEFAULT_EVENT_QUEUE_CAPACITY 100
#define BASTMLIB_CLOCK_REFERENCE_P_DEFAULT_DEVIANT_COUNT_THRESHOLD 2
#define BASTMLIB_CLOCK_REFERENCE_P_DEFAULT_IDEAL_COUNT_THRESHOLD 2
#define BASTMLIB_CLOCK_REFERENCE_P_DEFAULT_DEVIATION_THRESHOLD 1 /* 1 ms */

struct BASTMlib_ClockReference_Impl
{
	BASTMlib_Handle hAstm;

	BASTMlib_ClockReference_Config sConfig;

	unsigned int uiMaximumAcquisitionTime;
	unsigned int uiDeviantCount;
	unsigned int uiIdealCount;
	long lAverageDeviation;

	struct
	{
		BASTMlib_ClockReference_Event * asEvents;
		unsigned int uiSize;
		unsigned int uiCapacity;
		unsigned int uiWrite;
		unsigned int uiRead;
	} sEventQueue;
};

#endif /* BASTMLIB_CLOCK_REFERENCE_PRIV_H__ */

