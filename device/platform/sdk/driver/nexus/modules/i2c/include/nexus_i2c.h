/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_i2c.h $
* $brcm_Revision: 12 $
* $brcm_Date: 4/13/12 9:32a $
*
* API Description:
*   API name: I2c
*    Specific APIs related to I2c Control.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/i2c/7400/include/nexus_i2c.h $
* 
* 12   4/13/12 9:32a erickson
* SW7346-741: Add no sub address option for WriteRead
* 
* 11   1/3/12 2:16p erickson
* SWNOOS-507: add NEXUS_I2cSettings.fourByteXferMode
*
* 6   7/20/09 8:46a erickson
* PR56687: Change I2C clock enum from Khz to Hz
*
* 5   2/18/09 9:38a erickson
* PR52223: add NEXUS_I2c_SetSettings
*
* 4   4/24/08 9:54a erickson
* PR41989: added NEXUS_I2c_ReadNoAddrNoAck
*
* 3   2/4/08 4:24p vsilyaev
* PR 38682: Added more tags for the linux kernel/user proxy mode
*
* 2   2/4/08 2:41p vsilyaev
* PR 38682: Added markup for variable size pointers
*
* 1   1/18/08 2:15p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/8   11/30/07 11:12a erickson
* PR35457: api update
*
* Nexus_Devel/7   11/29/07 2:24p erickson
* PR35457: doc update for 0.5 release
*
* Nexus_Devel/6   11/21/07 1:57p erickson
* PR36725: added #include
*
* Nexus_Devel/5   11/5/07 2:12p erickson
* PR36725: remove NEXUS_I2c_Get
*
* Nexus_Devel/4   10/10/07 11:29a jgarrett
* PR 35551: Revising I2C Handle for synchronization purposes
*
* Nexus_Devel/3   10/5/07 1:42p jgarrett
* PR 35744: Changing comments
*
* Nexus_Devel/2   9/12/07 4:35p jgarrett
* PR 34702: Adding I2C interface
*
* Nexus_Devel/1   9/12/07 10:39a jgarrett
* PR 34702: Adding initial IO headers
*
***************************************************************************/
#ifndef NEXUS_I2C_H__
#define NEXUS_I2C_H__

#include "nexus_types.h"
#include "nexus_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for the I2c Interface.
**/
typedef struct NEXUS_I2c *NEXUS_I2cHandle;

/**
Summary:
Enumeration for I2C clock rate

Description:
This enumeration defines the clock rate for the I2C master
**/
typedef enum NEXUS_I2cClockRate
{
    NEXUS_I2cClockRate_e47Khz  = 47000,
    NEXUS_I2cClockRate_e50Khz  = 50000,
    NEXUS_I2cClockRate_e93Khz  = 93000,
    NEXUS_I2cClockRate_e100Khz = 100000,
    NEXUS_I2cClockRate_e187Khz = 187000,
    NEXUS_I2cClockRate_e200Khz = 200000,
    NEXUS_I2cClockRate_e375Khz = 375000,
    NEXUS_I2cClockRate_e400Khz = 400000,
    NEXUS_I2cClockRate_eMax
} NEXUS_I2cClockRate;

/**
Summary:
I2C Channel Settings.

Description:
All of these settings are used in NEXUS_I2c_Open.
Some of these settings may be changed with NEXUS_I2c_SetSettings. See detailed comments for which ones can be changed.
**/
typedef struct NEXUS_I2cSettings
{
    NEXUS_I2cClockRate clockRate; /* Clock Rate. May be changed with NEXUS_I2c_SetSettings. */
    bool interruptMode;           /* Will interrupts be used for this channel? [default=true] */
    bool burstMode;               /* Will burst mode reads be used? [default=false]. May be changed with NEXUS_I2c_SetSettings. */
    unsigned timeout;             /* If 0, a default will be assumed based on the clock rate */
    
    bool softI2c;                 /* If false, HW i2c will be used.  If true, SW bit bang i2c will be used. */
    NEXUS_GpioHandle clockGpio;   /* Only used if softI2c is true. If not NULL, this will override the BI2C GPIO default. */
    NEXUS_GpioHandle dataGpio;    /* Only used if softI2c is true. If not NULL, this will override the BI2C GPIO default. */
    bool fourByteXferMode;		  /* Will 4 byte transfer mode be used? */
} NEXUS_I2cSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_I2c_GetDefaultSettings(
    NEXUS_I2cSettings *pSettings    /* [out] */
    );

