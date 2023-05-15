/*------------------------------------------------------------------------------

 <dev:header>
    Copyright(c) 2009 Sony Corporation.

    $Revision: 1203 $
    $Author: cgleave $

</dev:header>

------------------------------------------------------------------------------*/
#include "drvi2c_feusb.h"
#include "i2c_feusb.h"
#include "i2c_listener.h"

sony_dvb_result_t drvi2c_feusb_Read (sony_dvb_i2c_t * pI2c, uint8_t deviceAddress, uint8_t * pData, uint32_t size, uint8_t mode)
{
    int result = 0 ;
    i2c_listener_t* pListener;// = (i2c_listener_t*) pI2c->user ;

    SONY_DVB_TRACE_I2C_ENTER ("drvi2c_feusb_Read");

    /* Removes compiler warnings. */
    mode = mode ;

    if (!pI2c)
    {
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_ARG);
    }
	else
	{
		pListener = (i2c_listener_t*) pI2c->user ;
	}

    result = i2c_feusb_Read (deviceAddress, pData, size) ;

    if (pListener != NULL) {
        i2c_listener_LogRead (pListener, pI2c, deviceAddress, pData, size, mode);
    }

    switch (result) {
    case I2C_FEUSB_OK:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_OK);
    case I2C_FEUSB_ERROR_ACK:
    case I2C_FEUSB_ERROR_ACCESS:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_I2C);
    default:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_OTHER);
    }
}

sony_dvb_result_t drvi2c_feusb_Write (sony_dvb_i2c_t * pI2c, uint8_t deviceAddress, const uint8_t * pData,
                                      uint32_t size, uint8_t mode)
{
    int result = 0 ;
    i2c_listener_t* pListener;// = (i2c_listener_t*) pI2c->user ;

    SONY_DVB_TRACE_I2C_ENTER ("drvi2c_feusb_Write");

    /* Removes compiler warnings. */
    mode = mode ;

    if (!pI2c)
    {
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_ARG);
    }
	else
	{
		pListener = (i2c_listener_t*) pI2c->user ;
	}

    result = i2c_feusb_Write (deviceAddress, pData, size) ;
    if (pListener != NULL) {
        i2c_listener_LogWrite (pListener, pI2c, deviceAddress, pData, size, mode);
    }

    switch (result) {
    case I2C_FEUSB_OK:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_OK);
    case I2C_FEUSB_ERROR_ACK:
    case I2C_FEUSB_ERROR_ACCESS:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_I2C);
    default:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_OTHER);
    }
}

/* For gateway read access */
sony_dvb_result_t drvi2c_feusb_ReadGw (sony_dvb_i2c_t * pI2c, uint8_t deviceAddress, uint8_t * pData,
                                       uint32_t size, uint8_t mode)
{

    int result = 0 ;
    i2c_listener_t* pListener;// = (i2c_listener_t*) pI2c->user ;

    SONY_DVB_TRACE_I2C_ENTER ("drvi2c_feusb_ReadGw");

    /* Removes compiler warnings. */
    mode = mode ;

    if (!pI2c)
    {
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_ARG);
    }
	else
	{
		pListener = (i2c_listener_t*) pI2c->user ;
	}

    result = i2c_feusb_ReadGw (deviceAddress, pData, size, pI2c->gwAddress, pI2c->gwSub) ;
    if (pListener != NULL) {
        i2c_listener_LogReadGw (pListener, pI2c, deviceAddress, pData, size, mode);
    }

    switch (result) {
    case I2C_FEUSB_OK:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_OK);
    case I2C_FEUSB_ERROR_ACK:
    case I2C_FEUSB_ERROR_ACCESS:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_I2C);
    default:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_OTHER);
    }
}

/* For gateway write access */
sony_dvb_result_t drvi2c_feusb_WriteGw (sony_dvb_i2c_t * pI2c, uint8_t deviceAddress, const uint8_t * pData,
                                        uint32_t size, uint8_t mode)
{
    int result = 0 ;
    i2c_listener_t* pListener;// = (i2c_listener_t*) pI2c->user ;

    SONY_DVB_TRACE_I2C_ENTER ("drvi2c_feusb_WriteGw");

    /* Removes compiler warnings. */
    mode = mode ;

    if (!pI2c)
    {
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_ARG);
    }
	else
	{
		pListener = (i2c_listener_t*) pI2c->user ;
	}

    result = i2c_feusb_WriteGw (deviceAddress, pData, size, pI2c->gwAddress, pI2c->gwSub);
    if (pListener != NULL) {
        i2c_listener_LogWriteGw (pListener, pI2c, deviceAddress, pData, size, mode);
    }

    switch (result) {
    case I2C_FEUSB_OK:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_OK);
    case I2C_FEUSB_ERROR_ACK:
    case I2C_FEUSB_ERROR_ACCESS:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_I2C);
    default:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_OTHER);
    }
}

/* Driver initialization */
sony_dvb_result_t drvi2c_feusb_Initialize (void)
{
    SONY_DVB_TRACE_I2C_ENTER ("drvi2c_feusb_Initialize");

    switch (i2c_feusb_Initialize ()) {
    case I2C_FEUSB_OK:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_OK);
    case I2C_FEUSB_ERROR_ACK:
    case I2C_FEUSB_ERROR_ACCESS:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_I2C);
    default:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_OTHER);
    }
}

/* Driver finalization */
sony_dvb_result_t drvi2c_feusb_Finalize (void)
{
    SONY_DVB_TRACE_I2C_ENTER ("drvi2c_feusb_Finalize");

    switch (i2c_feusb_Finalize ()) {
    case I2C_FEUSB_OK:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_OK);
    case I2C_FEUSB_ERROR_ACK:
    case I2C_FEUSB_ERROR_ACCESS:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_I2C);
    default:
        SONY_DVB_TRACE_I2C_RETURN (SONY_DVB_ERROR_OTHER);
    }
}
