/**
 * unittest_di_scart.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"

#include "unittest.h"
#include "di_uart.h"
#include "di_scart.h"
#include "di_err.h"



/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/


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

static UT_RESULT ut_di_scart_getSetting(void)
{
#if defined(CONFIG_SCART)
	DI_SCART_SETTINGS settings;
	
	DI_SCART_GetSettings(&settings);
	DI_UART_Print("setting.eMode : %d\n", settings.eMode);
	DI_UART_Print("setting.stNormalMode.eTvAspectRatioOutput : %d\n", settings.stNormalMode.eTvAspectRatioOutput);
	DI_UART_Print("setting.stNormalMode.eVcrAspectRatioOutput : %d\n", settings.stNormalMode.eVcrAspectRatioOutput);
	DI_UART_Print("setting.stNormalMode.eTvAspectRatioInput : %d\n", settings.stNormalMode.eTvAspectRatioInput);
	DI_UART_Print("setting.stNormalMode.eTvVideoOutput : %d\n", settings.stNormalMode.eTvVideoOutput);
	DI_UART_Print("setting.stNormalMode.eVcrVideoOutput : %d\n", settings.stNormalMode.eVcrVideoOutput);
	DI_UART_Print("setting.stNormalMode.eTvVideoInput : %d\n", settings.stNormalMode.eTvVideoInput);	
	
	return UT_OK;
#endif
	DI_UART_Print("CONFIG_SCART disabled!\n");
	return UT_FAIL;
}

static UT_MENU_ITEM_T	menu_di_scart[] = 
{
	MENU_START,
	MENU_FUNC(ut_di_scart_getSetting, 5),	
	MENU_END,
};


static UT_MENU_ITEM_T	menu_main_scart[] = 
{
	MENU_SUB(menu_di_scart),
};

void UT_DI_SCART_Init(void)
{
	HINT32 i;
	HINT32 array_size;
	
	array_size = sizeof(menu_main_scart)/sizeof(menu_main_scart[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_scart[i]);
	}
	array_size = sizeof(menu_di_scart)/sizeof(menu_di_scart[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_di_scart[i]);
	}
}