/**
Summary:
Open an I2c channel.
**/
NEXUS_I2cHandle NEXUS_I2c_Open( /* attr{destructor=NEXUS_I2c_Close} */
    unsigned channelIndex,
    const NEXUS_I2cSettings *pSettings
    );

/**
Summary:
Close an I2c channel.
**/
void NEXUS_I2c_Close(
    NEXUS_I2cHandle handle
    );

/**
Summary:
Set new settings. See comments for NEXUS_I2cSettings to see which members can be changed after NEXUS_I2c_Open.
**/
NEXUS_Error NEXUS_I2c_SetSettings(
    NEXUS_I2cHandle i2cHandle,
    const NEXUS_I2cSettings *pSettings
    );

/**
Summary:
Get current settings.
**/
void NEXUS_I2c_GetSettings(
    NEXUS_I2cHandle i2cHandle,
    NEXUS_I2cSettings *pSettings /* [out] */
    );

/**
Summary:
Write a programmable number of I2C registers using an 8 bit sub address.
**/
NEXUS_Error NEXUS_I2c_Write(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr,        /* 8 bit sub address */
    const uint8_t *pData,   /* attr{nelem=length;reserved=8} pointer to data to write */
    size_t length           /* number of bytes to write */
    );

/*
Summary:
This function atomically writes then reads a programmable number of I2C registers using an 8 bit
sub address.
*/
NEXUS_Error NEXUS_I2c_WriteRead(
    NEXUS_I2cHandle i2cHandle,  /* I2C Handle */
    uint16_t chipAddr,          /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr,            /* 8 bit sub address */
    const uint8_t *pWriteData,  /* attr{nelem=writeLength;reserved=8} pointer to data to write */
    size_t writeLength,         /* number of bytes to write */
    uint8_t *pReadData,         /* [out] attr{nelem=readLength;reserved=8} pointer to memory to store read data */
    size_t readLength           /* number of bytes to read */
    );
    
/*
Summary:
This function atomically writes then reads a programmable number of I2C registers using no sub address.
*/
NEXUS_Error NEXUS_I2c_WriteReadNoAddr(
    NEXUS_I2cHandle i2cHandle,  /* I2C Handle */
    uint16_t chipAddr,          /* 7 or 10 bit chip address (_unshifted_) */
    const uint8_t *pWriteData,  /* attr{nelem=writeLength;reserved=8} pointer to data to write */
    size_t writeLength,         /* number of bytes to write */
    uint8_t *pReadData,         /* [out] attr{nelem=readLength;reserved=8} pointer to memory to store read data */
    size_t readLength           /* number of bytes to read */
    );

/**
Summary:
Write a programmable number of I2C registers using an 8 bit
sub address and no ack.
**/
NEXUS_Error NEXUS_I2c_WriteNoAck(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr,        /* 8 bit sub address */
    const uint8_t *pData,   /* attr{nelem=length;reserved=8} pointer to data to write */
    size_t length           /* number of bytes to write */
    );

/**
Summary:
Write a programmable number of I2C registers using a 16 bit
sub address.
**/
NEXUS_Error NEXUS_I2c_WriteA16(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint16_t subAddr,       /* 16 bit sub address */
    const uint8_t *pData,   /* attr{nelem=length;reserved=8} pointer to data to write */
    size_t length           /* number of bytes to write */
    );

/**
Summary:
Write a programmable number of I2C registers using a 24 bit
sub address.
**/
NEXUS_Error NEXUS_I2c_WriteA24(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr,       /* 24 bit sub address */
    const uint8_t *pData,   /* attr{nelem=length;reserved=8} pointer to data to write */
    size_t length           /* number of bytes to write */
    );

/**
Summary:
Write a programmable number of I2C registers without a sub address
(raw write).
**/
NEXUS_Error NEXUS_I2c_WriteNoAddr(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    const uint8_t *pData,   /* attr{nelem=length;reserved=8} pointer to data to write */
    size_t length           /* number of bytes to write */
    );

