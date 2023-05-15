/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bspi_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 7/23/12 3:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/spi/7584/bspi_priv.h $
 * 
 * Hydra_Software_Devel/5   7/23/12 3:03p brianlee
 * SW7358-345: Added function to select EBI CS for HIF SPI.
 * 
 * Hydra_Software_Devel/4   4/23/10 2:29p erickson
 * SW3548-2904: don't write directly to BCHP_IRQ0_SPI_IRQEN because of
 * possible linux SPI flash collision, clean up BCHP_CHIP lists, add _isr
 * to two priv functions
 *
 * Hydra_Software_Devel/3   1/13/04 5:02p brianlee
 * PR9268: Make write structures constant.
 *
 * Hydra_Software_Devel/2   9/11/03 6:24p brianlee
 * Added more private functions.
 *
 * Hydra_Software_Devel/1   7/31/03 10:06a brianlee
 * Private SPI functions.
 *
 ***************************************************************************/
#ifndef BSPI_PRIV_H__
#define BSPI_PRIV_H__

#include "bspi.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	This function performs a SPI read operation.

Description:
	This function is used to read data from a SPI slave using a particular
	SPI channel.  The slave select for that SPI channel should already have
	been set when opening the channel.

Returns:
	TODO:

See Also:
	BSPI_AbortTxfr

****************************************************************************/
BERR_Code BSPI_P_Read(
	void *context,				/* Device channel handle */
	const uint8_t *pWriteData,	/* pointer to memory location where data is to sent  */
	uint8_t *pReadData,			/* pointer to memory location to store read data  */
	size_t length				/* number of bytes to read + number of bytes to write */
	);

/***************************************************************************
Summary:
	This function performs a SPI write operation.

Description:
	This function is used to write data to a SPI slave using a particular
	SPI channel.  The slave select for that SPI channel should already have
	been set when opening the channel.

Returns:
	TODO:

See Also:
	BSPI_AbortTxfr

****************************************************************************/
BERR_Code BSPI_P_Write(
	void *context,				/* Device channel handle */
	const uint8_t *pWriteData,	/* pointer to memory location where data is to sent  */
	size_t length				/* number of bytes to write */
	);

#if defined(HUMAX_PLATFORM_BASE)
/***************************************************************************
Summary:
	This function performs a SPI read operation.

Description:
	This function is used to read data from a SPI slave using a particular
	SPI channel.  The slave select for that SPI channel should already have
	been set when opening the channel.

Returns:
	TODO:

See Also:
	BSPI_AbortTxfr

****************************************************************************/
BERR_Code BSPI_P_Read_By_Mode(
	void *context,				/* Device channel handle */
    uint8_t mode,         		
	const uint8_t *pWriteData,	/* pointer to memory location where data is to sent  */
	uint8_t *pReadData,			/* pointer to memory location to store read data  */
	size_t length				/* number of bytes to read + number of bytes to write */
	);

/***************************************************************************
Summary:
	This function performs a SPI write operation.

Description:
	This function is used to write data to a SPI slave using a particular
	SPI channel.  The slave select for that SPI channel should already have
	been set when opening the channel.

Returns:
	TODO:

See Also:
	BSPI_AbortTxfr

****************************************************************************/
BERR_Code BSPI_P_Write_By_Mode(
	void *context,				/* Device channel handle */
    uint8_t mode,         		
	const uint8_t *pWriteData,	/* pointer to memory location where data is to sent  */
	size_t length				/* number of bytes to write */
	);
#endif

/***************************************************************************
Summary:
	This function sets the clock setting for an SPI channel

Description:
	This function allows the user to change the SPI clock setting.

Returns:
	TODO:

See Also:

****************************************************************************/
void BSPI_P_SetClk (
	BSPI_ChannelHandle channel, 		/* Device channel handle */
	uint32_t baud, 						/* serial clock baud setting */
	uint8_t clkConfig					/* clock polarity/phase setting */
	);

/***************************************************************************
Summary:
	This function sets the EBI CS line to use with HIF MSPI

Description:
	This function allows the user to set the EBI CS line to use with HIF MSPI

Returns:
	TODO:

See Also:

****************************************************************************/
void BSPI_P_SetEbiCs(
    BSPI_ChannelHandle  hChn,           /* Device channel handle */
    BSPI_EbiCs          ebiCs           /* EBI CS to use */
    );

/***************************************************************************
Summary:
	This function waits until a SPI transfer is finished

Description:
	This function is used to wait for SPI to finish.

Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_P_WaitForCompletion
(
	BSPI_ChannelHandle 	hChn,							/* Device channel handle */
	uint32_t			numBytes						/* number of bytes to transfer */
);

/***************************************************************************
Summary:
	This function handles SPI interrupt

Description:
	This function is the SPI ISR.

Returns:
	TODO:

See Also:

****************************************************************************/
void BSPI_P_HandleInterrupt_Isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
);

#ifdef __cplusplus
}
#endif

#endif



