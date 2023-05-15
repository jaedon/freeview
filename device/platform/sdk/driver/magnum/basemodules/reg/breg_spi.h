/***************************************************************************
 *	   Copyright (c) 2003-2011, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_spi.h $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 1/11/11 7:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_spi.h $
 * 
 * Hydra_Software_Devel/10   1/11/11 7:01p mbatchel
 * SW35230-2053, SW35230-2702: Integrate SPI isr support for DTV.
 * 
 * Hydra_Software_Devel/SW35230-2053/1   11/11/10 1:55p mbatchel
 * SW35230-2053: Integrate SPI isr support for DTV.
 * 
 * Hydra_Software_Devel/9   1/13/04 5:02p brianlee
 * PR9268: Make write structures constant.
 * 
 * Hydra_Software_Devel/8   9/11/03 5:06p brianlee
 * Fixed SPI register interface function definitions.
 * 
 * Hydra_Software_Devel/7   9/11/03 10:51a brianlee
 * Fixed BREG_SPI_Read definition.
 * 
 * Hydra_Software_Devel/6   8/29/03 10:30a marcusk
 * Fixed BREG_SPI_Read definition.
 * 
 * Hydra_Software_Devel/5   8/22/03 2:52p marcusk
 * Updated with latest changes discussed in the UPG design review.
 * 
 * Hydra_Software_Devel/4   3/31/03 1:20p marcusk
 * Updated with comments on threadsafety
 * 
 * Hydra_Software_Devel/3   3/31/03 10:32a marcusk
 * Updated comments.
 * 
 * Hydra_Software_Devel/2   3/10/03 2:39p marcusk
 * Removed const keyword from read routines (copy paste error).
 * 
 * Hydra_Software_Devel/1   3/5/03 5:13p marcusk
 * Initial version.
 * 
 ***************************************************************************/

/*= Module Overview ********************************************************
This module provides a standard abstraction API for accessing SPI registers.
Several SPI busses may exist on a single platform. Some may be hardware based, 
while others software based (using GPIO pins). In order to hide the 
specific implementation of the SPI bus from a PortingInterface this SPI
register access abstraction is used.

---++++ SPI Register Handles
Hardware and software SPI implementations must follow the defined function 
prototype definitions. All SPI PortingInterfaces supply the a function 
that initializes a standard BREG_SPI_Handle (and fills it with function 
pointers provided by the specific SPI PortingInterfaces implementation).
Please refer to the specific SPI PortingInterface implementations for more
details.

An example of this type of function would be:

<verbatim>
BERR_Code BSPI_CreateSpiRegHandle( BSPI_Handle SpiHandle, BREG_SPI_Handle *pSpiRegHandle );
</verbatim>

This handle should be created during initialization and passed to the 
appropriate porting interfaces.  The porting interfaces in turn pass 
this handle to the abstract SPI functions when then call the appropriate 
SPI implementation though function pointers contained in the BREG_SPI_Handle.

---++++ Handling ThreadSafe with SPI
Since multiple modules (PortingInterface and otherwise) may be sharing the
same SPI bus, any function calls to the modules in question must be serialized
to prevent thread collisions.

	1 All calls to the modules sharing the same SPI handle must be protected
	  through the use of a mutex.  This protection must be handled by the
	  upper level software (driver, middleware, etc).
	2 Upper level software can create an additional layer between the SPI
	  RegisterInterface and the corresponding SPI PortingInterface instance
	  that implements the SPI register access.  In this layer a mutex is
	  used to protect all SPI accesses.  This could easily be done
	  using the BREG_SPI_Impl structure.
***************************************************************************/

#ifndef BREG_SPI_H
#define BREG_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*
Summary:
This is an opaque handle that is used for SPI read and write functions.
*/
typedef struct BREG_SPI_Impl *BREG_SPI_Handle;

void BREG_SPI_SetContinueAfterCommand(
    BREG_SPI_Handle spiHandle,      /* Device channel handle */
    bool bEnable                    /* Enable or disable continue after command flag. This also enables continue after last transfer in the command ram.
                                                                    Using this allows us to hold the chip select line low to do multiple spi transfers. */
);

/*
Summary:
This function gets the total number of bits transferred for every command byte executed.
*/
void BREG_SPI_GetBitsPerTransfer(
                    BREG_SPI_Handle spiHandle,      /* Device channel handle */
                    uint8_t *pBitsPerTransfer       /* Number of bits transferred per command byte. */
                    );

