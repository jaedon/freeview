/***************************************************************************
*     Copyright (c) 2003, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bkni_emu_i2c.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 10/5/03 6:16p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /TestTools/midas/magnum/services/basemodules/kni/win32emu/bkni_emu_i2c.c $
* 
* Hydra_Software_Devel/2   10/5/03 6:16p frusso
* changed init call to use uri
* 
* Hydra_Software_Devel/1   9/24/03 6:39p frusso
* initial checkin
* 
***************************************************************************/

#include "bstd.h"
#include "bmem.h"
#include "bkni.h"
#include "bdbg.h"
#include "bkni_emu.h"
#include "bkni_emu_i2c.h"

/*****************************************************************************/

int BKNI_EMU_I2cReadReg( uint32_t address, uint32_t *data )
{
	return 0;
}

/*****************************************************************************/

int BKNI_EMU_I2cWriteReg( uint32_t address, uint32_t data )
{
	return 0;
}

/*****************************************************************************/

int BKNI_EMU_I2cReadMem( uint32_t address, void *data, size_t size )
{
	return 0;
}

/*****************************************************************************/

int BKNI_EMU_I2cWriteMem( uint32_t address, void *data, size_t size )
{
	return 0;
}

/*****************************************************************************/

int BKNI_EMU_I2cInit( const char* pszUri )
{
	return 0;
}

/*****************************************************************************/

int BKNI_EMU_I2cUninit(void)
{
	return 0;
}
