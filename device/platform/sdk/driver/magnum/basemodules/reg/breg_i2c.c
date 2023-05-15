/***************************************************************************
 *	   Copyright (c) 2003-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_i2c.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 4/12/12 7:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_i2c.c $
 * 
 * Hydra_Software_Devel/20   4/12/12 7:34p agin
 * SWNOOS-527:  Fix subaddress length of 2 and 3 problem.  Add no sub-
 * address option for WriteRead.
 * 
 * Hydra_Software_Devel/19   4/3/12 11:21a agin
 * SW7346-741:  Implement magnum changes to support NEXUS_I2c_WriteRead
 * 
 * Hydra_Software_Devel/18   10/4/10 3:39p adtran
 * SW35230-1525: add WriteRead atomic operation
 * 
 * Hydra_Software_Devel/17   3/24/10 11:48a vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/SW7601-171/1   3/9/10 3:22p vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/16   10/22/09 3:01p agin
 * SW7405-2686: Warn users to switch to new way of handling sw i2c.
 * 
 * Hydra_Software_Devel/15   12/13/08 3:44p agin
 * PR42305: Support i2c via gpio for read EDDC.
 * 
 * Hydra_Software_Devel/14   11/21/08 10:56a agin
 * PR49585: Add sw i2c support for 7601.
 * 
 * Hydra_Software_Devel/13   5/4/08 1:40a agin
 * PR42305: Support i2c via gpio.
 * 
 * Hydra_Software_Devel/12   4/23/08 6:27p farshidf
 * PR41729: Add BERR_Code BI2C_P_ReadNoAddrNoAck
 * 
 * Hydra_Software_Devel/11   4/18/05 10:46a agin
 * PR14828: add read and write no ack functions.
 * 
 * Hydra_Software_Devel/10   4/4/05 10:18a agin
 * PR14351: Added BREG_I2C_WriteNoAddrNoAck function.
 * 
 * Hydra_Software_Devel/9   3/2/05 5:49p agin
 * PR14318:  Added support for I2C write for NVRAM devices.
 * 
 * Hydra_Software_Devel/8   7/8/04 2:22p brianlee
 * PR11845: Add I2C API for 3-byte sub address.
 * 
 * Hydra_Software_Devel/7   1/13/04 5:02p brianlee
 * PR9268: Make write structures constant.
 * 
 * Hydra_Software_Devel/6   9/12/03 5:05p brianlee
 * Add chip address to function call.
 * 
 * Hydra_Software_Devel/5   9/12/03 4:58p brianlee
 * Add chip address to EDDC read/write functions.
 * 
 * Hydra_Software_Devel/4   9/11/03 10:59a brianlee
 * Modified function definitions to match I2C definitions.
 * 
 * Hydra_Software_Devel/3   8/22/03 2:52p marcusk
 * Updated with latest changes discussed in the UPG design review.
 * 
 * Hydra_Software_Devel/2   3/10/03 2:39p marcusk
 * Removed const keyword from read routines (copy paste error).
 * 
 * Hydra_Software_Devel/1   3/5/03 5:13p marcusk
 * Initial version.
 * 
 ***************************************************************************/
#include "bstd.h"
#include "breg_i2c.h"
#include "breg_i2c_priv.h"

BDBG_MODULE(breg_i2c);

