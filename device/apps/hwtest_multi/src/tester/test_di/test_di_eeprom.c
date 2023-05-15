/**
 * test_di_eeprom.c
*/

/**
 * @defgroup		TEST_EEPROM
 * @author		Tae-Hyuk Kang
 * @note
 * @brief
 * @file 			test_di_eeprom.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "di_err.h"
#include "di_eeprom.h"
#include "vkernel.h"
#include "taskdef.h"
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_TEST_DI_EEPROM
#ifdef DEBUG_TEST_DI_EEPROM
#define PrintDebug			printf
#define PrintError			printf
#else
#define PrintDebug			(...)
#define PrintError			printf
#endif

#define DEBUG_TEST_DI_EEPROM_ENTER_LEAVE
#ifdef DEBUG_TEST_DI_EEPROM_ENTER_LEAVE
#define PrintEnter()			printf("+++++%s\n", __func__)
#define PrintExit()			printf("------%s\n", __func__)
#else
#define PrintEnter()
#define PrintExit()
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
#define EEPROM_TEST_BUFFER	180


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
HBOOL eeprom_test_passed = FALSE;

unsigned long taskIdEepRead;
unsigned long taskIdEepWrite;

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

static void TEST_EEPROM_SelectTestCase(HUINT32 ulTestNumber);
static void TEST_EEPROM_Menu(void);

void TEST_DI_EEPROM(void)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;

    do {
        TEST_EEPROM_Menu(); /* print menu */

        ulRet = TEST_GetNumber(&ulTestNumber);
        if ( ulRet == DI_ERR_ERROR || ulTestNumber == 0)
        {
        	/* */
            break;
        }
		else if (ulRet == DI_ERR_OK)
		{
            TEST_EEPROM_SelectTestCase(ulTestNumber);
        }
        else
        {
            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
        }
    } while (1);

}


static void TEST_EEPROM_Menu(void)
{
	printf( "*============================================================*\n"); /* 70 */
	printf( "*    DD TEST TOOL/CHANNEL                                    *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    00. Quit                                                *\n"); /* 70 */
	printf( "*    01. test_di_eeprom_getcapabilities                      *\n"); /* 70 */
	printf( "*    02. test_di_eeprom_read (cache)                         *\n"); /* 70 */
	printf( "*    03. test_di_eeprom_write                                *\n"); /* 70 */
	printf( "*    04. test_di_eeprom_read (bulk)                          *\n"); /* 70 */
	printf( "*    05. test_di_eeprom_write (bulk)                         *\n"); /* 70 */
	printf( "*    06. test_di_eeprom_read/write random                    *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    Select Number & Press OK                                *\n"); /* 70 */
	printf( "*============================================================*\n"); /* 70 */
}

static void test_eeprom_getcapabilities(void)
{
	unsigned long	ulEepromSize = 0;

	DRV_EEPROM_GetCapabilities(&ulEepromSize);
	printf("EEPROM size : %d(0x%04X)\n", ulEepromSize, ulEepromSize);
}

static void test_eeprom_read_cache(void)
{
	unsigned long	ulAddress, ulEepromSize = 0;
	unsigned char	*pBuffer;

	DRV_EEPROM_GetCapabilities(&ulEepromSize);
	pBuffer = DD_MEM_Alloc(ulEepromSize);
	if (pBuffer == NULL)
	{
		printf("error : test_eeprom_read_cache fail mem alloc\n");
		return;
	}
	CHECK_BOOT_TIME("start : eeprom read cache\n");
	DI_EEPROM_Read(-0x800, pBuffer, ulEepromSize);
	CHECK_BOOT_TIME("end   : eeprom read cache\n");
	DD_MEM_Free(pBuffer);
}

static void test_eeprom_write(void)
{
	unsigned long	ulAddress, ulEepromSize = 0;
	unsigned char	*pBuffer;
	unsigned char	ucBuffer;

	DRV_EEPROM_GetCapabilities(&ulEepromSize);
	pBuffer = DD_MEM_Alloc(ulEepromSize);
	if (pBuffer == NULL)
	{
		printf("error : test_eeprom_read_cache fail mem alloc\n");
		return;
	}
	DI_EEPROM_Read(-0x800, &ucBuffer, 1);
	if (ucBuffer)
		VK_memset32(pBuffer, 0, ulEepromSize);
	else
		VK_memset32(pBuffer, 0xFF, ulEepromSize);

	CHECK_BOOT_TIME("start : eeprom write cache\n");
	DI_EEPROM_Write(-0x800, pBuffer, ulEepromSize);
	CHECK_BOOT_TIME("end   : eeprom write cache\n");
	DD_MEM_Free(pBuffer);
}

