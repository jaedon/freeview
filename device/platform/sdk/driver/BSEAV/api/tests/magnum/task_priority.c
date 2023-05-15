/***************************************************************************
 *     Copyright (c) 2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: task_priority.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 12/29/06 4:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/magnum/task_priority.c $
 * 
 * 2   12/29/06 4:44p erickson
 * PR26813: b_start_task now requires b_task_params, which should be
 * initialized with b_task_params_init
 * 
 * 1   11/1/06 11:36a erickson
 * PR25398: added usermode task priority test
 * 
 *********************************************************************/
#include "bsettop_board.h"
#include "bsettop_os.h"
#include <bkni.h>
#include <bkni_multi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*=***************
Modify bsettop_linux.c. The default impl of b_start_task does not honor
params.priority. See comment in code.

Build instructions:
make APP=task_priority -f Makefile.bsp

Run instructions:
settop task_priority [-l LOOPS]
*****************/

int loops = 1000000;
BKNI_EventHandle event1, event2;
int done = 0;

void *task_func(void *data)
{
	int i;
	BKNI_EventHandle event = (BKNI_EventHandle)data;
	for (i=0;i<loops;i++) {
	}
	printf("task %p done\n", data);
	BKNI_SetEvent(event);
	return NULL;
}

int main(int argc, char **argv)
{
	bresult result;
	b_task_t task1, task2;
	b_task_params task_params;
	int curarg = 1;

	/* process cmdline */
	while (curarg < argc) {
		if (!strcmp(argv[curarg], "-l") && curarg+1 < argc) {
			loops = atoi(argv[++curarg]);
		}
		curarg++;
	}	
	
	
	bsettop_boot_board();
	
	BKNI_CreateEvent(&event1);
	BKNI_CreateEvent(&event2);

	printf("high is task %p, low is task %p\n", (void*)event1, (void*)event2);
	printf("NOTE: high task should finish before low task\n");
		
	b_task_params_init(&task_params);
	
	task_params.priority = 0; /* highest */
	result = b_start_task(&task1, &task_params, task_func, event1);
	if (result) return -1;
	
	task_params.priority = 100; /* lowest */
	result = b_start_task(&task2, &task_params, task_func, event2);
	if (result) return -1;
	
	BKNI_WaitForEvent(event1, BKNI_INFINITE);
	BKNI_WaitForEvent(event2, BKNI_INFINITE);

	b_stop_task(task1);
	b_stop_task(task2);
	BKNI_DestroyEvent(event1);
	BKNI_DestroyEvent(event2);
		
	return 0;
}
