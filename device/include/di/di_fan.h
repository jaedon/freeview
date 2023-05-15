/**
 * @file	di_fan.h
 * @brief	FAN module DI layer header file
 * @version 	$Revision: 0.9.0 $
 * @date 	$Date: 2012/05/08 12:00:00 $
 * @author
 */

/** @defgroup	DI_FAN	FAN COMMON IF OF DI LAYER
 *  This is DI_FAN module of DI layer.
 *  @{
 */


#ifndef __DI_FAN_H__
#define __DI_FAN_H__

/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"
#include "di_err.h" /* if di module is included, di_err.h is included first!Later this line will be removed.*/

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/

typedef enum
{
	DI_FAN_MODE_AUTO = 0,
	DI_FAN_MODE_MANUAL
} DI_FAN_MODE_e;

typedef enum eDI_FAN_CTRL
{
	DI_FAN_CTRL_STOP = 0,
	DI_FAN_CTRL_LOW,
	DI_FAN_CTRL_LOW_MIDDLE,
	DI_FAN_CTRL_MIDDLE,
	DI_FAN_CTRL_MIDDLE_HIGH,
	DI_FAN_CTRL_HIGH
} DI_FAN_CTRL_e;

DI_ERR_CODE DI_FAN_SetMode(DI_FAN_MODE_e eMode);
DI_ERR_CODE DI_FAN_Control(DI_FAN_CTRL_e eControl);
DI_ERR_CODE DI_FAN_GetSpeed(HULONG *pulSpeed_Freq);

#endif /* __DI_FAN_H__ */
