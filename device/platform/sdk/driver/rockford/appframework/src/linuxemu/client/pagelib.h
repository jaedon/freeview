/***************************************************************************
*	  Copyright (c) 2003-2012, Broadcom Corporation
*	  All Rights Reserved
*	  Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: pagelib.h $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 10/16/12 1:13p $
*
* Module Description: Emulator Paging API
*
* Revision History: 
*
* $brcm_Log: /rockford/appframework/src/linuxemu/client/pagelib.h $
* 
* Hydra_Software_Devel/5   10/16/12 1:13p hongtaoz
* SW7445-8: added 3rd memc;
* 
* Hydra_Software_Devel/4   6/18/12 5:20p chengs
* SW7584-5: Set NUM_OF_MEMC to 1 for 7584.
* 
* Hydra_Software_Devel/3   12/10/10 12:08p hongtaoz
* SW7425-10: added emu file utility to read/write between file and device
* memory;
* 
* Hydra_Software_Devel/2   10/15/10 3:09p hongtaoz
* SW7425-1, SW7425-10: added MEMC1 support to the emulation framework;
* 
* Hydra_Software_Devel/5   4/28/09 3:17p jessem
* PR 54380: Changed all references to IKOS to EMULATION, EMU or BEMU.
* 
* Hydra_Software_Devel/4   10/16/07 3:49p jessem
* PR 36129: Cleanup.
* 
* Hydra_Software_Devel/3   10/16/07 3:24p jessem
* PR 36129: Modified header to include description.
* 
* Hydra_Software_Devel/2   10/16/07 3:05p jessem
* PR 36129: Prepended function names with IKOS_Client.
* 
***************************************************************************/
#ifndef __BEMU_PAGELIB_H__
#define __BEMU_PAGELIB_H__

#include "bstd.h"

/* support 2 MEMC for now */
#if (BCHP_CHIP==7584)
#define NUM_OF_MEMC       1
#elif (BCHP_CHIP==7445)
#define NUM_OF_MEMC       3
#else
#define NUM_OF_MEMC       2
#endif

int BEMU_Client_PageInit(const unsigned long *TotalMemSize, unsigned long *pMemBase, const unsigned long *pulMemBase, size_t numEntries);
int BEMU_Client_PageUnInit(void);
void BEMU_Client_PageWriteBack(void);
size_t BEMU_Client_Fread(
	void * ptr, 
	size_t size, 
	FILE * stream
);
size_t BEMU_Client_Fwrite(
	void * ptr, 
	size_t size, 
	FILE * stream
);

#endif /* __BEMU_PAGELIB_H__ */
