/**
 * unittest_di_crypt.c
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
#include "di_crypt.h"

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

static UT_RESULT ut_Crypt_all(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;

	stCrypt_t stEncryptInfo;
	stCrypt_t stDecryptInfo;
	const char secret[] = "This is a very secret key : !@&^%*&^%#@&^%*@%*&!^@%!#@*$";
	
	/* Initialize */
	VK_MEM_Memset(&stEncryptInfo, 0x00, sizeof(stCrypt_t));
	/* Set crypt info struct */
	stEncryptInfo.ulBuffSize = sizeof(secret);

	/* Memory Allocation */
	stEncryptInfo.pSrcBuffer = (HUINT8*)DI_CRYPT_MemAlloc(sizeof(secret));
	if(stEncryptInfo.pSrcBuffer == NULL)
	{
		DI_UART_Print("[%s:%d] pSrcBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
		goto done;
	}

	stEncryptInfo.pDstBuffer = (HUINT8*)DI_CRYPT_MemAlloc(sizeof(secret));
	if(stEncryptInfo.pDstBuffer == NULL)
	{
		DI_UART_Print("[%s:%d] pDstBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
		goto done;
	}

	VK_MEM_Memcpy(stEncryptInfo.pSrcBuffer, &secret[0], sizeof(secret));

	DI_UART_Print("%s \n", stEncryptInfo.pSrcBuffer);

	result = DRV_CRYPT_Encrypt(&stEncryptInfo);
	if(DI_ERR_OK != result)
	{
		DI_UART_Print("[%s:%d] Error Encrypt \n\r", __FUNCTION__, __LINE__);
		goto done;
	}
	
	VK_TASK_Sleep(100);


	/* Initialize */
	VK_MEM_Memset(&stDecryptInfo, 0x00, sizeof(stDecryptInfo));
	/* Set crypt info struct */
	stDecryptInfo.ulBuffSize = sizeof(secret);

	/* Memory Allocation */
	stDecryptInfo.pSrcBuffer = (HUINT8*)DI_CRYPT_MemAlloc(100);
	if(stDecryptInfo.pSrcBuffer == NULL)
	{
		DI_UART_Print("[%s:%d] pSrcBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
		goto done;
	}

	stDecryptInfo.pDstBuffer = (HUINT8*)DI_CRYPT_MemAlloc(100);
	if(stDecryptInfo.pDstBuffer == NULL)
	{
		DI_UART_Print("[%s:%d] pDstBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
		goto done;
	}
	
	VK_MEM_Memcpy(stDecryptInfo.pSrcBuffer, stEncryptInfo.pDstBuffer, sizeof(secret));
	DI_UART_Print("%s \n", stDecryptInfo.pSrcBuffer);

	result = DRV_CRYPT_Decrypt(&stDecryptInfo);
	if(DI_ERR_OK != result)
	{
		DI_UART_Print("[%s:%d] Error Decrypt \n\r", __FUNCTION__, __LINE__);
		goto done;
	}
	VK_TASK_Sleep(100);

	DI_UART_Print("%s \n", stDecryptInfo.pDstBuffer);

	if(memcmp(stDecryptInfo.pDstBuffer, secret, sizeof(secret)))
	{
		DI_UART_Print("[%s:%d] Wrong decrypt!!!! \n\r", __FUNCTION__, __LINE__);
		goto done;
	}
	
	DI_CRYPT_MemFree(stEncryptInfo.pSrcBuffer);
	DI_CRYPT_MemFree(stEncryptInfo.pDstBuffer);

	DI_CRYPT_MemFree(stDecryptInfo.pSrcBuffer);
	DI_CRYPT_MemFree(stDecryptInfo.pDstBuffer);

	ret = UT_OK;
done:
	return ret;
}



static UT_MENU_ITEM_T	menu_crypt[] =
{
	MENU_START,
	MENU_FUNC(ut_Crypt_all, 1),	
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_crypt[] =
{
	MENU_SUB(menu_crypt),
};

void UT_CRYPT_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_crypt)/sizeof(menu_main_crypt[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_crypt[i]);
	}
	array_size = sizeof(menu_crypt)/sizeof(menu_crypt[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_crypt[i]);
	}
}


