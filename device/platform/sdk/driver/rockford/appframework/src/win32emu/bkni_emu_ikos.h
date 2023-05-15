/***************************************************************************
*     Copyright (c) 2003, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bkni_emu_ikos.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 10/5/03 6:16p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /TestTools/midas/magnum/services/basemodules/kni/win32emu/bkni_emu_ikos.h $
* 
* Hydra_Software_Devel/2   10/5/03 6:16p frusso
* changed init call to use uri
* 
* Hydra_Software_Devel/1   9/24/03 6:39p frusso
* initial checkin
* 
***************************************************************************/

#ifndef _BSystem_Emu_IKOS_H_
#define _BSystem_Emu_IKOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "windows.h"

int BKNI_EMU_IkosInit( const char* pszUri );
int BKNI_EMU_IkosUninit();
int BKNI_EMU_IkosReadReg( uint32_t address, uint32_t *data );
int BKNI_EMU_IkosWriteReg( uint32_t address, uint32_t data );
int BKNI_EMU_IkosReadMem( uint32_t address, void *data, size_t size );
int BKNI_EMU_IkosWriteMem( uint32_t address, void *data, size_t size );
int BKNI_EMU_IkosRecv( SOCKET s, char *buf, int len, int flags );
int BKNI_EMU_IkosSend( SOCKET s, const char *buf, int len, int flags );

#ifdef __cplusplus
}
#endif

#endif