/***************************************************************************
 *	   Copyright (c) 2003-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_i2c.h $
 * $brcm_Revision: Hydra_Software_Devel/24 $
 * $brcm_Date: 4/12/12 7:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_i2c.h $
 * 
 * Hydra_Software_Devel/24   4/12/12 7:34p agin
 * SWNOOS-527:  Fix subaddress length of 2 and 3 problem.  Add no sub-
 * address option for WriteRead.
 * 
 * Hydra_Software_Devel/23   4/3/12 11:21a agin
 * SW7346-741:  Implement magnum changes to support NEXUS_I2c_WriteRead
 * 
 * Hydra_Software_Devel/22   3/21/11 4:04p agin
 * SW7346-96:  Add additional prototypes for read and write sw i2c.
 * 
 * Hydra_Software_Devel/21   10/4/10 3:39p adtran
 * SW35230-1525: add WriteRead atomic operation
 * 
 * Hydra_Software_Devel/20   3/24/10 11:46a vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/SW7601-171/1   3/9/10 3:22p vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/19   2/5/09 1:08p vle
 * PR51682: Add declarations for BREG_I2C_ReadSwEDDC
 * 
 * Hydra_Software_Devel/18   12/9/08 11:59a mward
 * PR48987: Add declarations for some  I2C bit-bang functions now used by
 * bhdm.
 * 
 * Hydra_Software_Devel/17   4/24/08 10:17a farshidf
 * PR41729: correct the compile
 * 
 * Hydra_Software_Devel/16   4/23/08 6:27p farshidf
 * PR41729: update the function name
 * 
 * Hydra_Software_Devel/15   4/23/08 6:23p farshidf
 * PR41729: Add BREG_I2C_ReadNoAddrNoAck_Func
 * 
 * Hydra_Software_Devel/14   4/18/05 10:46a agin
 * PR14828: add read and write no ack functions.
 * 
 * Hydra_Software_Devel/13   3/14/05 5:51p agin
 * PR14351: Fassl's changes
 * 
 * Hydra_Software_Devel/12   3/2/05 5:49p agin
 * PR14318:  Added support for I2C write for NVRAM devices.
 * 
 * Hydra_Software_Devel/11   7/8/04 2:23p brianlee
 * PR11845: Add I2C API for 3-byte sub address.
 * 
 * Hydra_Software_Devel/10   1/13/04 5:02p brianlee
 * PR9268: Make write structures constant.
 * 
 * Hydra_Software_Devel/9   9/12/03 4:58p brianlee
 * Add chip address to EDDC read/write functions.
 * 
 * Hydra_Software_Devel/8   9/11/03 10:59a brianlee
 * Modified function definitions to match I2C definitions.
 * 
 * Hydra_Software_Devel/7   8/22/03 2:52p marcusk
 * Updated with latest changes discussed in the UPG design review.
 * 
 * Hydra_Software_Devel/6   5/30/03 12:04p marcusk
 * Updated documentation to use Open rather than Create to be more
 * consistant.
 * 
 * Hydra_Software_Devel/5   3/31/03 1:20p marcusk
 * Updated with comments on threadsafety
 * 
 * Hydra_Software_Devel/4   3/31/03 10:32a marcusk
 * Updated comments.
 * 
 * Hydra_Software_Devel/3   3/31/03 9:42a marcusk
 * Updated with comments.
 * 
 * Hydra_Software_Devel/2   3/10/03 2:39p marcusk
 * Removed const keyword from read routines (copy paste error).
 * 
 * Hydra_Software_Devel/1   3/5/03 5:13p marcusk
 * Initial version.
 * 
 ***************************************************************************/

