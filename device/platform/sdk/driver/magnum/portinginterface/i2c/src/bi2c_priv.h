/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bi2c_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 4/12/12 7:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/i2c/7346/bi2c_priv.h $
 * 
 * Hydra_Software_Devel/27   4/12/12 7:34p agin
 * SWNOOS-527:  Fix subaddress length of 2 and 3 problem.  Add no sub-
 * address option for WriteRead.
 * 
 * Hydra_Software_Devel/26   4/2/12 3:08p agin
 * SW7346-741:  Implement magnum changes to support NEXUS_I2c_WriteRead
 * 
 * Hydra_Software_Devel/SWNOOS-527/1   4/2/12 11:57a agin
 * SWNOOS-527:  Support I2C WriteRead.
 * 
 * Hydra_Software_Devel/25   2/1/12 12:17a agin
 * SW7346-657:  Subaddress not handled properly for
 * BI2C_P_WriteBy4BytesCmd.
 * 
 * Hydra_Software_Devel/24   1/9/12 10:43a agin
 * SWNOOS-507:  Eliminate redundant functions.
 * 
 * Hydra_Software_Devel/23   1/7/12 1:42p agin
 * SWNOOS-507:  Don't require a buffer for BI2C_P_WriteBy4BytesCmd.
 * 
 * Hydra_Software_Devel/22   4/14/11 3:17p agin
 * SW7344-44:  Add mutex protection for i2c accesses, handle EDDC
 * different.
 * 
 * Hydra_Software_Devel/21   3/21/11 4:11p agin
 * SW7346-96:  Add prototypes for BI2C_P_WriteSwA24, BI2C_P_ReadSwA24 and
 * BI2C_P_WriteSwA16.
 * 
 * Hydra_Software_Devel/20   3/24/10 11:47a vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/SW7601-171/1   3/9/10 3:22p vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/19   10/26/09 1:52p agin
 * SW7115-841: Add support for BERR_Code BI2C_P_ReadSwNoAck and
 * BI2C_P_ReadSwNoAddrNoAck.
 * 
 * Hydra_Software_Devel/18   12/13/08 3:45p agin
 * PR42305: Support i2c via gpio for read EDDC.
 * 
 * Hydra_Software_Devel/17   11/21/08 10:57a agin
 * PR49585: Add sw i2c support for 7601.
 * 
 * Hydra_Software_Devel/16   10/14/08 5:07p agin
 * PR42305: Add BI2C_P_ReadSwNoAddr and BI2C_P_ReadSwEDDC.
 * 
 * Hydra_Software_Devel/15   5/4/08 1:36a agin
 * PR42305: Support i2c via gpio.
 * 
 * Hydra_Software_Devel/14   4/21/08 1:17p farshidf
 * PR41729: add BI2C_P_ReadNoAddrNoAck function
 * 
 * Hydra_Software_Devel/13   5/24/07 11:53a jkim
 * PR29251: Add 4byte transfer support for 7405
 * 
 * Hydra_Software_Devel/12   4/24/07 5:25p jkim
 * PR29538: Add burst mode support
 * 
 * Hydra_Software_Devel/11   4/18/05 10:46a agin
 * PR14828: add read and write no ack functions.
 * 
 * Hydra_Software_Devel/10   3/14/05 5:51p agin
 * PR14351: Fassl's changes
 * 
 * Hydra_Software_Devel/9   3/2/05 5:49p agin
 * PR14318:  Added support for I2C write for NVRAM devices.
 * 
 * Hydra_Software_Devel/8   7/8/04 2:23p brianlee
 * PR11845: Add I2C API for 3-byte sub address.
 * 
 * Hydra_Software_Devel/7   1/13/04 5:02p brianlee
 * PR9268: Make write structures constant.
 * 
 * Hydra_Software_Devel/6   10/15/03 2:33p brianlee
 * Moved #defne MAX_I2C_CHANNELS to bi2c.h so application code can use it.
 * 
 * Hydra_Software_Devel/5   9/18/03 10:16a brianlee
 * Make SetClk and GetClk functions public so the caller can change the
 * clock rate after the channel is opened.
 * 
 * Hydra_Software_Devel/4   9/12/03 5:02p brianlee
 * Add chip address to EDDC read/write functions instead of using the
 * fixed value of A0.
 * 
 * Hydra_Software_Devel/3   9/11/03 11:42a brianlee
 * Modified function definitions to match the ones in register interface.
 * 
 * Hydra_Software_Devel/2   8/21/03 4:03p brianlee
 * Added EDDCRead and EDDCWrite functions.  Added enums for I2C commands.
 * 
 * Hydra_Software_Devel/1   7/31/03 9:44a brianlee
 * Private I2C functions.
 * 
 ***************************************************************************/
