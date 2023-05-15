/**
 * unittest_di_uart.c
*/

/**
 * @author		Larry
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "unittest.h"
#include "di_uart.h"
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

static UT_RESULT ut_uart_all(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	char *string = "UART";
	char str;

	DI_UART_Print("Unittest : Type one letter to test ReadByte \n");

	result = DI_UART_ReadByte(&str);
	if(result != DI_ERR_OK)
	{
		ret = UT_FAIL;
		goto done;
	}

	DI_UART_Print("Unittest : recieved   ");

	result = DI_UART_WriteByte(str);
	if(result != DI_ERR_OK)
	{
		ret = UT_FAIL;
		goto done;
	}

	DI_UART_Print("\n");

	result = DI_UART_Write(string, 4);
	if(result != DI_ERR_OK)
	{
		ret = UT_FAIL;
		goto done;
	}

	ret = UT_OK;
done:
	return ret;
}



static UT_MENU_ITEM_T	menu_uart[] =
{
	MENU_START,
	MENU_FUNC(ut_uart_all, 1),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_uart[] =
{
	MENU_SUB(menu_uart),
};

void UT_UART_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_uart)/sizeof(menu_main_uart[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_uart[i]);
	}
	array_size = sizeof(menu_uart)/sizeof(menu_uart[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_uart[i]);
	}
}


