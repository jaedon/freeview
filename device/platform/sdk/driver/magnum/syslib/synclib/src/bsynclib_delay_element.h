/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_delay_element.h $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 3/19/10 2:43p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_delay_element.h $
* 
* Hydra_Software_Devel/6   3/19/10 2:43p bandrews
* SW7405-3774: mute control merge
* 
* Hydra_Software_Devel/SW7405-3774/2   3/18/10 9:34p bandrews
* SW7405-4042: rework delay estimation so that imprecise lipsync works
* correctly
* 
* Hydra_Software_Devel/SW7405-3774/1   2/17/10 6:17p bandrews
* SW7405-3774: attempt to fix early audio unmute then mute
* 
* Hydra_Software_Devel/5   12/10/09 9:18p bandrews
* SW7401-3634: adding PWC (now JTI) support to synclib
* 
* Hydra_Software_Devel/4   7/24/09 4:14p bandrews
* PR52812: Added any-time source or display rate change support
* 
* Hydra_Software_Devel/3   5/12/09 4:40p bandrews
* PR48566: preferred units
* 
* Hydra_Software_Devel/2   2/25/09 7:53p bandrews
* PR52514: needed another const
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/4   2/22/08 3:06p bandrews
* PR37951: Debugging callbacks
* 
* Hydra_Software_Devel/3   2/20/08 10:03p bandrews
* PR37951: Updated based on feedback from usage
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:53p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bstd.h"
#include "bsynclib.h"

#ifndef BSYNCLIB_DELAY_ELEMENT_H__
#define BSYNCLIB_DELAY_ELEMENT_H__

/*
Summary:
*/
typedef struct
{
	bool bValid;
	bool bTimedOut;
	BSYNClib_Units ePreferredUnits;
	BSYNClib_Units eOriginalUnits;
	unsigned int uiMeasured;
	unsigned int uiCustom;
	unsigned int uiCapacity;
	unsigned int uiQuantizationLevel;
} BSYNClib_Delay_Data;

/*
Summary:
*/
typedef struct
{
	bool bAccepted;
	bool bEstimated;
	bool bGenerateCallback;
	unsigned int uiApplied;
	unsigned int uiDesired;
} BSYNClib_Delay_Results;

/*
Summary:
*/
typedef struct
{
	BSYNClib_Delay_Data sData;
	BSYNClib_Delay_Data sSnapshot;
	BSYNClib_Delay_Results sResults;
} BSYNClib_Delay;

/*
Summary:
*/
typedef struct
{
	bool bEnabled; /* TODO: is this being moved to NULLing out the callback function pointer? */
	bool bReceived;
#if 0
	unsigned long ulTime;
#endif
	BSYNClib_Units ePreferredUnits;
} BSYNClib_DelayNotification_Data;

/*
Summary:
*/
typedef struct
{
	bool bEnabled;
	BSYNClib_UnsignedValue sThreshold;
	bool bGenerateCallback;
} BSYNClib_DelayNotification_Results;

/*
Summary:
*/
typedef struct
{
	BSYNClib_DelayNotification_Data sData;
	BSYNClib_DelayNotification_Data sSnapshot;
	BSYNClib_DelayNotification_Results sResults;
} BSYNClib_DelayNotification;

/*
Summary:
*/
typedef enum
{
	BSYNClib_DelayElement_LifecycleEvent_eNone,
	BSYNClib_DelayElement_LifecycleEvent_eStarted,
	BSYNClib_DelayElement_LifecycleEvent_eStopped
} BSYNClib_DelayElement_LifecycleEvent;

/*
Summary:
*/
typedef enum
{
	BSYNClib_DelayElement_SynchronizationEvent_eNone,
	BSYNClib_DelayElement_SynchronizationEvent_eSynchronized,
	BSYNClib_DelayElement_SynchronizationEvent_eIgnored
} BSYNClib_DelayElement_SynchronizationEvent;

#ifdef BDBG_DEBUG_BUILD
extern const char * const BSYNClib_DelayElement_LifecycleEventNames[];
extern const char * const BSYNClib_DelayElement_SynchronizationEventNames[];
#endif

/*
Summary:
*/
typedef struct
{
	bool bStarted;
	bool bSynchronize;
} BSYNClib_DelayElement_Data;

/*
Summary:
*/
typedef struct
{
	bool bChanged;
	BSYNClib_DelayElement_LifecycleEvent eLifecycleEvent;
	BSYNClib_DelayElement_SynchronizationEvent eSynchronizationEvent;
	bool bDelayReceived;
	bool bGenerateDelayCallback;
	bool bGenerateNotificationCallback;
} BSYNClib_DelayElement_DiffResults;

/*
Summary:
*/
typedef struct
{
	unsigned int uiIndex;
	BSYNClib_Channel_Handle hParent;

	BSYNClib_Delay sDelay;
	BSYNClib_DelayNotification sNotification;

	BSYNClib_DelayElement_Data sData;
	BSYNClib_DelayElement_Data sSnapshot;
} BSYNClib_DelayElement;

/*
Summary:
*/
void BSYNClib_DelayElement_Init(BSYNClib_DelayElement * psElement);

/*
Summary:
*/
void BSYNClib_DelayElement_Reset_isr(BSYNClib_DelayElement * psElement);

/*
Summary:
*/
void BSYNClib_DelayElement_Snapshot_isr(BSYNClib_DelayElement * psElement);

/*
Summary:
*/
void BSYNClib_Delay_Init(BSYNClib_Delay * psDelay);

/*
Summary:
*/
void BSYNClib_Delay_Reset_isr(BSYNClib_Delay * psDelay);

/*
Summary:
*/
void BSYNClib_Delay_Snapshot_isr(BSYNClib_Delay * psDelay);

/*
Summary:
*/
void BSYNClib_DelayNotification_Init(BSYNClib_DelayNotification * psNotification);

/*
Summary:
*/
void BSYNClib_DelayNotification_Reset_isr(BSYNClib_DelayNotification * psNotification);

/*
Summary:
*/
void BSYNClib_DelayNotification_Snapshot_isr(BSYNClib_DelayNotification * psNotification);

void BSYNClib_DelayElement_CheckLifecycle_isr(
	bool bStarted,
	const BSYNClib_DelayElement * psCurrent,
	BSYNClib_DelayElement_DiffResults * psResults
);

void BSYNClib_DelayElement_Diff_isr(
	const BSYNClib_DelayElement * psDesired,
	const BSYNClib_DelayElement * psCurrent,
	BSYNClib_DelayElement_DiffResults * psResults
);

void BSYNClib_DelayElement_Patch_isr(
	const BSYNClib_DelayElement * psDesired,
	BSYNClib_DelayElement * psCurrent,
	BSYNClib_DelayElement_DiffResults * psResults
);

#endif /* BSYNCLIB_DELAY_ELEMENT_H__ */