/*
Summary:
This function writes a programmable number of SPI registers.
*/
BERR_Code BREG_SPI_Write(
					BREG_SPI_Handle spiHandle,		/* Device channel handle */
					const uint8_t *pWriteData,		/* pointer to write memory location */
					size_t length					/* size of *pWriteData  buffers (number of bytes to write ) */
					);

/*
Summary:
This function writes all the data in an iterative fashion using the programmable number of SPI registers.
*/
BERR_Code BREG_SPI_WriteAll(
                    BREG_SPI_Handle spiHandle,      /* Device channel handle */
                    const uint8_t *pWriteData,      /* pointer to write memory location */
                    size_t length                   /* size of *pWriteData  buffers (number of bytes to write ) */
                    );
/*
Summary:
This function reads a programmable number of SPI registers.

The SPI protocol always writes data out as it is reading data in.
The incoming data is stored in pReadData while the data
going out is sourced from pWriteData.
*/
BERR_Code BREG_SPI_Read(
					BREG_SPI_Handle spiHandle,		/* Device channel handle */
					const uint8_t *pWriteData,		/* pointer to memory location where data is to sent  */
					uint8_t *pReadData,		/* pointer to memory location to store read data  */
					size_t length					/* size of *pWriteData and *pReadData buffers (number of bytes to read + number of bytes to write) */
					);

/*
Summary:
This function reads readLength amound of data from the SPI slave.

The SPI protocol always writes data out as it is reading data in.
The incoming data is stored in pReadData while the data
going out is sourced from pWriteData.
NOTE: Internally the spi get clocked for writeLength + readLength cycles.
*/
BERR_Code BREG_SPI_ReadAll(
                    BREG_SPI_Handle spiHandle,      /* Device channel handle */
                    const uint8_t *pWriteData,      /* pointer to memory location where data is to sent  */
                    size_t writeLength,             /* size of *pWriteData buffer */
                    uint8_t *pReadData,             /* pointer to memory location to store read data  */
                    size_t readLength               /* size of *pReadData buffer */
                    );

#if defined(HUMAX_PLATFORM_BASE)
/*
Summary:
This function writes a programmable number of SPI registers.
*/
BERR_Code BREG_SPI_WritebyMode(
					BREG_SPI_Handle spiHandle,		/* Device channel handle */
					uint8_t mode,         		
					const uint8_t *pWriteData,		/* pointer to write memory location */
					size_t length					/* size of *pWriteData  buffers (number of bytes to write ) */
					);

/*
Summary:
This function reads a programmable number of SPI registers.

The SPI protocol always writes data out as it is reading data in.
The incoming data is stored in pReadData while the data
going out is sourced from pWriteData.
*/
BERR_Code BREG_SPI_ReadbyMode(
					BREG_SPI_Handle spiHandle,		/* Device channel handle */
					uint8_t mode,				
					const uint8_t *pWriteData,		/* pointer to memory location where data is to sent  */
					uint8_t *pReadData,		/* pointer to memory location to store read data  */
					size_t length					/* size of *pWriteData and *pReadData buffers (number of bytes to read + number of bytes to write) */
					);
#endif

/*
Summary:
This function writes a programmable number of SPI registers in isr.
*/
BERR_Code BREG_SPI_Write_isr(
					BREG_SPI_Handle spiHandle,		/* Device channel handle */
					const uint8_t *pWriteData,		/* pointer to write memory location */
					size_t length					/* size of *pWriteData  buffers (number of bytes to write ) */
					);

/*
Summary:
This function reads a programmable number of SPI registers in isr.

The SPI protocol always writes data out as it is reading data in.
The incoming data is stored in pReadData while the data
going out is sourced from pWriteData.
*/
BERR_Code BREG_SPI_Read_isr(
					BREG_SPI_Handle spiHandle,		/* Device channel handle */
					const uint8_t *pWriteData,		/* pointer to memory location where data is to sent  */
					uint8_t *pReadData,		/* pointer to memory location to store read data  */
					size_t length					/* size of *pWriteData and *pReadData buffers (number of bytes to read + number of bytes to write) */
					);


#ifdef __cplusplus
}
#endif
 
#endif
/* End of File */



