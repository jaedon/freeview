/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_state_machine.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/24/08 3:09p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_state_machine.h $
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/1   12/12/07 2:54p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bsyslib.h"

#ifndef BSYNCLIB_STATE_MACHINE_H__
#define BSYNCLIB_STATE_MACHINE_H__

/*
Summary:
*/
typedef struct BSYNClib_StateMachine_Impl * BSYNClib_StateMachine_Handle;

/*
Summary:
*/
typedef enum
{
	BSYNClib_StateMachine_State_eStopped,
	BSYNClib_StateMachine_State_eAcquire,
	BSYNClib_StateMachine_State_eSync,
	BSYNClib_StateMachine_State_eTrack
} BSYNClib_StateMachine_State;

/*
Summary:
*/
typedef struct
{
	BSYNClib_StateMachine_State eState;
} BSYNClib_StateMachine_Status;

/*
Summary:
*/
typedef BERR_Code (*BSYNClib_StateMachine_StateChange)
(
	void * pvParm1,
	int iParm2,
	BSYNClib_StateMachine_State eState
);

/*
Summary:
*/
typedef struct
{
	BSYNClib_StateMachine_StateChange pfStateChange;
	void * pvParm1;
	int iParm2;
} BSYNClib_StateMachine_StateChangeCallback;

/*
Summary:
*/
typedef struct
{
	BSYNClib_StateMachine_StateChangeCallback cbStateChange;
} BSYNClib_StateMachine_Settings;

/*
Summary:
*/
typedef enum
{
	BSYNClib_StateMachine_Signal_eStart,
	BSYNClib_StateMachine_Signal_eCheckPassed,
	BSYNClib_StateMachine_Signal_eCheckFailed,
	BSYNClib_StateMachine_Signal_eDataChanged,
	BSYNClib_StateMachine_Signal_eDelayApplied,
	BSYNClib_StateMachine_Signal_eStop
} BSYNClib_StateMachine_Signal;

/*
Summary:
Gets the default settings
*/
BERR_Code BSYNClib_StateMachine_GetDefaultSettings(
	BSYNClib_StateMachine_Settings * psSettings /* [out] */
);

/*
Summary:
Opens a state machine handle
*/
BERR_Code BSYNClib_StateMachine_Open(
	const BSYNClib_StateMachine_Settings * psSettings,
	BSYNClib_StateMachine_Handle * phMachine
);

/*
Summary:
Closes a state machine handle
*/
BERR_Code BSYNClib_StateMachine_Close(
	BSYNClib_StateMachine_Handle hMachine
);

/*
Summary:
*/
BERR_Code BSYNClib_StateMachine_SendSignal(
	BSYNClib_StateMachine_Handle hMachine,
	BSYNClib_StateMachine_Signal eSignal
);

/*
Summary:
*/
BERR_Code BSYNClib_StateMachine_GetStatus(
	const BSYNClib_StateMachine_Handle hMachine,
	BSYNClib_StateMachine_Status * psStatus
);

#endif /* BSYNCLIB_STATE_MACHINE_H__ */

