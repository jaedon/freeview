
/********************************************************************
 * Workfile   : unittest_di_usb.c
 * Project    : ALi BASE
 * Author     :
 * Description: Commands Template
 *
 *                                 Copyright (c) 2014 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#define _______________________________________________________________________
#define ____HEADER_FILES____
#include "linden_trace.h"

#include "unittest.h"

#include "di_usb.h"
#include "vkernel.h"
#include "di_uart.h"

#define DEBUG_MODULE	TRACE_MODULE_DI_USB


#define _______________________________________________________________________
#define ____MACRO_DEFINITION____


#define _______________________________________________________________________
#define ____EXTERNAL_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____EXTERNAL_FUNCTION_DEFINITION____


#define _______________________________________________________________________
#define ____TYPE_DEFINITION____


#define _______________________________________________________________________
#define ____GLOBAL_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____STATIC_VARIABLE_DEFINITION____

static unsigned char bConnected;

#define _______________________________________________________________________
#define ____STATIC_FUNCTION_DEFINITION____


#define _______________________________________________________________________
#define ____STATIC_FUNCTION_BODY____

static DI_ERR_CODE P_UNITTEST_USB_CallBack(DI_USB_Event_t etUsbEventType, void *pvUsbInfo)
{
	switch(etUsbEventType)
	{
	case DI_USB_Event_Insert:
		bConnected = TRUE;
		break;
	case DI_USB_Event_Extract:
	case DI_USB_Event_OverCurrent :
	default:
		break;
	}

	return DI_ERR_OK;
}

#define _______________________________________________________________________
#define ____GLOBAL_FUNCTION_BODY____


static int ut_di_usb(void)
{
	int count = 30*1000;
	int func_ret = -1;

	DI_USB_RegisterCallback(P_UNITTEST_USB_CallBack);

	DI_UART_Print("Plug-in USB\n");
	while(count)
	{
		if (bConnected)
		{
			func_ret = 0;
			break;
		}
		VK_TASK_Sleep(100);
		count -= 100;
	}

	return func_ret;
}

static UT_MENU_ITEM_T	menu_usb[] =
{
	MENU_START,
	MENU_FUNC(ut_di_usb, -1),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_usb[] =
{
	MENU_SUB(menu_usb),
};

void UT_USB_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_usb)/sizeof(menu_main_usb[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_usb[i]);
	}
	array_size = sizeof(menu_usb)/sizeof(menu_usb[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_usb[i]);
	}
}