/*= Module Overview ********************************************************
This module provides a standard abstraction API for accessing I2C registers.
Several I2C busses may exist on a single platform. Some may be hardware based, 
while others software based (using GPIO pins). In order to hide the 
specific implementation of the I2C bus from a PortingInterface this I2C 
register access abstraction is used.

---++++ 7 or 10 Bit I2C Chip Addresses 
For all I2C functions, the chipAddr parameter is always used as the 
*un-shifted* 7 bit (bits 15:7 = 0) or 10 bit (bits 15:10 = 0) chip address. 
If (bits 9:7 == 0) then 7 bit addressing is used by default. Some I2C
implementations will return errors for 10 bit chip addresses (for example
if a hardware I2C implementation does not support 10 bit chip addresses).

---++++ Non 8-Bit Sub-Address Handling 
Although most I2C devices use 8 or 16 bit sub-addresses, some may require 
larger sub addresses. These devices must be use the BREG_I2C_WriteNoAddr and 
BREG_I2C_ReadNoAddr in order to form larger sub-addresses. 

The following example shows how to access read from a device with a 32 bit sub 
address: 

<verbatim>
BERR_Code BXXX_ReadRegister( 
			BREG_I2C_Handle cntx, 
			uint16_t chipAddr, 
			uint32_t subAddr, 
			uint8_t *pData, 
			size_t length )
{
   BREG_I2C_WriteNoAddr(cntx, chipAddr, &subAddr, sizeof(subAddr) );
   BREG_I2C_ReadNoAddr(cntx, chipAddr, pData, length);
}
</verbatim>

Writing to a device with a non 8 or 16 bit sub address is slightly different; 
you need to pre-pend your data with the sub address you wish to write. 

---++++ I2C Register Handles
Hardware and software I2C implementations must follow the defined function 
prototype definitions. All I2C PortingInterfaces supply the a function 
that initializes a standard BREG_I2C_Handle (and fills it with function 
pointers provided by the specific I2C PortingInterfaces implementation).
Please refer to the specific I2C PortingInterface implementations for more
details.

An example of this type of function would be:

<verbatim>
BERR_Code BI2C_Open( BI2C_Handle I2cHandle, BREG_I2C_Handle *pI2cRegHandle );
</verbatim>

This handle should be created during initialization and passed to the 
appropriate porting interfaces.  The porting interfaces in turn pass 
this handle to the abstract I2C functions when then call the appropriate 
I2C implementation though function pointers contained in the BREG_I2C_Handle.

---++++ Handling ThreadSafe with I2C
Since multiple modules (PortingInterface and otherwise) may be sharing the
same I2C bus, any function calls to the modules in question must be serialized
to prevent thread collisions.

	1 All calls to the modules sharing the same I2C handle must be protected
	  through the use of a mutex.  This protection must be handled by the
	  upper level software (driver, middleware, etc).
	1 Upper level software can create an additional layer between the I2C
	  RegisterInterface and the corresponding I2C PortingInterface instance
	  that implements the I2C register access.  In this layer a mutex is
	  used to protect all I2C accesses.  This could easily be done
	  using the BREG_I2C_Impl structure.
***************************************************************************/

#ifndef BREG_I2C_H
#define BREG_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/*
Summary:
This is an opaque handle that is used for I2C register functions.
*/
typedef struct BREG_I2C_Impl *BREG_I2C_Handle;

/*
Summary:
This function writes a programmable number of I2C registers using an 8 bit
sub address.
*/
BERR_Code BREG_I2C_Write(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint8_t subAddr, /* 8 bit sub address */
						 const uint8_t *pData, /* pointer to data to write */
						 size_t length /* number of bytes to write */
						 );

/*
Summary:
This function uses software "bit-bang" to write a programmable number of I2C registers using an 8 bit
sub address.
*/
BERR_Code BREG_I2C_WriteSw(
                            BREG_I2C_Handle i2cHandle, /* I2C Handle */
                            uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
                            uint8_t subAddr, /* 8 bit sub address */
                            const uint8_t *pData, /* pointer to data to write */
                            size_t length /* number of bytes to write */
                          );

/*
Summary:
This function writes a programmable number of I2C registers using an 8 bit
sub address and no ack.
*/
BERR_Code BREG_I2C_WriteNoAck(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint8_t subAddr, /* 8 bit sub address */
						 const uint8_t *pData, /* pointer to data to write */
						 size_t length /* number of bytes to write */
						 );

/*
Summary:
This function writes a programmable number of I2C registers using a 16 bit
sub address.
*/
BERR_Code BREG_I2C_WriteA16(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint16_t subAddr, /* 16 bit sub address */
						 const uint8_t *pData, /* pointer to data to write */
						 size_t length /* number of bytes to write */
						 );