#ifndef BI2C_PRIVATE_H__
#define BI2C_PRIVATE_H__

#include "bi2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Data Transfer Format */
typedef enum
{
    BI2C_P_eWriteOnly = 0,
    BI2C_P_eReadOnly  = 1,
    BI2C_P_eReadWrite = 2,
    BI2C_P_eWriteRead = 3
} BI2C_P_XferType;

/***************************************************************************
Summary:
	This function performs a read operation with 8-bit target sub address.

Description:
	This function is used to read from a target with 8-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_ReadNoAck(), BI2C_P_ReadA16(), BI2C_P_ReadNoAddr(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_Read(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 8-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with 8-bit target sub address.

Description:
	This function is used to read from a target with 8-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_ReadNoAck(), BI2C_P_ReadA16(), BI2C_P_ReadNoAddr(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadSw(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 8-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with 8-bit target sub address
	and no ack.

Description:
	This function is used to read from a target with 8-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadA16(), BI2C_P_ReadNoAddr(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadNoAck(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 8-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with 8-bit target sub address
	and no ack.

Description:
	This function is used to read from a target with 8-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadA16(), BI2C_P_ReadNoAddr(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadSwNoAck(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 8-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with 16-bit target sub address.

Description:
	This function is used to read from a target with 16-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadNoAck(), BI2C_P_ReadNoAddr(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadA16(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 16-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with 16-bit target sub address.

Description:
	This function is used to read from a target with 16-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadNoAck(), BI2C_P_ReadNoAddr(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadSwA16(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 16-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with 24-bit target sub address.

Description:
	This function is used to read from a target with 24-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadNoAck(), BI2C_P_ReadNoAddr(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadA24(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 24-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with 24-bit target sub address.

Description:
	This function is used to read from a target with 24-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadNoAck(), BI2C_P_ReadNoAddr(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadSwA24(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 24-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with no target sub address.

Description:
	This function is used to read from a target without specifying sub
	address.  This is normally used when the user wants to continue reading 
	from where the last read command left off.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadNoAck(), BI2C_P_ReadA16(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadNoAddr(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with no target sub address.

Description:
	This function is used to read from a target without specifying sub
	address.  This is normally used when the user wants to continue reading 
	from where the last read command left off.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadNoAck(), BI2C_P_ReadA16(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadSwNoAddr(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with no target sub address.

Description:
	This function is used to read from a target without specifying sub
	address and without waiting for an ack.  This is normally used when the 
	user wants to continue reading from where the last read command left off.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadNoAck(), BI2C_P_ReadA16(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadNoAddrNoAck(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a read operation with no target sub address.

Description:
	This function is used to read from a target without specifying sub
	address and without waiting for an ack.  This is normally used when the 
	user wants to continue reading from where the last read command left off.
		
Returns:
	TODO:

See Also:
	BI2C_P_Read(), BI2C_P_ReadNoAck(), BI2C_P_ReadA16(), BI2C_EDDCRead().

****************************************************************************/
BERR_Code BI2C_P_ReadSwNoAddrNoAck(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a write followed by read operation.

Description:
	This function is used to avoid doing a stop followed by start between transfers.
		
Returns:
	TODO:

See Also:
	BI2C_P_WriteA16(), BI2C_P_WriteNoAck(), BI2C_P_WriteNoAddr(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteRead
(
    void *              context, 
    uint16_t            chipAddr, 
    uint32_t            subAddr, 
    const uint8_t       *pWriteData, 
    size_t              writeLength, 
    uint8_t             *pReadData, 
    size_t              readLength 
);

/***************************************************************************
Summary:
	This function performs a write followed by read operation, with no sub address.

Description:
	This function is used to avoid doing a stop followed by start between transfers.
		
Returns:
	TODO:

See Also:
	BI2C_P_WriteA16(), BI2C_P_WriteNoAck(), BI2C_P_WriteNoAddr(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteReadNoAddr
(
    void *              context, 
    uint16_t            chipAddr, 
    const uint8_t       *pWriteData, 
    size_t              writeLength, 
    uint8_t             *pReadData, 
    size_t              readLength 
);

/***************************************************************************
Summary:
	This function performs a write operation with 8-bit target sub address.

Description:
	This function is used to write data to a target with 8-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_WriteA16(), BI2C_P_WriteNoAck(), BI2C_P_WriteNoAddr(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_Write(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 8-bit sub address */
    const uint8_t       *pData,             /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a write operation with 8-bit target sub address.

Description:
	This function is used to write data to a target with 8-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_WriteA16(), BI2C_P_WriteNoAck(), BI2C_P_WriteNoAddr(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteSw(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 8-bit sub address */
    const uint8_t       *pData,             /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a write operation with 8-bit target sub address
	and no ack.

Description:
	This function is used to write data to a target with 8-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_WriteA16(), BI2C_P_WriteA16(), BI2C_P_WriteNoAddr(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteNoAck(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 8-bit sub address */
    const uint8_t       *pData,             /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a write operation with 16-bit target sub address.

Description:
	This function is used to write data to a target with 16-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Write(), BI2C_P_WriteNoAck(), BI2C_P_WriteNoAddr(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteA16(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 16-bit sub address */
    const uint8_t       *pData,             /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a write operation with 16-bit target sub address.

Description:
	This function is used to write data to a target with 16-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Write(), BI2C_P_WriteNoAck(), BI2C_P_WriteNoAddr(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteSwA16(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 16-bit sub address */
    const uint8_t       *pData,             /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a write operation with 24-bit target sub address.

Description:
	This function is used to write data to a target with 16-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Write(), BI2C_P_WriteNoAck(), BI2C_P_WriteNoAddr(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteA24(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 24-bit sub address */
    const uint8_t       *pData,             /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a write operation with 24-bit target sub address.

Description:
	This function is used to write data to a target with 16-bit sub address.
		
Returns:
	TODO:

See Also:
	BI2C_P_Write(), BI2C_P_WriteNoAck(), BI2C_P_WriteNoAddr(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteSwA24(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 24-bit sub address */
    const uint8_t       *pData,             /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a write operation with no target sub address.

Description:
	This function is used to write from a target without specifying sub
	address.  This is normally used when the user wants to continue writing 
	from where the last write command left off.
		
Returns:
	TODO:

See Also:
	BI2C_P_Write(), BI2C_P_WriteNoAck(), BI2C_P_WriteA16(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteNoAddr(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    const uint8_t       *pData,             /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a write operation with no target sub address
	and no ack.

Description:
	This function is used to write from a target without specifying sub
	address and without waiting for an ack.  This is normally used when 
	the user wants to continue writing from where the last write command 
	left off.
		
Returns:
	TODO:

See Also:
	BI2C_P_Write(), BI2C_P_WriteNoAck(), BI2C_P_WriteA16(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteNoAddrNoAck(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    const uint8_t       *pData,     /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a write operation to an nvram device.

Description:
	This function is used to write an nvram device.
		
Returns:
	TODO:

See Also:
	BI2C_P_Write(), BI2C_P_WriteNoAck(), BI2C_P_WriteA16(), BI2C_EDDCwrite().

****************************************************************************/
BERR_Code BI2C_P_WriteNvram(
    void                *context,           /* Device channel handle */
    uint16_t            chipAddr,           /* chip address */
    uint32_t            subAddr,            /* 8-bit sub address */
    const uint8_t       *pData,     /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function performs a EDDC read operation.

Description:
	This function is used to perform an Enhanced Display Data Channel read
	protocol.
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_ReadEDDC(
    void                *context,           /* Device channel handle */
    uint8_t             chipAddr,           /* chip address */
    uint8_t             segment,            /* EDDC segment pointer */
    uint32_t            subAddr,            /* 8-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a EDDC read operation.

Description:
	This function is used to perform an Enhanced Display Data Channel read
	protocol.
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_ReadSwEDDC(
    void                *context,           /* Device channel handle */
    uint8_t             chipAddr,           /* chip address */
    uint8_t             segment,            /* EDDC segment pointer */
    uint32_t            subAddr,            /* 8-bit sub address */
    uint8_t             *pData,             /* pointer to memory location to store read data  */
    size_t              length              /* number of bytes to read */
    );

/***************************************************************************
Summary:
	This function performs a EDDC write operation.

Description:
	This function is used to perform an Enhanced Display Data Channel write
	protocol.
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_WriteEDDC(
    void                *context,           /* Device channel handle */
    uint8_t             chipAddr,           /* chip address */
    uint8_t             segment,            /* EDDC segment pointer */
    uint32_t            subAddr,            /* 8-bit sub address */
    const uint8_t       *pData,     /* pointer to data to write */
    size_t              length              /* number of bytes to write */
    );

/***************************************************************************
Summary:
	This function is the main read function

Description:
	This function is called by public read functions
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_ReadCmd
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* pointer to register address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    uint8_t             *pData,             /* storage */
    size_t              numBytes,           /* number of bytes to read */
    bool                mutex,              /* protect with a mutex */
    bool                ack,                /* check for ack? */
    bool                noStop              /* no stop at the end? */
);

/***************************************************************************
Summary:
	This function is the main read function

Description:
	This function is called by public read functions
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_ReadSwCmd
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* pointer to register address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    uint8_t             *pData,             /* storage */
    size_t              numBytes,           /* number of bytes to read */
    bool                eddc,               /* EDDC mode */
    uint8_t             segment,            /* EDDC segment */
    bool                checkForAck         /* Ignore ack? */
);

/***************************************************************************
Summary:
	This function is the burst mode read function

Description:
	This function is called by public read functions
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_BurstReadCmd
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* pointer to register address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    uint8_t             *pData,             /* storage */
    size_t              numBytes            /* number of bytes to read */
);

/***************************************************************************
Summary:
	This function is the burst mode read function

Description:
	This function is called by public read functions
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_BurstReadCmdNoAck
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* pointer to register address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    uint8_t             *pData,             /* storage */
    size_t              numBytes            /* number of bytes to read */
);

/***********************************************************************func
 * Name: BI2C_P_ReadBy4BytesCmd
 *   - Read data from i2c slave device.
 *       write (1 byte of slave device register's address, subAddr)
 *   and read  (0-8 bytes of data)
 *   Using i2c Write.& Read
 *
 * NOTE: This function will be called by BI2C_P_Read, BI2C_P_ReadA16, BI2C_P_ReadNoAddr
 ***************************************************************************/
BERR_Code BI2C_P_ReadBy4BytesCmd
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* sub address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    uint8_t             *pData,             /* storage */
    size_t              numBytes,           /* number of bytes to read */
    bool                mutex,              /* protect with a mutex */
    bool                ack,                /* check for ack? */
    bool                noStop              /* no stop at the end? */
);

/***************************************************************************
Summary:
	This function is the main write function

Description:
	This function is called by public write functions
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_WriteCmd
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* sub address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    const uint8_t       *pData,             /* storage */
    size_t              numBytes,           /* number of bytes to write */
    bool                isNvram,            /* is this a nvram access? */ 
    bool                mutex,              /* protect with a mutex */
    bool                ack,                /* check for ack? */
    bool                noStop              /* Do we need a no stop at the end of the transfer? */
);

/***************************************************************************
Summary:
	This function is the main write function

Description:
	This function is called by public write functions
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_WriteSwCmd
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* pointer to register address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    const uint8_t       *pData,             /* storage */
    size_t              numBytes,           /* number of bytes to write */
    bool                isNvram             /* is this a nvram access? */ 
);

/***********************************************************************func
 * Name: BI2C_P_WriteBy4BytesCmd
 *   - Write data to an i2c slave device.
 *
 * NOTE: This function will be called by BI2C_P_Write, BI2C_P_WriteA16, BI2C_P_WriteNoAddr
 ***************************************************************************/
BERR_Code BI2C_P_WriteBy4BytesCmd
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* register address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    const uint8_t       *pData,             /* storage */
    size_t              numBytes,           /* number of bytes to write */
    bool                isNvram,            /* is this a nvram access? */ 
    bool                mute,               /* protect with a mutex */
    bool                ack,                /* check for ack? */
    bool                noStop              /* do we need a stop at the end of the transfer */
);

/***************************************************************************
Summary:
	This function waits for I2C transfer to complete

Description:
	This function waits for event if interrupt is enabled.  If interrupt is
	not enabled, it stays in a loop and polls until timeout occurs.
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_WaitForCompletion
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint32_t            numBytes            /* number of bytes to transfer */
);

/***************************************************************************
Summary:
	This function waits for an NVRAM I2C slave device transfer to complete

Description:
	This function waits a calculated amount of time for an NVRAM I2C
	slave device to respond with an acknowledgement.
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_WaitForNVMToAcknowledge
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* pointer to register address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    size_t              numBytes            /* number of bytes to write */
);

/***************************************************************************
Summary:
	This function waits for data to leave the 8 byte I2C fifo.

Description:
	This function waits a calculated amount of time for the 8 byte I2C fifo
	to empty rather than rely on waiting for an ack.
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_P_WaitForDataToLeaveFIFO
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint32_t            numBytes            /* number of bytes to transfer */
);


/***************************************************************************
Summary:
	This function sets up I2C HW ready for HDMI HDCP Auto Ri/Pj link intergrity check feature

Description:
	This function sets up the neccessary I2C HW to use for HDMI HDCP Auto Ri/Pj 
	link intergrity check feature.
		
See Also:

****************************************************************************/
BERR_Code BI2C_P_SetupHdmiHwAccess(
    void                *context,           /* Device channel handle */
    uint32_t            dataTransferFormat, /* Data Transfer Format */
    uint32_t            cnt1,               /* Counter 1 value */
    uint32_t            cnt2                /* Counter 2 value */
);


/***************************************************************************
Summary:
	This function is the interrupt handler for I2C 

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
static void BI2C_P_HandleInterrupt_Isr
(
    void                *pParam1,           /* Device channel handle */
    int                 parm2               /* not used */
);

/***************************************************************************
Summary:
	This function is used for initializing the i2c gpio support.

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
void BI2C_P_GpioInit(void);

#ifdef __cplusplus
}
#endif
 
#endif



