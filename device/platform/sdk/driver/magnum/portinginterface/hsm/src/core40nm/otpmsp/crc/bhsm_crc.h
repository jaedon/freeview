/***************************************************************************
 *     Copyright (c) 2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * Module Description:
 *                    			                    
 *
 * Revision History: $
 *
 *
 ***************************************************************************/

/*****************************************************************************
Summary:

This function calculates the CRC for the 32 byte data section.

Description:

This function shall be used to calculate the CRC required when programming 
the OTP data section.  Before the data section can be programmed, a call
to this function must be done so the proper CRC , along with the data section
data, can be passed onto the BHSM_ProgramDataSectIO PI.

SAMPLE CODE:

****ADD THE FOLLOWING INCLUDE FILES:****

#include "bsp_s_otp.h"
#include "bhsm_otpmsp.h"
#include "bhsm_crc.h"

****ADD THE FOLLOWING PARAMETER DEFINITIONS:****

# This contains the 32 bytes of data section, MSB first. 
unsigned char data_section[32] = 
{
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00		
};	

unsigned char crc_value[4];

BHSM_ProgramDataSectIO_t inoutp_progDataSectIO;

****ADD THE FOLLOWING CODE TO PROGRAM A DATA SECTION:****

BHSM_CalculateCrc(data_section, crc_value);

inoutp_progDataSectIO.progDsEnum = 0; #This specifies the data section to program (0-7)

for(i=0;i<4;i++)
    inoutp_progDataSectIO.aucCrc[i] = crc_value[i];

for(i=0;i<32;i++)
	inoutp_progDataSectIO.aucDataSectData[i] = data_section[i];

inoutp_progDataSectIO.unMode = 0x00010112;

BHSM_ProgramDataSect (
    transport->hHsm,
	&inoutp_progDataSectIO
);

Calling Context:
This function can be called anytime after HSM is initialized and before 
programming a data section using BHSM_ProgramDataSectIO

The library that contains this function is located in "magnum\portinginterface\hsm\7400\a0\otpmsp\crc\libcrc.a"

When building, add the fillowing to your makefile:
LIBS += $(MAGNUM)/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/otpmsp/crc/libcrc.a
CFLAGS += -I$(MAGNUM)/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/otpmsp/crc


Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed. 

Input:
unsigned char *  - data_section, pointer to 32 bytes of data with MSB first.
unsigned char *  - crc, storage location of 4 byte CRC of data section.
               		
Output:
NA

Returns:
NA

See Also:
BHSM_ProgramDataSectIO

*****************************************************************************/
void BHSM_CalculateCrc(
	unsigned char *data_section,
	unsigned char *crc
);

