/**
 * drv_key.h
*/

/**
 * @defgroup
 * @author
 * @note
 * @brief
 * @file
*/

#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__


/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"
#include "di_key.h"
#if defined(CONFIG_FRONTKEY_GPIO)
#include "drv_gpio.h"
#endif
/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/* Sunwoo */
typedef struct IRMessage
{
	DI_KEY_DEVICE eDevice;
	DI_KEY_TYPE eKeyType;
	HUINT8 	        		ucDecodedCode;    		/* 디코딩된 코드 */
	HUINT32 				ulRepeatCounter;		   /* 리모컨 키 repetition 을 처리 */
#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	HBOOL				bMultiCustomMessage;
	HUINT16 				usDecodedCustomCode;
	HUINT32 				ulNextState;
#endif
}IR_MSG;

#if defined(CONFIG_FRONTKEY_GPIO)
typedef struct GPIO_KeyMappingTable_t_
{
	GPIO_ID_t 	gpio_id;
	char 	*gpio_name;
	unsigned char 	ucKeyID;
	GPIO_STATE_t gpioState;
	DI_KEY_TYPE key_type;

} GPIO_KeyMappingTable_t;
#endif

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_KEY_Init(void);
DRV_Error DRV_KEY_SendFrontKeyMessage(unsigned char ulKeyID, unsigned char nKeyType);
DRV_Error DRV_KEY_SendMultiKeyMessage(unsigned long ulKeyMap, unsigned char ucKeyType);
HUINT32 DRV_KEY_GetIRTaskQueueID(void);
void Drv_KeyEmulate(HUINT32 ulKeyCode);

#endif