/*
Summary:
This function uses software "bit-bang" to write a programmable number of I2C registers using a 16 bit
sub address.
*/
BERR_Code BREG_I2C_WriteSwA16(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint16_t subAddr, /* 16 bit sub address */
						 const uint8_t *pData, /* pointer to data to write */
						 size_t length /* number of bytes to write */
						 );

/*
Summary:
This function writes a programmable number of I2C registers using a 24 bit
sub address.
*/
BERR_Code BREG_I2C_WriteA24(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint32_t subAddr, /* 24 bit sub address */
						 const uint8_t *pData, /* pointer to data to write */
						 size_t length /* number of bytes to write */
						 );

/*
Summary:
This function uses software "bit-bang" to write a programmable number of I2C registers using a 24 bit
sub address.
*/
BERR_Code BREG_I2C_WriteSwA24(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint32_t subAddr, /* 24 bit sub address */
						 const uint8_t *pData, /* pointer to data to write */
						 size_t length /* number of bytes to write */
						 );

/*
Summary:
This function writes a programmable number of I2C registers without a sub address
(raw write).
*/
BERR_Code BREG_I2C_WriteNoAddr(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 const uint8_t *pData, /* pointer to data to write */
						 size_t length /* number of bytes to write */
					     );

/*
Summary:
This function writes a programmable number of I2C registers without a sub address
(raw write), and without waiting for an ack.
*/
BERR_Code BREG_I2C_WriteNoAddrNoAck(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 const uint8_t *pData, /* pointer to data to write */
						 size_t length /* number of bytes to write */
					     );

/*
Summary:
This function writes an I2C NVRAM device using an 8 bit sub address.
*/
BERR_Code BREG_I2C_WriteNvram(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint8_t subAddr, /* 8 bit sub address */
						 const uint8_t *pData, /* pointer to data to write */
						 size_t length /* number of bytes to write */
						 );

/*
Summary:
This function reads a programmable number of I2C registers using an 8 bit
sub address.
*/
BERR_Code BREG_I2C_Read(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint8_t subAddr, /* 8 bit sub address */
						 uint8_t *pData, /* pointer to memory location to store read data */
						 size_t length /* number of bytes to read */
						 );

/*
Summary:
This function uses software "bit-bang" to read a programmable number of I2C registers using an 8 bit
sub address.
*/
BERR_Code BREG_I2C_ReadSw(
                           BREG_I2C_Handle i2cHandle, /* I2C Handle */
                           uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
                           uint8_t subAddr, /* 8 bit sub address */
                           uint8_t *pData, /* pointer to memory location to store read data */
                           size_t length /* number of bytes to read */
                          );

/*
Summary:
This function reads a programmable number of I2C registers using an 8 bit
sub address and no ack..
*/
BERR_Code BREG_I2C_ReadNoAck(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint8_t subAddr, /* 8 bit sub address */
						 uint8_t *pData, /* pointer to memory location to store read data */
						 size_t length /* number of bytes to read */
						 );

/*
Summary:
This function reads a programmable number of I2C registers using a 16 bit
sub address.
*/
BERR_Code BREG_I2C_ReadA16(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint16_t subAddr, /* 16 bit sub address */
						 uint8_t *pData, /* pointer to memory location to store read data */
						 size_t length /* number of bytes to read */
						 );

/*
Summary:
This function uses software "bit-bang" to read a programmable number of I2C registers using a 16 bit
sub address.
*/
BERR_Code BREG_I2C_ReadSwA16(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint16_t subAddr, /* 16 bit sub address */
						 uint8_t *pData, /* pointer to memory location to store read data */
						 size_t length /* number of bytes to read */
						 );

/*
Summary:
This function reads a programmable number of I2C registers using a 24 bit
sub address.
*/
BERR_Code BREG_I2C_ReadA24(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint32_t subAddr, /* 32 bit sub address */
						 uint8_t *pData, /* pointer to memory location to store read data */
						 size_t length /* number of bytes to read */
						 );

/*
Summary:
This function uses software "bit-bang" to read a programmable number of I2C registers using a 24 bit
sub address.
*/
BERR_Code BREG_I2C_ReadSwA24(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint32_t subAddr, /* 32 bit sub address */
						 uint8_t *pData, /* pointer to memory location to store read data */
						 size_t length /* number of bytes to read */
						 );

