/***************************************************************************
 *	   Copyright (c) 2003-2011, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_spi_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 1/11/11 7:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_spi_priv.h $
 * 
 * Hydra_Software_Devel/6   1/11/11 7:01p mbatchel
 * SW35230-2053, SW35230-2702: Integrate SPI isr support for DTV.
 * 
 * Hydra_Software_Devel/SW35230-2053/1   11/11/10 1:55p mbatchel
 * SW35230-2053: Integrate SPI isr support for DTV.
 * 
 * Hydra_Software_Devel/5   1/13/04 5:02p brianlee
 * PR9268: Make write structures constant.
 * 
 * Hydra_Software_Devel/4   9/11/03 5:07p brianlee
 * Fixed SPI register interface function definitions.
 * 
 * Hydra_Software_Devel/3   8/22/03 2:52p marcusk
 * Updated with latest changes discussed in the UPG design review.
 * 
 * Hydra_Software_Devel/2   3/10/03 2:39p marcusk
 * Removed const keyword from read routines (copy paste error).
 * 
 * Hydra_Software_Devel/1   3/5/03 5:14p marcusk
 * Initial version.
 * 
 ***************************************************************************/
#ifndef BREG_SPI_PRIV_H
#define BREG_SPI_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BREG_SPI_Impl
{
	void * context;
    void (*BREG_SPI_Set_Continue_After_Command_Func)( void *context, bool bEnable);
    void (*BREG_SPI_Get_Bits_Per_Transfer_Func)(void * context, uint8_t *pBitsPerTransfer);
    BERR_Code (*BREG_SPI_Write_All_Func)(void * context, const uint8_t *pWriteData, size_t length);
	BERR_Code (*BREG_SPI_Write_Func)(void * context, const uint8_t *pWriteData, size_t length);
	BERR_Code (*BREG_SPI_Read_Func)(void * context, const uint8_t *pWriteData, uint8_t *pReadData, size_t length);
    BERR_Code (*BREG_SPI_Read_All_Func)(void * context, const uint8_t *pWriteData, size_t writeLength, uint8_t *pReadData, size_t readLength);
#if defined(HUMAX_PLATFORM_BASE)
	BERR_Code (*BREG_SPI_WritebyMode_Func)(void * context,uint8_t mode, const uint8_t *pWriteData, size_t length);
	BERR_Code (*BREG_SPI_ReadbyMode_Func)(void * context,uint8_t mode, const uint8_t *pWriteData, uint8_t *pReadData, size_t length);
#endif	
	BERR_Code (*BREG_SPI_Write_Func_isr)(void * context, const uint8_t *pWriteData, size_t length);
	BERR_Code (*BREG_SPI_Read_Func_isr)(void * context, const uint8_t *pWriteData, uint8_t *pReadData, size_t length);
} BREG_SPI_Impl;

#ifdef __cplusplus
}
#endif
 
#endif
/* End of File */