/**
Summary:
Write a programmable number of I2C registers without a sub address
(raw write), and without waiting for an ack.
**/
NEXUS_Error NEXUS_I2c_WriteNoAddrNoAck(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    const uint8_t *pData,   /* attr{nelem=length;reserved=8} pointer to data to write */
    size_t length           /* number of bytes to write */
    );

/**
Summary:
Write an I2C NVRAM device using an 8 bit sub address.
**/
NEXUS_Error NEXUS_I2c_WriteNvram(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr,        /* 8 bit sub address */
    const uint8_t *pData,   /* attr{nelem=length;reserved=8} pointer to data to write */
    size_t length           /* number of bytes to write */
    );

/**
Summary:
Read a programmable number of I2C registers using an 8 bit
sub address.
**/
NEXUS_Error NEXUS_I2c_Read(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr,        /* 8 bit sub address */
    uint8_t *pData,         /* [out] attr{nelem=length;reserved=8} pointer to memory location to store read data */
    size_t length           /* number of bytes to read */
    );

/**
Summary:
Read a programmable number of I2C registers using an 8 bit
sub address and no ack.
**/
NEXUS_Error NEXUS_I2c_ReadNoAck(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr,        /* 8 bit sub address */
    uint8_t *pData,         /* [out] attr{nelem=length;reserved=8} pointer to memory location to store read data */
    size_t length           /* number of bytes to read */
    );

/**
Summary:
Read a programmable number of I2C registers using a 16 bit
sub address.
**/
NEXUS_Error NEXUS_I2c_ReadA16(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint16_t subAddr,       /* 16 bit sub address */
    uint8_t *pData,         /* [out] attr{nelem=length;reserved=8} pointer to memory location to store read data */
    size_t length           /* number of bytes to read */
    );

/*
Summary:
Read a programmable number of I2C registers using a 24 bit
sub address.
**/
NEXUS_Error NEXUS_I2c_ReadA24(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr,       /* 32 bit sub address */
    uint8_t *pData,         /* [out] attr{nelem=length;reserved=8} pointer to memory location to store read data */
    size_t length           /* number of bytes to read */
    );

/**
Summary:
Read a programmable number of I2C registers without a sub address
(raw read).
**/
NEXUS_Error NEXUS_I2c_ReadNoAddr(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t *pData,         /* [out] attr{nelem=length;reserved=8} pointer to memory location to store read data */
    size_t length           /* number of bytes to read */
    );

/**
Summary:
Read a programmable number of I2C registers without a sub address
(raw read) and no ack.
**/
NEXUS_Error NEXUS_I2c_ReadNoAddrNoAck(
    NEXUS_I2cHandle handle, /* I2C Handle */
    uint16_t chipAddr,      /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t *pData,         /* [out] attr{nelem=length;reserved=8} pointer to memory location to store read data */
    size_t length           /* number of bytes to read */
    );

/**
Summary:
Read using the Enhanced Display Data Channel protocol.
**/
NEXUS_Error NEXUS_I2c_ReadEDDC(
    NEXUS_I2cHandle handle,     /* I2C Handle */
    uint8_t chipAddr,           /* chip address */
    uint8_t segment,            /* EDDC segment pointer */
    uint8_t subAddr,            /* 8-bit sub address */
    uint8_t *pData,             /* [out] attr{nelem=length;reserved=8} pointer to memory location to store read data  */
    size_t length               /* number of bytes to read */
    );

/**
Summary:
Write using the Enhanced Display Data Channel protocol.
**/
NEXUS_Error NEXUS_I2c_WriteEDDC(
    NEXUS_I2cHandle handle,     /* I2C Handle */
    uint8_t chipAddr,           /* chip address */
    uint8_t segment,            /* EDDC segment pointer */
    uint8_t subAddr,            /* 8-bit sub address */
    const uint8_t *pData,       /* attr{nelem=length;reserved=8} pointer to data to write */
    size_t length               /* number of bytes to write */
    );

/**
Summary:
Perform a software reset

Description:
After an interruption in protocol, power loss or system reset, any 2-wire
part can be protocol reset by following these steps: a) Create a start bit
condition, b) clock 9 cycles, c) create another start bit followed by stop
bit condition. The device is ready for next communication after above steps
have been completed.
**/
NEXUS_Error NEXUS_I2c_SwReset(
    NEXUS_I2cHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_I2C_H__ */


