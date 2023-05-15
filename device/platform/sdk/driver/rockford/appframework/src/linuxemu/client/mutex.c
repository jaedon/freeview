/***************************************************************************
 *	   Copyright (c) 2003-2009, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mutex.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 4/28/09 3:17p $
 *
 * Module Description:
 *	 The source code for the emulator mutex API.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/appframework/src/linuxclient/mutex.c $
 * 
 * Hydra_Software_Devel/2   4/28/09 3:17p jessem
 * PR 54380: Changed all references to IKOS to EMULATION, EMU or BEMU.
 * 
 * Hydra_Software_Devel/1   10/16/07 3:28p jessem
 * PR 36129: Initial version. Mutex API for IKOS-based platforms.
 ***************************************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "mutex.h"
#include "bstd.h"

struct BEMU_Client_MutexObj 
{
	pthread_mutex_t mutex;
};

int BEMU_Client_CreateMutex(BEMU_Client_MutexHandle *handle)
{

	*handle = (BEMU_Client_MutexHandle)malloc(sizeof(**handle));
	if (!*handle) {
		return 1;
	}
	if (pthread_mutex_init(&(*handle)->mutex, NULL)) {
		free(*handle);
		return 1;
	} else {
		return 0;
	}
}

int BEMU_Client_TryAcquireMutex(BEMU_Client_MutexHandle handle)
{
	int rc;


	rc = pthread_mutex_trylock(&handle->mutex);
	if (rc==0) {
		return 0;
	} else if (rc==EBUSY) {
		return 1;
	} else {
		return 2;
	}
}

int BEMU_Client_AcquireMutex(BEMU_Client_MutexHandle handle)
{

	if (pthread_mutex_lock(&handle->mutex))
		return 1;
	else
		return 0;
}

void BEMU_Client_ReleaseMutex(BEMU_Client_MutexHandle handle)
{

	if (pthread_mutex_unlock(&handle->mutex)) {
		fprintf(stderr, "pthread_mutex_unlock failed");
		assert(false);
	}
	return ;
}

void BEMU_Client_DestroyMutex(BEMU_Client_MutexHandle handle)
{

	pthread_mutex_destroy(&handle->mutex);
	free(handle);
	return ;
}

int BEMU_Client_Sleep(unsigned int millisec)
{
	struct timespec delay;
	struct timespec rem;
	int rc;

	delay.tv_sec = millisec/1000;
	delay.tv_nsec = 1000 * 1000 * (millisec%1000);

	for(;;) {
		rc = nanosleep(&delay, &rem); /* [u]sleep can't be used because it uses SIGALRM */
		if (rc!=0) {
			if (errno==EINTR) {
				delay = rem; /* sleep again */
				continue;
			}
			return 1;
		}
		break; /* done */
	}

	return 0;
}

