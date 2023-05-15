/***************************************************************************
*     Copyright (c) 2003-2007, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: memmgr.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 3/31/07 3:50p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/common/appframework/memmgr.h $
* 
* Hydra_Software_Devel/2   3/31/07 3:50p shyam
* PR 29285 : Added linuxclient support for simulations/emulation
* 
* Hydra_Software_Devel/1   6/16/06 4:00p maivu
* PR 22212: Move to common directory
* 
* Hydra_Software_Devel/2   7/27/05 3:55p jasonh
* PR 16397: Removed SetMemoryOffset function (done through #defines)
* 
* Hydra_Software_Devel/3   9/20/04 1:35p darnstein
* PR 10943: Add function to allow changing the heap starting address
* from 0x02000000 to anything else.  Use this for dual PCI board.
* 
* Hydra_Software_Devel/2   9/29/03 12:04p aram
* integrated with int1
***************************************************************************/
 
#ifndef _MEMMGR_H
#define _MEMMGR_H


#if __cplusplus
extern "C" {
#endif

int  memmgr_Init(int argc, char *argv[], BSystem_Info *pInfo);
int  memmgr_Done(void);

#if __cplusplus
}
#endif

#endif /* _MEMMGR_H */







