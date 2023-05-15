/**
 * drv_pwm.h
*/

/**
 * @defgroup		DRV_PWM DRV_PWM : Pwm driver module
 * @author		Jin-hong Kim
 * @note			PWM driver APIs
 * @brief			Define broadcom dependent PWM Driver
 * @file			drv_pwm.h

*/


#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

extern DRV_Error DRV_PWM_Init(HUINT32 ulChannelNumber);
extern DRV_Error DRV_PWM_Start(HUINT32 ulChannelNumber);
extern DRV_Error DRV_PWM_Stop(HUINT32 ulChannelNumber);
extern DRV_Error DRV_PWM_GetControlWord(HUINT32 ulChannelNumber, HUINT32 *pulControlWord);
extern DRV_Error DRV_PWM_SetControlWord(HUINT32 ulChannelNumber, HUINT32 ulControlWord);
extern DRV_Error DRV_PWM_GetPeriodInterval(HUINT32 ulChannelNumber, HUINT32 *pulPeriodInterval);
extern DRV_Error DRV_PWM_SetPeriodInterval(HUINT32 ulChannelNumber, HUINT32 ulPeriodInterval);
extern DRV_Error DRV_PWM_GetOnInterval(HUINT32 ulChannelNumber, HUINT32 *pulOnInterval);
extern DRV_Error DRV_PWM_SetOnInterval(HUINT32 ulChannelNumber, HUINT32 ulOnInterval);


#endif /* !__DRV_PWM_H__ */


