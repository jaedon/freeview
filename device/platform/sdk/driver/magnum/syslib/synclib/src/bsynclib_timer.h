/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_timer.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/24/08 3:09p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_timer.h $
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:54p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bstd.h"
#include "bsyslib.h"
#include "bsynclib.h"

#ifndef BSYNCLIB_TIMER_H__
#define BSYNCLIB_TIMER_H__

typedef struct
{
	BSYNClib_Channel_Handle hOwner;
	bool bScheduled; /* is the timer scheduled? */
	BSYSlib_Timer_Handle hTimer;
} BSYNClib_Timer;

void BSYNClib_Timer_Init(BSYNClib_Timer * psTimer);
void BSYNClib_Timer_Reset_isr(BSYNClib_Timer * psTimer);

#endif /* BSYNCLIB_TIMER_H__ */

