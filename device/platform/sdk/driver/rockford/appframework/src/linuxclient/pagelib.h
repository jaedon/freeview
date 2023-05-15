/***************************************************************************
*	  Copyright (c) 2003-2009, Broadcom Corporation
*	  All Rights Reserved
*	  Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: pagelib.h $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 4/28/09 3:17p $
*
* Module Description: Emulator Paging API
*
* Revision History: 
*
* $brcm_Log: /rockford/appframework/src/linuxclient/pagelib.h $
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

int BEMU_Client_PageInit(size_t TotalMemSize, void **pMemBase, unsigned long ulMemBase);
int BEMU_Client_PageUnInit(void);
void BEMU_Client_PageWriteBack(void);

#endif /* __BEMU_PAGELIB_H__ */
