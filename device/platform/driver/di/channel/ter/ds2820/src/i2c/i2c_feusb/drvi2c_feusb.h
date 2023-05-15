/*------------------------------------------------------------------------------

 <dev:header>
    Copyright(c) 2009 Sony Corporation.

    $Revision: 840 $
    $Author: sking $

</dev:header>

------------------------------------------------------------------------------*/

#ifndef DRVI2C_FEUSB_H
#define DRVI2C_FEUSB_H

#include <sony_dvb.h>
#include <sony_dvb_i2c.h>

/* Bridge of I2C code */

sony_dvb_result_t drvi2c_feusb_Read (sony_dvb_i2c_t * pI2c, uint8_t deviceAddress, uint8_t * pData,
                                     uint32_t size, uint8_t mode);

sony_dvb_result_t drvi2c_feusb_Write (sony_dvb_i2c_t * pI2c, uint8_t deviceAddress, const uint8_t * pData,
                                      uint32_t size, uint8_t mode);

sony_dvb_result_t drvi2c_feusb_ReadGw (sony_dvb_i2c_t * pI2c, uint8_t deviceAddress, uint8_t * pData,
                                       uint32_t size, uint8_t mode);

sony_dvb_result_t drvi2c_feusb_WriteGw (sony_dvb_i2c_t * pI2c, uint8_t deviceAddress, const uint8_t * pData,
                                        uint32_t size, uint8_t mode);

sony_dvb_result_t drvi2c_feusb_Initialize (void);
sony_dvb_result_t drvi2c_feusb_Finalize (void);

#endif /* DRVI2C_FEUSB_H */
