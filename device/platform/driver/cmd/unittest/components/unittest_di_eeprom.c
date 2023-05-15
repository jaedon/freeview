/**
 * unittest_di_eeprom.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include <string.h>

#include "linden_trace.h"
#include "unittest.h"

#include "di_err.h"
#include "di_uart.h"
#include "di_eeprom.h"
#include "vkernel.h"
#include "taskdef.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DI_EEPROM

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

static UT_RESULT ut_di_eeprom_get_capabilities(void)
{
	DI_ERR_CODE 	result;
	unsigned long	ulEepromSize = 0;
	int 			ret = UT_FAIL;

	result = DI_EEPROM_GetCapabilities(&ulEepromSize);
	DONE_ERR(result != DI_ERR_OK);
	PrintDebug("EEPROM size : %d(0x%04X)\n", ulEepromSize, ulEepromSize);

	ret = UT_OK;

done:
	return ret;
}

static UT_RESULT ut_di_eeprom_read_write(void)
{

	DI_ERR_CODE 	result;
	HUINT8			*pReadBuf;
	HUINT8			*pWriteBuf;
	unsigned long	ulEepromSize = 0;
	int				mem_result;
	int 			ret = UT_FAIL;

	DI_UART_Print("%s start\n", __func__);

	result = DI_EEPROM_GetCapabilities(&ulEepromSize);
	DONE_ERR(result != DI_ERR_OK);

	pReadBuf = VK_MEM_Alloc(ulEepromSize);
	DONE_ERR(pReadBuf == NULL);
	VK_memset(pReadBuf, 0, ulEepromSize);

	pWriteBuf = VK_MEM_Alloc(ulEepromSize);
	DONE_ERR(pWriteBuf == NULL);

	VK_memset(pWriteBuf, 0, ulEepromSize);

	DI_UART_Print("read eeprom\n");
	result = DI_EEPROM_Read(0, pReadBuf, ulEepromSize);
	DONE_ERR(result != DI_ERR_OK);
	DI_UART_Dump(pReadBuf, ulEepromSize, 16);

	DI_UART_Print("write eeprom to zero\n");
	result = DI_EEPROM_Write(0, pWriteBuf, ulEepromSize);
	DONE_ERR(result != DI_ERR_OK);

	VK_memset(pWriteBuf, 0xFF, ulEepromSize);

	DI_UART_Print("write eeprom to 0xFF\n");
	result = DI_EEPROM_Write(0, pWriteBuf, ulEepromSize);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Print("read flash block\n");
	result = DI_EEPROM_Read(0, pReadBuf, ulEepromSize);

	DI_UART_Print("compare flash block\n");
	mem_result = VK_memcmp(pReadBuf, pWriteBuf, ulEepromSize);
	DONE_ERR(mem_result != 0);

	VK_MEM_Free(pReadBuf);
	VK_MEM_Free(pWriteBuf);

	ret = UT_OK;

done:
	return ret;

}

static UT_RESULT ut_di_eeprom_erase(void)
{

	DI_ERR_CODE 	result;
	HUINT8			*pReadBuf;
	HUINT8			*pWriteBuf;
	unsigned long	ulEepromSize = 0;
	int 			ret = UT_FAIL;

	DI_UART_Print("%s start\n", __func__);

	result = DI_EEPROM_GetCapabilities(&ulEepromSize);
	DONE_ERR(result != DI_ERR_OK);

	pReadBuf = VK_MEM_Alloc(ulEepromSize);
	DONE_ERR(pReadBuf == NULL);
	VK_memset(pReadBuf, 0, ulEepromSize);

	pWriteBuf = VK_MEM_Alloc(ulEepromSize);
	DONE_ERR(pWriteBuf == NULL);
	VK_memset(pWriteBuf, 0, ulEepromSize);

	DI_UART_Print("erase eeprom\n");
	result = DI_EEPROM_EraseAll();
	DONE_ERR(result != DI_ERR_OK);

	result = DI_EEPROM_Read(0, pReadBuf, ulEepromSize);
	DONE_ERR(result != DI_ERR_OK);
	DI_UART_Dump(pReadBuf, ulEepromSize, 16);

	VK_MEM_Free(pReadBuf);
	VK_MEM_Free(pWriteBuf);

	ret = UT_OK;

done:
	return ret;

}

static UT_MENU_ITEM_T menu_eeprom[] =
{
	MENU_START,
	MENU_FUNC(ut_di_eeprom_get_capabilities,1),
	MENU_FUNC(ut_di_eeprom_read_write,5),
	MENU_FUNC(ut_di_eeprom_erase,5),
	MENU_END,
};

static UT_MENU_ITEM_T menu_main_eeprom[] =
{
	MENU_SUB(menu_eeprom),
};

void UT_EEPROM_Init(void)
{
	HINT32 	i;
	HINT32	array_size;

	array_size = sizeof(menu_main_eeprom)/sizeof(menu_main_eeprom[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_eeprom[i]);
	}

	array_size = sizeof(menu_eeprom)/sizeof(menu_eeprom[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_eeprom[i]);
	}
}

