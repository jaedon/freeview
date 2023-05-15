/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bastmlib_clock_reference.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 7/14/10 7:44p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/astmlib/noarch/bastmlib_clock_reference.h $
* 
* Hydra_Software_Devel/3   7/14/10 7:44p bandrews
* SW3548-1161: expose default configs as public
* 
* Hydra_Software_Devel/2   6/25/09 5:49p bandrews
* PR53244: Add const to declaration
* 
* Hydra_Software_Devel/1   3/24/08 3:08p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/2   2/15/08 10:00p bandrews
* PR36148: Updated ASTM based on reviews
* 
* Hydra_Software_Devel/1   1/25/08 9:21p bandrews
* PR36148: Updated based on simulation
***************************************************************************/

#include "bstd.h"
#include "bastmlib.h"

#ifndef BASTMLIB_CLOCK_REFERENCE_H__
#define BASTMLIB_CLOCK_REFERENCE_H__

typedef struct BASTMlib_ClockReference_Impl * BASTMlib_ClockReference_Handle;

typedef struct BASTMlib_ClockReference_Config
{
	unsigned int uiMinimumTimeBetweenEvents; /* minimum time that must 
		elapse between receiving events, in ms. used with processing 
		frequency to determine size of event ring buffer. if events come 
		more often than this time, the event buffer may overflow, and newer
		events will be lost */
	BASTMlib_ClockRate eClockReferenceDomain; /* the clocking domain of 
		the clock reference time stamps */
	unsigned int uiDeviationThreshold; /* threshold above which abs value of 
		clock reference rate difference to STC will be considered deviant, 
		in eClockReferenceDomain ticks */
	unsigned int uiDeviantCountThreshold; /* how many deviant clock
		coupling measurements must be received before deeming the clock
		reference bad and decoupling the timebases from the input */
	unsigned int uiIdealCountThreshold;/* how many ideal clock
		coupling measurements must be received before deeming the clock
		reference good and recoupling the timebases to the input */
} BASTMlib_ClockReference_Config;

void BASTMlib_ClockReference_Create(
	BASTMlib_ClockReference_Handle * phReference
);

void BASTMlib_ClockReference_Destroy(
	BASTMlib_ClockReference_Handle hReference
);

void BASTMlib_ClockReference_Flush_isr(
    BASTMlib_ClockReference_Handle hReference
);

void BASTMlib_ClockReference_Reset_isr(
	BASTMlib_ClockReference_Handle hReference
);

void BASTMlib_ClockReference_GetDefaultConfig(
	BASTMlib_ClockReference_Config * psConfig
);

void BASTMlib_ClockReference_GetConfig(
	const BASTMlib_ClockReference_Handle hReference,
	BASTMlib_ClockReference_Config * psConfig
);

void BASTMlib_ClockReference_SetConfig(
	BASTMlib_ClockReference_Handle hReference,
	const BASTMlib_ClockReference_Config * psConfig
);

void BASTMlib_ClockReference_EventHandler_isr(
	BASTMlib_ClockReference_Handle hReference,
	const BASTMlib_ClockReference_Event * psEvent
	
);

void BASTMlib_ClockReference_ResizeEventQueue(
	BASTMlib_ClockReference_Handle hReference
);

#endif /* BASTMLIB_CLOCK_REFERENCE_H__ */

