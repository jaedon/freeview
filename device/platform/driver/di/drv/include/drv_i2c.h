/**
 * drv_i2c.h
*/

/**
 * @defgroup		DRV_I2C DRV_I2C : DRV_I2C apis
 * @author		Jin-hong Kim
 * @note			DRV_I2C APIs
 * @brief			Define DRV_I2C APIs
 * @file 			drv_i2c.h
*/

#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__


/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

#include "i2c_bus_index.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum DRV_I2C_CLOCK_e
{
    DRV_I2C_CLOCK_47K = 47,
	DRV_I2C_CLOCK_50K = 50,
	DRV_I2C_CLOCK_93K = 93,
	DRV_I2C_CLOCK_100K = 100,
	DRV_I2C_CLOCK_187K = 187,
	DRV_I2C_CLOCK_200K = 200,
	DRV_I2C_CLOCK_375K = 375,
	DRV_I2C_CLOCK_400K = 400,
	DRV_I2C_CLOCK_MAX
} DRV_I2C_CLOCK_E;


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_I2c_Init(void);

 DRV_Error DRV_I2c_ReadA24( HUINT32  ulI2cChannel,
										 HUINT16 usChipAddr,
										 HUINT32 usSubAddr,
										 HUINT8 *  ucBuffer,
										 HUINT32  ulLength
										 );
DRV_Error DRV_I2c_WriteA24( HUINT32  ulI2cChannel,  
                                        HUINT16 usChipAddr,
                                        HUINT16 usSubAddr,
                                        HUINT8 *  ucBuffer,                               
                                        HUINT32  ulLength  
                                        );

DRV_Error DRV_I2c_ReadA16( HUINT32  ulI2cChannel,  
                                        HUINT16 usChipAddr,
                                        HUINT16 usSubAddr,
                                        HUINT8 *  ucBuffer,                               
                                        HUINT32  ulLength  
                                        );
DRV_Error DRV_I2c_WriteA16( HUINT32  ulI2cChannel,  
                                        HUINT16 usChipAddr,
                                        HUINT16 usSubAddr,
                                        HUINT8 *  ucBuffer,                               
                                        HUINT32  ulLength  
                                        );

DRV_Error DRV_I2c_ReadA8_NoRetry( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT8 usSubAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        );
DRV_Error DRV_I2c_ReadA8( HUINT32  ulI2cChannel,  
                                        HUINT16 usChipAddr,
                                        HUINT8 usSubAddr,
                                        HUINT8 *  ucBuffer,                               
                                        HUINT32  ulLength  
                                        );
DRV_Error DRV_I2c_WriteA8( HUINT32  ulI2cChannel,  
                                    HUINT16 usChipAddr,
                                    HUINT8 usSubAddr,
                                    HUINT8 *  ucBuffer,                               
                                    HUINT32  ulLength  
                                    );
DRV_Error DRV_I2c_WriteA8NoAck( HUINT32  ulI2cChannel,  
                                    HUINT16 usChipAddr,
                                    HUINT8 usSubAddr,
                                    HUINT8 *  ucBuffer,                               
                                    HUINT32  ulLength  
                                    );
DRV_Error DRV_I2c_ReadNoAddr( HUINT32  ulI2cChannel,  
                                        HUINT16 usChipAddr,
                                        HUINT8 *  ucBuffer,                               
                                        HUINT32  ulLength  
                                        );
DRV_Error DRV_I2c_WriteNoAddr( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        );
DRV_Error DRV_I2c_SetClock( HUINT32  ulI2cChannel, DRV_I2C_CLOCK_E eClock);

#endif /* !__DRV_I2C_H__ */

