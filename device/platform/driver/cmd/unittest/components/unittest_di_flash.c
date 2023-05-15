/**
 * unittest_di_flash.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "stdlib.h"
#include "string.h"

#include "linden_trace.h"
#include "unittest.h"

#include "di_err.h"
#include "di_uart.h"
#include "di_flash.h"
#include "flash_map.h"
#include "vkernel.h"
#include "taskdef.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

#define DEBUG_MODULE        TRACE_MODULE_DI_FLASH


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

static UT_RESULT ut_di_flash(void)
{
	int			ret = -1;
    DI_ERR_CODE result;
	int			mem_result;
	HUINT8		*pReadBuf;
	HUINT8		*pWriteBuf;
	HUINT32			currentTime;

	DI_UART_Print("%s start\n", __func__);
#if 0//defined(CONFIG_NOR_FLASH) && !defined(CONFIG_NAND_FLASH)  TODO : Nor only 고려하여 다시 작성.
	pReadBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pReadBuf == NULL);
	VK_memset(pReadBuf, 0, FLASH_BLOCK_SIZE);

	pWriteBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pWriteBuf == NULL);
	VK_memset(pWriteBuf, 0, FLASH_BLOCK_SIZE);

	currentTime = 100;
	DI_UART_Print("read flash block\n");
	result = DI_FLASH_Read(FLASH_UBI_AREA_START, pReadBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);
	DI_UART_Dump(pReadBuf, FLASH_BLOCK_SIZE, 16);

	DI_UART_Print("write flash block to zero\n");
	result = DI_FLASH_Write(FLASH_UBI_AREA_START, pWriteBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Print("write flash block to 0xFF\n");
	VK_memset(pWriteBuf, 0xFF, FLASH_BLOCK_SIZE);
	result = DI_FLASH_Write(FLASH_UBI_AREA_START, pWriteBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Print("read flash block\n");
	result = DI_FLASH_Read(FLASH_UBI_AREA_START, pReadBuf, FLASH_BLOCK_SIZE);

	DI_UART_Print("compare flash block\n");
	mem_result = VK_memcmp(pReadBuf, pWriteBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(mem_result != 0);
#endif
#if defined(CONFIG_EMMC_FLASH)
#if 0
	pReadBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pReadBuf == NULL);
	VK_memset(pReadBuf, 0, FLASH_BLOCK_SIZE);

	pWriteBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pWriteBuf == NULL);
	VK_memset(pWriteBuf, 0, FLASH_BLOCK_SIZE);

	currentTime = 100;
	DI_UART_Print("read flash block\n");
	result = DI_EMMC_Read(FLASH_UBI_AREA_START, pReadBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);
	DI_UART_Dump(pReadBuf, FLASH_BLOCK_SIZE, 16);

	DI_UART_Print("write flash block to zero\n");
	result = DI_EMMC_Write(FLASH_UBI_AREA_START, pWriteBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Print("write flash block to 0xFF\n");
	VK_memset(pWriteBuf, 0xFF, FLASH_BLOCK_SIZE);
	result = DI_EMMC_Write(FLASH_UBI_AREA_START, pWriteBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Print("read flash block\n");
	result = DI_EMMC_Read(FLASH_UBI_AREA_START, pReadBuf, FLASH_BLOCK_SIZE);

	DI_UART_Print("compare flash block\n");
	mem_result = VK_memcmp(pReadBuf, pWriteBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(mem_result != 0);
#endif
#endif
#if defined(CONFIG_NAND_FLASH)
	pReadBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pReadBuf == NULL);
	VK_memset(pReadBuf, 0, FLASH_BLOCK_SIZE);

	pWriteBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pWriteBuf == NULL);
	VK_memset(pWriteBuf, 0, FLASH_BLOCK_SIZE);

	currentTime = 100;
	DI_UART_Print("read flash block\n");
	result = DI_NANDFLASH_Read(FLASH_UBI_AREA_START, pReadBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);
	DI_UART_Dump(pReadBuf, FLASH_BLOCK_SIZE, 16);

	DI_UART_Print("write flash block to zero\n");
	result = DI_NANDFLASH_Write(FLASH_UBI_AREA_START, pWriteBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Print("write flash block to 0xFF\n");
	VK_memset(pWriteBuf, 0xFF, FLASH_BLOCK_SIZE);
	result = DI_NANDFLASH_Write(FLASH_UBI_AREA_START, pWriteBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Print("read flash block\n");
	result = DI_NANDFLASH_Read(FLASH_UBI_AREA_START, pReadBuf, FLASH_BLOCK_SIZE);

	DI_UART_Print("compare flash block\n");
	mem_result = VK_memcmp(pReadBuf, pWriteBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(mem_result != 0);
#endif

	VK_MEM_Free(pReadBuf);
	VK_MEM_Free(pWriteBuf);

	ret = 0;
done:
	return ret;
}

static UT_RESULT ut_di_flash_erase(void)
{
    DI_ERR_CODE result;
	int			ret = -1;
	HUINT8		*pReadBuf;
	HUINT8		*pWriteBuf;
	HUINT32			currentTime;

	DI_UART_Print("%s start\n", __func__);

#if defined(CONFIG_EMMC_FLASH)
	DI_UART_Print("Not implemented!!!!!\n");
#else
#if defined(CONFIG_NOR_FLASH) || defined(CONFIG_DI10)
	pReadBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pReadBuf == NULL);
	VK_memset(pReadBuf, 0, FLASH_BLOCK_SIZE);

	pWriteBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pWriteBuf == NULL);
	VK_memset(pWriteBuf, 0, FLASH_BLOCK_SIZE);

	currentTime = 100;
	DI_UART_Print("erase flash block\n");
	result = DI_FLASH_EraseBlock(0);
	DONE_ERR(result != DI_ERR_OK);

	result = DI_FLASH_Read(0, pReadBuf, FLASH_BLOCK_SIZE);
	DONE_ERR(result != DI_ERR_OK);
	DI_UART_Dump(pReadBuf, FLASH_BLOCK_SIZE, 16);
#else

	pReadBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pReadBuf == NULL);
	VK_memset(pReadBuf, 0, FLASH_BLOCK_SIZE);

	pWriteBuf = VK_MEM_Alloc(FLASH_BLOCK_SIZE);
	DONE_ERR(pWriteBuf == NULL);
	VK_memset(pWriteBuf, 0, FLASH_BLOCK_SIZE);

#endif

	VK_MEM_Free(pReadBuf);
	VK_MEM_Free(pWriteBuf);
#endif
	ret = 0;
done:
	return ret;
}

static UT_MENU_ITEM_T	menu_flash[] =
{
	MENU_START,
	MENU_FUNC(ut_di_flash, 20),
	MENU_FUNC(ut_di_flash_erase, 20),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_flash[] =
{
	MENU_SUB(menu_flash),
};


void UT_FLASH_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_flash)/sizeof(menu_main_flash[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_flash[i]);
	}
	array_size = sizeof(menu_flash)/sizeof(menu_flash[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_flash[i]);
	}
}


