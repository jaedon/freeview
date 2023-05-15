/***************************************************************************
 *	   Copyright (c) 2003-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_i2c_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 4/12/12 7:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_i2c_priv.h $
 * 
 * Hydra_Software_Devel/20   4/12/12 7:34p agin
 * SWNOOS-527:  Fix subaddress length of 2 and 3 problem.  Add no sub-
 * address option for WriteRead.
 * 
 * Hydra_Software_Devel/19   4/2/12 3:11p agin
 * SW7346-741:  Implement magnum changes to support NEXUS_I2c_WriteRead
 * 
 * Hydra_Software_Devel/SWNOOS-527/1   4/2/12 12:09p agin
 * SWNOOS-527:  Support I2C WriteRead.
 * 
 * Hydra_Software_Devel/18   3/21/11 4:06p agin
 * SW7346-96:  Add BREG_I2C_ReadSwA24_Func and BREG_I2C_WriteSwA24_Func.
 * 
 * Hydra_Software_Devel/17   10/4/10 3:39p adtran
 * SW35230-1525: add WriteRead atomic operation
 * 
 * Hydra_Software_Devel/16   3/24/10 11:46a vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/SW7601-171/1   3/9/10 3:22p vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/15   11/21/08 10:56a agin
 * PR49585: Add sw i2c support for 7601.
 * 
 * Hydra_Software_Devel/14   10/14/08 5:08p agin
 * PR42305: Add BI2C_P_ReadSwNoAddr and BI2C_P_ReadSwEDDC.
 * 
 * Hydra_Software_Devel/13   5/4/08 1:37a agin
 * PR42305: Support i2c via gpio.
 * 
 * Hydra_Software_Devel/12   4/21/08 2:09p farshidf
 * PR41729: fix compile issue
 * 
 * Hydra_Software_Devel/11   4/21/08 2:02p farshidf
 * PR41729: add BREG_I2C_ReadNoAddrNoAck_Func
 * 
 * Hydra_Software_Devel/10   4/18/05 10:46a agin
 * PR14828: add read and write no ack functions.
 * 
 * Hydra_Software_Devel/9   3/14/05 5:51p agin
 * PR14351: Fassl's changes
 * 
 * Hydra_Software_Devel/8   3/2/05 5:49p agin
 * PR14318:  Added support for I2C write for NVRAM devices.
 * 
 * Hydra_Software_Devel/7   7/8/04 2:23p brianlee
 * PR11845: Add I2C API for 3-byte sub address.
 * 
 * Hydra_Software_Devel/6   1/13/04 5:02p brianlee
 * PR9268: Make write structures constant.
 * 
 * Hydra_Software_Devel/5   9/12/03 4:58p brianlee
 * Add chip address to EDDC read/write functions.
 * 
 * Hydra_Software_Devel/4   9/11/03 10:53a brianlee
 * Fixed function definitions so they match I2C definitions.
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
#ifndef BREG_I2C_PRIV_H
#define BREG_I2C_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BREG_I2C_Impl
{
	void * context;
	BERR_Code (*BREG_I2C_Write_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteSw_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteNoAck_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteA16_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteSwA16_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteA24_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteSwA24_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteNoAddr_Func)(void * context, uint16_t chipAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteSwNoAddr_Func)(void * context, uint16_t chipAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteNoAddrNoAck_Func)(void * context, uint16_t chipAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteNvram_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_Read_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadSw_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadNoAck_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadA16_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadSwA16_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadA24_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadSwA24_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadNoAddr_Func)(void * context, uint16_t chipAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadSwNoAddr_Func)(void * context, uint16_t chipAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadNoAddrNoAck_Func)(void * context, uint16_t chipAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadEDDC_Func)(void * context, uint8_t chipAddr, uint8_t segment, uint32_t subAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_ReadSwEDDC_Func)(void * context, uint8_t chipAddr, uint8_t segment, uint32_t subAddr, uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_WriteEDDC_Func)(void * context, uint8_t chipAddr, uint8_t segment, uint32_t subAddr, const uint8_t *pData, size_t length);
	BERR_Code (*BREG_I2C_SetupHdmiHwAccess_Func)(void * context, uint32_t dataTransferFormat,uint32_t cnt1, uint32_t cnt2);	
	BERR_Code (*BREG_I2C_WriteRead_Func)(void * context, uint16_t chipAddr, uint32_t subAddr, const uint8_t *pWriteData, size_t writeLength, uint8_t *pReadData, size_t readLength );
	BERR_Code (*BREG_I2C_WriteReadNoAddr_Func)(void * context, uint16_t chipAddr, const uint8_t *pWriteData, size_t writeLength, uint8_t *pReadData, size_t readLength );
        
} BREG_I2C_Impl;

#ifdef __cplusplus
}
#endif
 
#endif
/* End of File */

