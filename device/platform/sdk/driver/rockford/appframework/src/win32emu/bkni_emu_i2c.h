/***************************************************************************
*     Copyright (c) 2003, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bkni_emu_i2c.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 10/5/03 6:16p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /TestTools/midas/magnum/services/basemodules/kni/win32emu/bkni_emu_i2c.h $
* 
* Hydra_Software_Devel/2   10/5/03 6:16p frusso
* changed init call to use uri
* 
* Hydra_Software_Devel/1   9/24/03 6:39p frusso
* initial checkin
* 
***************************************************************************/

#ifndef _BSystem_Emu_I2C_H_
#define _BSystem_Emu_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

int BKNI_EMU_I2cInit( const char* pszUri );
int BKNI_EMU_I2cUninit();
int BKNI_EMU_I2cReadReg( uint32_t address, uint32_t *data );
int BKNI_EMU_I2cWriteReg( uint32_t address, uint32_t data );
int BKNI_EMU_I2cReadMem( uint32_t address, void *data, size_t size );
int BKNI_EMU_I2cWriteMem( uint32_t address, void *data, size_t size );

#ifdef __cplusplus
}
#endif

#endif