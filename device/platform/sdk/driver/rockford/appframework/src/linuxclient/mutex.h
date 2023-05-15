/***************************************************************************
*	  Copyright (c) 2003-2009, Broadcom Corporation
*	  All Rights Reserved
*	  Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: mutex.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 4/28/09 3:17p $
*
* Module Description: Emulator platform mutex API.
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/linuxclient/mutex.h $
* 
* Hydra_Software_Devel/2   4/28/09 3:17p jessem
* PR 54380: Changed all references to IKOS to EMULATION, EMU or BEMU.
* 
* Hydra_Software_Devel/1   10/16/07 3:27p jessem
* PR 36129: Initial version. Mutex API for IKOS-based platforms.
* 
* Hydra_Software_Devel/2   10/16/07 3:05p jessem
* PR 36129: Prepended function names with IKOS_Client.
* 
***************************************************************************/
#ifndef __BEMU_MUTEX_H__
#define __BEMU_MUTEX_H__

typedef struct BEMU_Client_MutexObj *BEMU_Client_MutexHandle;

int BEMU_Client_CreateMutex(BEMU_Client_MutexHandle *handle);
int BEMU_Client_TryAcquireMutex(BEMU_Client_MutexHandle handle);
int BEMU_Client_AcquireMutex(BEMU_Client_MutexHandle handle);
void BEMU_Client_ReleaseMutex(BEMU_Client_MutexHandle handle);
void BEMU_Client_DestroyMutex(BEMU_Client_MutexHandle handle);
int BEMU_Client_Sleep(unsigned int millisec);

#endif /* __BEMU_MUTEX_H__ */
