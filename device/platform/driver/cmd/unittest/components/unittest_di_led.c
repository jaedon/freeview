/**
 * unittest_led.c
*/

/**
 * @author		Larry
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"

#include "unittest.h"

#include "di_err.h"
#include "di_led.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

#define DEBUG_MODULE	TRACE_MODULE_DI_LED


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/



////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////DI_LED TEST START//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
static int ut_di_ledPowerState(void)
{
	int				ret = UT_FAIL;
    DI_ERR_CODE 	result;
	HUINT32 cap;

	result = DI_LED_GetCapability(&cap);
	DONE_ERR(result != DI_ERR_OK);
	DI_UART_Print("LED Capability : 0x%x \n", cap);

	DI_UART_Print("Turn off LED\n");
	result = DI_LED_SetPowerLedState(DI_LED_OFF);
	DONE_ERR(result != DI_ERR_OK);
	DI_UART_Print("Do you see that LED is off ?\n");
	VK_TASK_Sleep(3000);

	DI_UART_Print("DI_LED_STANDBY\n");
	result = DI_LED_SetPowerLedState(DI_LED_STANDBY);
	DONE_ERR(result != DI_ERR_OK);
	VK_TASK_Sleep(3000);

	DI_UART_Print("DI_LED_STANDBY_LEDLOW\n");
	result = DI_LED_SetPowerLedState(DI_LED_STANDBY_LEDLOW);
	DONE_ERR(result != DI_ERR_OK);
	VK_TASK_Sleep(3000);

	DI_UART_Print("DI_LED_WATCHING_LEDLOW\n");
	result = DI_LED_SetPowerLedState(DI_LED_WATCHING_LEDLOW);
	DONE_ERR(result != DI_ERR_OK);
	VK_TASK_Sleep(3000);

	DI_UART_Print("DI_LED_RECORDING\n");
	result = DI_LED_SetPowerLedState(DI_LED_RECORDING);
	DONE_ERR(result != DI_ERR_OK);
	VK_TASK_Sleep(3000);

	DI_UART_Print("DI_LED_WATCHING\n");
	result = DI_LED_SetPowerLedState(DI_LED_WATCHING);
	DONE_ERR(result != DI_ERR_OK);
	DI_UART_Print("Do you see that LED is ON ?\n");
	VK_TASK_Sleep(3000);



	ret = UT_OK;

done:
    PrintExit();
	return ret;
}

static int ut_di_dimLevel(void)
{
	int				ret = UT_FAIL;
    DI_ERR_CODE 	result;
	HUINT32 cap;

	result = DI_LED_SetLedsDimmingLevel(DI_LED_NUM2 , 100);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
    PrintExit();
	return ret;
}


static UT_MENU_ITEM_T	menu_led[] =
{
	MENU_START,
	MENU_FUNC(ut_di_ledPowerState, 18),
	MENU_FUNC(ut_di_dimLevel, 5),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_led[] =
{
	MENU_SUB(menu_led),
};


void UT_LED_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_led)/sizeof(menu_main_led[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_led[i]);
	}
	array_size = sizeof(menu_led)/sizeof(menu_led[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_led[i]);
	}
}