static void test_eeprom_read_bulk(void)
{
	unsigned long	ulAddress, ulEepromSize = 0;
	unsigned char	*pBuffer;

	DRV_EEPROM_GetCapabilities(&ulEepromSize);
	pBuffer = DD_MEM_Alloc(ulEepromSize);
	if (pBuffer == NULL)
	{
		printf("error : test_eeprom_read_cache fail mem alloc\n");
		return;
	}
	CHECK_BOOT_TIME("start : eeprom read bulk\n");
	DRV_EEPROM_Read(0, pBuffer, ulEepromSize);
	CHECK_BOOT_TIME("end   : eeprom read bulk\n");
	DD_MEM_Free(pBuffer);
}

static void test_eeprom_write_bulk(void)
{
	unsigned long	ulAddress, ulEepromSize = 0;
	unsigned char	*pBuffer;
	unsigned char	ucBuffer;

	DRV_EEPROM_GetCapabilities(&ulEepromSize);
	pBuffer = DD_MEM_Alloc(ulEepromSize);
	if (pBuffer == NULL)
	{
		printf("error : test_eeprom_read_cache fail mem alloc\n");
		return;
	}
	DI_EEPROM_Read(-0x800, &ucBuffer, 1);
	if (ucBuffer)
		VK_memset32(pBuffer, 0, ulEepromSize);
	else
		VK_memset32(pBuffer, 0xFF, ulEepromSize);

	CHECK_BOOT_TIME("start : eeprom write bulk\n");
	DRV_EEPROM_Write(0, pBuffer, ulEepromSize);
	CHECK_BOOT_TIME("end   : eeprom write bulk\n");
	DD_MEM_Free(pBuffer);
}

void thread_eeprom_write(void *arg)
{
	int ret;
	int i, j;
	char	WrData[EEPROM_TEST_BUFFER];
	char	RdData[EEPROM_TEST_BUFFER];

	for (i=0; i<EEPROM_TEST_BUFFER; i++)
	{
		for (j=0; j<EEPROM_TEST_BUFFER; j++)
		{
			WrData[i] = (unsigned char)(j+i);
		}
		DI_EEPROM_Write(j+i, WrData, EEPROM_TEST_BUFFER);
		DI_EEPROM_Read(j+i, RdData, EEPROM_TEST_BUFFER);
		if (VK_memcmp(WrData, WrData, EEPROM_TEST_BUFFER))
		{
			printf("error EEPROM Write");
		}
		VK_TASK_Sleep(10);
	}

}

void thread_eeprom_read(void *arg)
{
	int ret;
	int i, j;
	char	WrData[EEPROM_TEST_BUFFER];
	char	RdData[EEPROM_TEST_BUFFER];

	eeprom_test_passed = TRUE;
	for (i=0; i<EEPROM_TEST_BUFFER; i++)
	{
		for (j=0; j<EEPROM_TEST_BUFFER; j++)
		{
			WrData[i] = (unsigned char)(j+i);
		}
		//DI_EEPROM_Write(j+i, WrData, EEPROM_TEST_BUFFER);
		ret = DI_EEPROM_Read(j+i, RdData, EEPROM_TEST_BUFFER);
		if (ret != DI_ERR_OK)
		{
			printf("error EEPROM Write (DI_EEPROM_Read) %08X\n", ret);
			eeprom_test_passed = FALSE;
		}
		VK_TASK_Sleep(10);
	}

}

static void test_eeprom_read_write_random(void)
{
	int ret;
	int i,j;

	eeprom_test_passed = FALSE;

	ret = VK_TASK_Create(
			thread_eeprom_write,
			USER_PRIORITY0,
			TIMER_TASK_STACK_SIZE,
			"msg_q_task_id",
			NULL,
			&taskIdEepWrite, FALSE);
	if(VK_OK != ret)
		goto __error_return_label;

	ret = VK_TASK_Create(
			thread_eeprom_read,
			USER_PRIORITY0,
			TIMER_TASK_STACK_SIZE,
			"msg_q_task_id2",
			NULL,
			&taskIdEepRead, FALSE);
	if(VK_OK != ret)
		goto __error_return_label;

	ret = VK_TASK_Start(taskIdEepWrite);
	if(VK_OK != ret)
		goto __error_return_label;


	ret = VK_TASK_Start(taskIdEepRead);
	if(VK_OK != ret)
		goto __error_return_label;

	VK_TASK_Sleep(5000);

	VK_TASK_Stop(taskIdEepRead);
	VK_TASK_Stop(taskIdEepWrite);
	taskIdEepRead = 0;
	taskIdEepWrite = 0;

	if(TRUE == eeprom_test_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}

__error_return_label:
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}


static void TEST_EEPROM_SelectTestCase(HUINT32 ulTestNumber)
{
    switch( ulTestNumber )
    {
    case 1 :
		test_eeprom_getcapabilities();
		break;
	case 2 :
		test_eeprom_read_cache();
		break;
	case 3 :
		test_eeprom_write();
		break;
	case 4 :
		test_eeprom_read_bulk();
		break;
	case 5 :
		test_eeprom_write_bulk();
		break;
	case 6 :
		test_eeprom_read_write_random();
		break;
	default :
		printf( "Unknown test case(%d)!!\n", ulTestNumber);
		break;
    }
}


