/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_os.cpp $
* $brcm_Revision: 2 $
* $brcm_Date: 2/28/12 5:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/brutus_os.cpp $
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "brutus_os.h"

BDBG_MODULE(brutus_os);

CScopedMutex::CScopedMutex(B_MutexHandle mutex) :
    _mutex(mutex)
{
    BDBG_ASSERT(mutex);
    B_Mutex_Lock(_mutex);
}

CScopedMutex::~CScopedMutex()
{
    BDBG_ASSERT(_mutex);
    B_Mutex_Unlock(_mutex);
}

