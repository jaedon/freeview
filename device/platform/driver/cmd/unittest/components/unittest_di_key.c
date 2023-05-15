/**
 * unittest_di_key.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "unittest.h"

#include "di_err.h"
#include "di_uart.h"
#include "di_key.h"
#include "taskdef.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DI_KEY


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

static pfnDI_KEY_Notify p_cbKeyNotti(DI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, DI_KEY_TYPE eKeyType)
{
	DI_UART_Print("[p_cbKeyNotti] notify OK\n");
	DI_UART_Print("Key Device = %d, KeyCode = 0x%08X, KeyType = %d\n", eDevice, ulKeyCode, eKeyType); 
}

static UT_RESULT ut_di_key_register_key_event_callback(void)
{
	int	ret = UT_FAIL;

	DI_KEY_RegisterKeyEventCallback(p_cbKeyNotti);
	
	ret = UT_OK;
		
done:
	return ret;
	
}

static UT_MENU_ITEM_T menu_key[] =
{
	MENU_START,
	MENU_FUNC(ut_di_key_register_key_event_callback,1),
	MENU_END,
};

static UT_MENU_ITEM_T menu_main_key[] =
{
	MENU_SUB(menu_key),
};

void UT_KEY_Init(void)
{
	HINT32 	i;
	HINT32	array_size;

	array_size = sizeof(menu_main_key)/sizeof(menu_main_key[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_key[i]);
	}

	array_size = sizeof(menu_key)/sizeof(menu_key[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_key[i]);
	}
}

