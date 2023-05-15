/**
 * unittest_di_frontPannel.c
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
#include "di_front_panel.h"

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

static UT_RESULT ut_frontPannel_getCapability(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	DI_FRONT_PANEL_CAP stPanelCapa;

	result = DI_FRONT_PANEL_GetCapability(NULL);
    if(result == DI_ERR_OK)
	{
		goto done;
	}

	result = DI_FRONT_PANEL_GetCapability(&stPanelCapa);
    if(result != DI_ERR_OK)
	{
		goto done;
	}
	DI_UART_Print("Display NULL \n");
	result = DI_FRONT_PANEL_Display(NULL);
	if(result == DI_ERR_OK)
	{
		goto done;
	}

	ret = UT_OK;
done:
	return ret;
}



static UT_MENU_ITEM_T	menu_frontPannel[] =
{
	MENU_START,
	MENU_FUNC(ut_frontPannel_getCapability, 1),

	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_frontPannel[] =
{
	MENU_SUB(menu_frontPannel),
};

void UT_FRONT_PANEL_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_frontPannel)/sizeof(menu_main_frontPannel[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_frontPannel[i]);
	}
	array_size = sizeof(menu_frontPannel)/sizeof(menu_frontPannel[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_frontPannel[i]);
	}
}