BERR_Code BREG_I2C_Write(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t subAddr, const uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_Write_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_WriteSw(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t subAddr, const uint8_t *pData, size_t length)
{
	BDBG_WRN(("BREG_I2C_WriteSw() will be removed in the future.  To prepare for this, call BI2C_OpenChannel() with the softI2c field set to true.  Then call BREG_I2C_Write() instead.\n"));
	return (i2cHandle->BREG_I2C_WriteSw_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_WriteNoAck(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t subAddr, const uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_WriteNoAck_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_WriteA16(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint16_t subAddr, const uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_WriteA16_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_WriteSwA16(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint16_t subAddr, const uint8_t *pData, size_t length)
{
	BDBG_WRN(("BREG_I2C_WriteSwA16() will be removed in the future.  To prepare for this, call BI2C_OpenChannel() with the softI2c field set to true.  Then call BREG_I2C_WriteA16() instead.\n"));
	return (i2cHandle->BREG_I2C_WriteSwA16_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_WriteA24(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_WriteA24_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_WriteNoAddr(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, const uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_WriteNoAddr_Func)( i2cHandle->context, chipAddr, pData, length );
}

BERR_Code BREG_I2C_WriteSwNoAddr(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, const uint8_t *pData, size_t length)
{
	BDBG_WRN(("BREG_I2C_WriteSwNoAddr() will be removed in the future.  To prepare for this, call BI2C_OpenChannel() with the softI2c field set to true.  Then call BREG_I2C_WriteNoAddr() instead.\n"));
	return (i2cHandle->BREG_I2C_WriteSwNoAddr_Func)( i2cHandle->context, chipAddr, pData, length );
}

BERR_Code BREG_I2C_WriteNoAddrNoAck(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, const uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_WriteNoAddrNoAck_Func)( i2cHandle->context, chipAddr, pData, length );
}

BERR_Code BREG_I2C_WriteNvram(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t subAddr, const uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_WriteNvram_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_Read(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t subAddr, uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_Read_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_ReadSw(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t subAddr, uint8_t *pData, size_t length)
{
	BDBG_WRN(("BREG_I2C_ReadSw() will be removed in the future.  To prepare for this, call BI2C_OpenChannel() with the softI2c field set to true.  Then call BREG_I2C_Read() instead.\n"));
	return (i2cHandle->BREG_I2C_ReadSw_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_ReadNoAck(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t subAddr, uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_ReadNoAck_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_ReadA16(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint16_t subAddr, uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_ReadA16_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_ReadSwA16(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint16_t subAddr, uint8_t *pData, size_t length)
{
	BDBG_WRN(("BREG_I2C_ReadSwA16() will be removed in the future.  To prepare for this, call BI2C_OpenChannel() with the softI2c field set to true.  Then call BREG_I2C_ReadA16() instead.\n"));
	return (i2cHandle->BREG_I2C_ReadSwA16_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_ReadA24(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint32_t subAddr, uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_ReadA24_Func)( i2cHandle->context, chipAddr, subAddr, pData, length );
}

BERR_Code BREG_I2C_ReadNoAddr(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_ReadNoAddr_Func)( i2cHandle->context, chipAddr, pData, length );
}

BERR_Code BREG_I2C_ReadSwNoAddr(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t *pData, size_t length)
{
	BDBG_WRN(("BREG_I2C_ReadSwNoAddr() will be removed in the future.  To prepare for this, call BI2C_OpenChannel() with the softI2c field set to true.  Then call BREG_I2C_ReadNoAddr() instead.\n"));
	return (i2cHandle->BREG_I2C_ReadSwNoAddr_Func)( i2cHandle->context, chipAddr, pData, length );
}

BERR_Code BREG_I2C_ReadNoAddrNoAck(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_ReadNoAddrNoAck_Func)( i2cHandle->context, chipAddr, pData, length );
}

BERR_Code BREG_I2C_ReadEDDC(BREG_I2C_Handle i2cHandle, uint8_t chipAddr, uint8_t segment, uint8_t subAddr, uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_ReadEDDC_Func)( i2cHandle->context, chipAddr, segment, subAddr, pData, length );
}

BERR_Code BREG_I2C_ReadSwEDDC(BREG_I2C_Handle i2cHandle, uint8_t chipAddr, uint8_t segment, uint8_t subAddr, uint8_t *pData, size_t length)
{
	BDBG_WRN(("BREG_I2C_ReadSwEDDC() will be removed in the future.  To prepare for this, call BI2C_OpenChannel() with the softI2c field set to true.  Then call BREG_I2C_ReadEDDC() instead.\n"));
	return (i2cHandle->BREG_I2C_ReadSwEDDC_Func)( i2cHandle->context, chipAddr, segment, subAddr, pData, length );
}

BERR_Code BREG_I2C_WriteEDDC(BREG_I2C_Handle i2cHandle, uint8_t chipAddr, uint8_t segment, uint8_t subAddr, const uint8_t *pData, size_t length)
{
	return (i2cHandle->BREG_I2C_WriteEDDC_Func)( i2cHandle->context, chipAddr, segment, subAddr, pData, length );
}

BERR_Code BREG_I2C_SetupHdmiHwAccess(BREG_I2C_Handle i2cHandle, uint32_t dataTransferFormat,uint32_t cnt1, uint32_t cnt2)
{
	return (i2cHandle->BREG_I2C_SetupHdmiHwAccess_Func)( i2cHandle->context, dataTransferFormat, cnt1, cnt2);
}

BERR_Code BREG_I2C_WriteRead(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, uint8_t subAddr, const uint8_t *pWriteData, size_t writeLength, uint8_t *pReadData, size_t readLength)
{
	return (i2cHandle->BREG_I2C_WriteRead_Func)( i2cHandle->context, chipAddr, subAddr, pWriteData, writeLength, pReadData, readLength );
}

BERR_Code BREG_I2C_WriteReadNoAddr(BREG_I2C_Handle i2cHandle, uint16_t chipAddr, const uint8_t *pWriteData, size_t writeLength, uint8_t *pReadData, size_t readLength)
{
	return (i2cHandle->BREG_I2C_WriteReadNoAddr_Func)( i2cHandle->context, chipAddr, pWriteData, writeLength, pReadData, readLength );
}

/* End of File */
