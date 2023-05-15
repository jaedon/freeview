/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_state_machine_priv.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/24/08 3:09p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_state_machine_priv.h $
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/1   12/12/07 2:54p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bsynclib_state_machine.h"

#ifndef BSYNCLIB_STATE_MACHINE_PRIV_H__
#define BSYNCLIB_STATE_MACHINE_PRIV_H__

typedef struct
{
	BSYNClib_StateMachine_State eState;
} BSYNClib_StateMachine_Data;

/*
Summary:
*/
struct BSYNClib_StateMachine_Impl
{
	BSYNClib_StateMachine_Settings sSettings;
	BSYNClib_StateMachine_Data sData;
	BSYNClib_StateMachine_Status sStatus;
};

/*
Summary:
*/
BERR_Code BSYNClib_StateMachine_FireStateChangeCallback(
	BSYNClib_StateMachine_Handle hMachine
);

#endif /* BSYNCLIB_STATE_MACHINE_PRIV_H__ */