/*
Summary:
This function reads a programmable number of I2C registers without a sub address
(raw read).
*/
BERR_Code BREG_I2C_ReadNoAddr(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint8_t *pData, /* pointer to memory location to store read data */
						 size_t length /* number of bytes to read */
						 );

/*
Summary:
This function uses software "bit-bang" to read a programmable number of I2C registers without a sub address
(raw read).
*/
BERR_Code BREG_I2C_ReadSwNoAddr(
                                 BREG_I2C_Handle i2cHandle, /* I2C Handle */
                                 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
                                 uint8_t *pData, /* pointer to memory location to store read data */
                                 size_t length /* number of bytes to read */
                                );

/*
Summary:
This function reads a programmable number of I2C registers without a sub address and no Ack
(raw read).
*/

BERR_Code BREG_I2C_ReadNoAddrNoAck(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
						 uint8_t *pData, /* pointer to memory location to store read data */
						 size_t length /* number of bytes to read */
						 );
/*
Summary:
This function is used to perform an Enhanced Display Data Channel read protocol.
*/
BERR_Code BREG_I2C_ReadEDDC(
						BREG_I2C_Handle i2cHandle,	/* I2C Handle */
						uint8_t chipAddr,			/* chip address */
						uint8_t segment,			/* EDDC segment pointer */
						uint8_t subAddr,			/* 8-bit sub address */
						uint8_t *pData,				/* pointer to memory location to store read data  */
						size_t length				/* number of bytes to read */
						);

/*
Summary:
This function uses software "bit-bang" to read an Enhanced Display Data Channel read protocol.
*/
BERR_Code BREG_I2C_ReadSwEDDC(
						BREG_I2C_Handle i2cHandle,	/* I2C Handle */
						uint8_t chipAddr,			/* chip address */
						uint8_t segment,			/* EDDC segment pointer */
						uint8_t subAddr,			/* 8-bit sub address */
						uint8_t *pData, 			/* pointer to memory location to store read data  */
						size_t length				/* number of bytes to read */
						);

/*
Summary:
This function is used to perform an Enhanced Display Data Channel write protocol.
*/
BERR_Code BREG_I2C_WriteEDDC(
						BREG_I2C_Handle i2cHandle,	/* I2C Handle */
						uint8_t chipAddr,			/* chip address */
						uint8_t segment,			/* EDDC segment pointer */
						uint8_t subAddr,			/* 8-bit sub address */
						const uint8_t *pData,		/* pointer to data to write */
						size_t length				/* number of bytes to write */
						);


/*
Summary:
This function set up i2c for HDMI HDCP HW Ri/Pj Link integrity check
*/
BERR_Code BREG_I2C_SetupHdmiHwAccess(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint32_t dataTransferFormat, 	/* Data Transfer Format */
						 uint32_t cnt1,   /* Counter 1 value */
						 uint32_t cnt2    /* Counter 2 value */
						 );


/*
Summary:
This function is used to perform write and read as an atomic operation.
*/
BERR_Code BREG_I2C_WriteRead(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr,         /* 7 or 10 bit chip address (_unshifted_) */
						 uint8_t subAddr,           /* 8 bit sub address */
						 const uint8_t *pWriteData, /* pointer to data to write */
						 size_t writeLength,        /* number of bytes to write */
						 uint8_t *pReadData,        /* pointer to data to write */
						 size_t readLength          /* number of bytes to write */
						 );

/*
Summary:
This function is used to perform write and read as an atomic operation, with no sub address
*/
BERR_Code BREG_I2C_WriteReadNoAddr(
						 BREG_I2C_Handle i2cHandle, /* I2C Handle */
						 uint16_t chipAddr,         /* 7 or 10 bit chip address (_unshifted_) */
						 const uint8_t *pWriteData, /* pointer to data to write */
						 size_t writeLength,        /* number of bytes to write */
						 uint8_t *pReadData,        /* pointer to data to write */
						 size_t readLength          /* number of bytes to write */
						 );

#ifdef __cplusplus
}
#endif
 
#endif
/* End of File */
