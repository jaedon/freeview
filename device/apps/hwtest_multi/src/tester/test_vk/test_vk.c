/**
 * test_vk.c
*/

/**
 * @defgroup		TEST_VK
 * @author		Tae-Hyuk Kang
 * @note
 * @brief
 * @file 			test_vk.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"

#include "di_err.h"
#include "vkernel.h"
#include "taskdef.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_VK


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

static void TEST_VK_SelectTestSubCase(HUINT32 ulTestNumber);
static void TEST_VK_SelectTestCase(HUINT32 ulTestNumber);
static void TEST_VK_SelectTestSubCase1(HUINT32 ulTestNumber);
static void TEST_VK_SelectTestSubCase2(HUINT32 ulTestNumber);
static void TEST_VK_SelectTestSubCase3(HUINT32 ulTestNumber);
static void TEST_VK_SelectTestSubCase4(HUINT32 ulTestNumber);
static void TEST_VK_SelectTestSubCase5(HUINT32 ulTestNumber);
static int test_vk_sem_timeout(void);
static int test_vk_sem_create_destory(void);
static int test_vk_sem_count(void);
static int test_vk_sem_basic(void);
static int test_vk_task_advance_sleep(void);
static int test_vk_task_priority(void);
static int test_vk_task_create_destory(void);
static int test_vk_task_argument(void);
static int test_vk_task_sleep(void);
static int test_vk_task_basic(void);
static int test_vk_msg_send_receive(void);
static int test_vk_msg_priority(void);
static int test_vk_msg_create_destroy(void);
static int test_vk_msg_priority(void);
static int test_vk_task_join(void);
static int test_vk_timer_event_after(void);
static int test_vk_timer_event_every(void);
static int test_vk_timer_event_cancel(void);
static int test_vk_clock_offset_time(void);
static int test_vk_clock_time(void);
static int test_vk_msg_timeout(void);
static int test_vk_msg_send_timeout(void);
static int test_vk_msg_receive_timeout(void);
static int test_vk_time_diff(void);

// system time disturbance task
void thred_time_disturbance(void *arg)
{
	unsigned long unixTime;

	printf("start system time disturbance task\n");
	unixTime = 0x12345678;
	for (;;)
	{
		VK_TASK_Sleep(10);
		unixTime += 1000;
		VK_CLOCK_SetTime(0, unixTime);
	}
}


static void TEST_VK_Menu(void)
{
    printf( "*============================================================*\n"); /* 70 */
    printf( "*    DD TEST TOOL/VKERNEL                                   *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    00. Quit                                                *\n"); /* 70 */
    printf( "*    01. Semaphore                                          *\n"); /* 70 */
    printf( "*    02. Message Queue                                       *\n"); /* 70 */
    printf( "*    03. Thread                                              *\n"); /* 70 */
    printf( "*    04. timer                                              *\n"); /* 70 */
    printf( "*    05. clock                                              *\n"); /* 70 */
    printf( "*    06. vk capacity                                              *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    Select Number & Press OK                                *\n"); /* 70 */
    printf( "*============================================================*\n"); /* 70 */
}

static void TEST_VK_Sub_Menu1(void)
{
    printf( "*============================================================*\n"); /* 70 */
    printf( "*    DD TEST TOOL/SEMAPHORE                                    *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    00. Quit                                              *\n"); /* 70 */
    printf( "*    01. test_vk_sem_basic                                           *\n"); /* 70 */
    printf( "*    02. test_vk_sem_count                                               *\n"); /* 70 */
    printf( "*    03. test_vk_sem_create_destory                                              *\n"); /* 70 */
    printf( "*    04. test_vk_sem_timeout                                       *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    Select Number & Press OK                                *\n"); /* 70 */
    printf( "*============================================================*\n"); /* 70 */
}

static void TEST_VK_Sub_Menu2(void)
{
	printf( "*============================================================*\n"); /* 70 */
	printf( "*    DD TEST TOOL/MESSAGE QUEUE                              *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    00. Quit                                                *\n"); /* 70 */
	printf( "*    01. test_vk_msg_send_receive                            *\n"); /* 70 */
	printf( "*    02. test_vk_msg_timeout                                 *\n"); /* 70 */
	printf( "*    03. test_vk_msg_send_timeout                            *\n"); /* 70 */
	printf( "*    04. test_vk_msg_receive_timeout                         *\n"); /* 70 */
	printf( "*    05. test_vk_msg_create_destroy                          *\n"); /* 70 */
	printf( "*    06. test_vk_msg_priority                                *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    Select Number & Press OK                                *\n"); /* 70 */
	printf( "*============================================================*\n"); /* 70 */
}

static void TEST_VK_Sub_Menu3(void)
{
    printf( "*============================================================*\n"); /* 70 */
    printf( "*    DD TEST TOOL/SEMAPHORE                                    *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    00. Quit                                                       *\n"); /* 70 */
    printf( "*    01. test_vk_task_basic                                          *\n"); /* 70 */
    printf( "*    02. test_vk_task_sleep                                               *\n"); /* 70 */
    printf( "*    03. test_vk_task_argument                                              *\n"); /* 70 */
    printf( "*    04. test_vk_task_create_destory                                       *\n"); /* 70 */
    printf( "*    05. test_vk_task_priority                                       *\n"); /* 70 */
    printf( "*    06. test_vk_task_advance_sleep                                       *\n"); /* 70 */
    printf( "*    06. test_vk_task_join                                       *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    Select Number & Press OK                                *\n"); /* 70 */
    printf( "*============================================================*\n"); /* 70 */
}

static void TEST_VK_Sub_Menu4(void)
{
    printf( "*============================================================*\n"); /* 70 */
    printf( "*    DD TEST TOOL/TIMER                                    *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    00. Quit                                                       *\n"); /* 70 */
    printf( "*    01. test_vk_time_diff                                          *\n"); /* 70 */
    printf( "*    02. test_vk_timer_event_after                                          *\n"); /* 70 */
    printf( "*    03. test_vk_timer_event_every                                          *\n"); /* 70 */
    printf( "*    04. test_vk_timer_event_cancel                                          *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    Select Number & Press OK                                *\n"); /* 70 */
    printf( "*============================================================*\n"); /* 70 */
}

static void TEST_VK_Sub_Menu5(void)
{
    printf( "*============================================================*\n"); /* 70 */
    printf( "*    DD TEST TOOL/CLOCK                                    *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    00. Quit                                                       *\n"); /* 70 */
    printf( "*    01. test_vk_clock_time                                          *\n"); /* 70 */
    printf( "*    02. test_vk_clock_offset_time                                          *\n"); /* 70 */
    printf( "*------------------------------------------------------------*\n"); /* 70 */
    printf( "*    Select Number & Press OK                                *\n"); /* 70 */
    printf( "*============================================================*\n"); /* 70 */
}


void TEST_VKERNEL(void)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;

    do {
        TEST_VK_Menu(); /* print menu */

        ulRet = TEST_GetNumber(&ulTestNumber);
        if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
        {
        	/* */
            break;
        }
		else if (ulRet == DI_ERR_OK)
		{
            TEST_VK_SelectTestCase(ulTestNumber);
        }
        else {
            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
        }
    } while (1);

}

static void TEST_VK_SelectTestCase(HUINT32 ulTestNumber)
{
	DI_ERR_CODE ulRet = 0;

	switch( ulTestNumber )
	{
		case 1 : //semaphore
		{
		    do {
		        TEST_VK_Sub_Menu1(); /* print menu */

		        ulRet = TEST_GetNumber(&ulTestNumber);
				if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
				{
					/* */
					break;
				}
				else if (ulRet == DI_ERR_OK)
				{
		            TEST_VK_SelectTestSubCase1(ulTestNumber);
		        }
		        else
		        {
		            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
		        }
		    } while (1);
		    break;
		}
		case 2 : //message queue
		{
		    do {
		        TEST_VK_Sub_Menu2(); /* print menu */

		        ulRet = TEST_GetNumber(&ulTestNumber);
				if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
				{
					/* */
					break;
				}
				else if (ulRet == DI_ERR_OK)
				{
		            TEST_VK_SelectTestSubCase2(ulTestNumber);
		        }
		        else
		        {
		            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
		        }
		    } while (1);
		    break;
		}

		case 3 : //thread
		{
		    do {
		        TEST_VK_Sub_Menu3(); /* print menu */

		        ulRet = TEST_GetNumber(&ulTestNumber);
				if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
				{
					/* */
					break;
				}
		        else if (ulRet == DI_ERR_OK)
		        {
		            TEST_VK_SelectTestSubCase3(ulTestNumber);
		        }
		        else
		        {
		            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
		        }
		    } while (1);
		    break;
		}
		case 4 : //time
		{
		    do {
		        TEST_VK_Sub_Menu4(); /* print menu */

		        ulRet = TEST_GetNumber(&ulTestNumber);
				if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
				{
					/* */
					break;
				}
				else if (ulRet == DI_ERR_OK)
				{
		            TEST_VK_SelectTestSubCase4(ulTestNumber);
		        }
		        else
		        {
		            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
		        }
		    } while (1);
		    break;
		}
		case 5: //clock
		{
		    do {
		        TEST_VK_Sub_Menu5(); /* print menu */

		        ulRet = TEST_GetNumber(&ulTestNumber);
				if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
				{
					/* */
					break;
				}
		        else if (ulRet == DI_ERR_OK)
		        {
		            TEST_VK_SelectTestSubCase5(ulTestNumber);
		        }
		        else
		        {
		            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
		        }
		    } while (1);
		    break;
		}
		case 6: //vk capacity
		{
			test_vk_capacity();
			break;
		}
		default :
		{
		    printf( "Unknown key Code(%d)!!\n", ulTestNumber);
		    break;
		}
	}
}

static void TEST_VK_SelectTestSubCase1(HUINT32 ulTestNumber)
{
    switch( ulTestNumber )
    {
        case 0: TEST_VKERNEL(); break;
        case 1: test_vk_sem_basic(); break;
        case 2: test_vk_sem_count(); break;
        case 3: test_vk_sem_create_destory(); break;
        case 4: test_vk_sem_timeout(); break;
        default: printf( "Unknown key Code(%d)!!\n", ulTestNumber); break;
    }
}

static void TEST_VK_SelectTestSubCase2(HUINT32 ulTestNumber)
{
	switch( ulTestNumber )
	{
		case 0: TEST_VKERNEL(); break;
		case 1: test_vk_msg_send_receive(); break;
		case 2: test_vk_msg_timeout(); break;
		case 3: test_vk_msg_send_timeout(); break;
		case 4: test_vk_msg_receive_timeout(); break;
		case 5: test_vk_msg_create_destroy(); break;
		case 6: test_vk_msg_priority(); break;
		default: printf( "Unknown key Code(%d)!!\n", ulTestNumber); break;
	}
}


static void TEST_VK_SelectTestSubCase3(HUINT32 ulTestNumber)
{
	switch( ulTestNumber )
	{
		case 0: TEST_VKERNEL(); break;
		case 1: test_vk_task_basic(); break;
		case 2: test_vk_task_sleep(); break;
		case 3: test_vk_task_argument(); break;
		case 4: test_vk_task_create_destory(); break;
		case 5: test_vk_task_priority(); break;
		case 6: test_vk_task_advance_sleep(); break;
		case 7: test_vk_task_join(); break;
		default: printf( "Unknown key Code(%d)!!\n", ulTestNumber); break;
	}
}

static void TEST_VK_SelectTestSubCase4(HUINT32 ulTestNumber)
{
	switch( ulTestNumber )
	{
		case 0: TEST_VKERNEL(); break;
		case 1 : test_vk_time_diff(); break;
		case 2 : test_vk_timer_event_after(); break;
		case 3 : test_vk_timer_event_every(); break;
		case 4 : test_vk_timer_event_cancel(); break;
		default: printf( "Unknown key Code(%d)!!\n", ulTestNumber); break;
	}
}

static void TEST_VK_SelectTestSubCase5(HUINT32 ulTestNumber)
{
	switch( ulTestNumber )
	{
		case 0: TEST_VKERNEL(); break;
		case 1: test_vk_clock_time(); break;
		case 2: test_vk_clock_offset_time(); break;
		default: printf( "Unknown key Code(%d)!!\n", ulTestNumber); break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////VK_CAPACITY TEST START////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
int test_vk_capacity(void)
{
	printf("available creation task  = %d\n", 80);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////VK_CLOCK TEST START////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// unix time을 set/get을 테스트 한다.
//
static int test_vk_clock_time(void)
{
	int errCode;
	HUINT32 unix_time;

	unix_time = 0;
	PrintDebug("unix_time = 0x%llX\n", unix_time);
	errCode = VK_CLOCK_SetTime(VK_CLOCK_SETBY_SI, unix_time);
	if(VK_OK != errCode) {
		PrintError("%s(%d) : VK_CLOCK_SetTime Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	unix_time = 0xFFFFFFFF;
	PrintDebug("unix_time = 0x%llX\n", unix_time);
	errCode = VK_CLOCK_GetTime(&unix_time);
	if(VK_OK != errCode) {
		PrintError("%s(%d) : VK_CLOCK_GetTime Error\n", __func__, __LINE__);
		goto __error_return_label;
	}
	PrintDebug("unix_time = 0x%llX\n", unix_time);
	if (unix_time == (HUINT64)0)
	{
		printf("Success - exact time was returned\n");
	}
	else
	{
		printf("Fail - error is more than 1s\n");
		printf("unix_time = 0x%llX\n", unix_time);
		goto __error_return_label;
	}

	printf("%s [RESULT] Pass\n", __func__);
	return VK_OK;

__error_return_label:
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}

static int test_vk_clock_offset_time(void)
{
	int errCode;
	HUINT64 unix_time;
	int i, j;
	int hour, min;
	HUINT32 set_off_time;
	HUINT32 get_off_time;

	for(i=-12; i<12+1; i++)
	{
		for(j=0; j<59+1; j++)
		{
			set_off_time = i*60 + j;
			errCode = VK_CLOCK_SetOffsetTime(i, j);
			if(VK_OK != errCode) {
				PrintError("%s(%d) : VK_CLOCK_SetOffsetTime Error\n", __func__, __LINE__);
				goto __error_return_label;
			}
			hour = min = 0;
			errCode = VK_CLOCK_GetOffsetTime(&hour, &min);
			if(VK_OK != errCode) {
				PrintError("%s(%d) : VK_CLOCK_GetOffsetTime Error\n", __func__, __LINE__);
				goto __error_return_label;
			}
			get_off_time = hour*60 + min;

			if(j%15 == 0)
			{
				PrintDebug("set hour = %d, min = %d\n", i, j);
				PrintDebug("get hour = %d, min = %d\n", hour, min);
				PrintDebug("set_off_time= %d\n", set_off_time);
				PrintDebug("get_off_time = %d\n", get_off_time);
				if(set_off_time != get_off_time)
				{
					goto __error_return_label;
				}
			}
		}
	}

	printf("%s [RESULT] Pass\n", __func__);
	return VK_OK;

__error_return_label:
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////VK_TIMER TEST START////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TIME_OUT_VAL  1000
#define TIME_OUT_ERR_LIMIT  10
static HUINT32 test_timer_id;
static HBOOL test_timer_passed;
static HUINT32 test_timer_count;
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 등록한 Timer가 제대로 Cancel되는지 테스트 한다.
// TIME_OUT_ERR_LIMIT mili정도를 오차 범위로 하였음.
//
void timer_event_after_cancel(unsigned long timer_id, void *arg)
{
	HUINT64 milisecond;

	if(test_timer_id == timer_id)
	{
		test_timer_passed = FALSE;
		milisecond = VK_TIMER_GetSystemTick();
		PrintDebug("milisecond4= %lld, count = %ld\n", milisecond, test_timer_count);
	}
}

void timer_event_every_cancel(unsigned long timer_id, void *arg)
{
	HUINT64 milisecond;

	if(test_timer_id == timer_id)
	{
		test_timer_count++;
		milisecond = VK_TIMER_GetSystemTick();
		PrintDebug("milisecond3= %lld, count = %ld\n", milisecond, test_timer_count);
		if(test_timer_count == 9)
		{
			VK_TIMER_Cancel(test_timer_id);
		}
		else if(test_timer_count == 10)
		{
			test_timer_passed = FALSE;
		}
	}
}

static int test_vk_timer_event_cancel(void)
{
	int errCode;
	HUINT64 milisecond;

	test_timer_id = 0;
	test_timer_passed = FALSE;
	test_timer_count = 0;

	errCode = VK_TIMER_EventAfter(TIME_OUT_VAL, timer_event_after_cancel, NULL, 0, &test_timer_id);
	if(VK_OK != errCode)
	{
		PrintError("%s(%d) : VK_TIMER_EventAfter Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond1= %lld\n", milisecond);

	test_timer_passed = TRUE;
	VK_TASK_Sleep(TIME_OUT_VAL - TIME_OUT_ERR_LIMIT);
	VK_TIMER_Cancel(test_timer_id);

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond2= %lld\n", milisecond);

	if(test_timer_passed == FALSE)
	{
		goto __error_return_label;
	}

	errCode = VK_TIMER_EventEvery(TIME_OUT_VAL, timer_event_every_cancel, NULL, 0, &test_timer_id);
	if(VK_OK != errCode)
	{
		PrintError("%s(%d) : VK_TIMER_EventAfter Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond1= %lld\n", milisecond);

	test_timer_passed = TRUE;
	VK_TASK_Sleep( (TIME_OUT_VAL + TIME_OUT_ERR_LIMIT) *10 );

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond2= %lld\n", milisecond);

	if(TRUE == test_timer_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}

__error_return_label:
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 지정된 Time후에 Cancel하기 전까지 등록한 func을 지정된 시간 마다 호출하는지를 테스트 한다.
// TIME_OUT_ERR_LIMIT mili정도를 오차 범위로 하였음.
//
void timer_event_every(unsigned long timer_id, void *arg)
{
	HUINT64 milisecond;

	if(test_timer_id == timer_id)
	{
		test_timer_count++;
		milisecond = VK_TIMER_GetSystemTick();
		PrintDebug("milisecond3= %lld, count = %ld\n", milisecond, test_timer_count);
		if(test_timer_count == 10)
		{
			test_timer_passed = TRUE;
			VK_TIMER_Cancel(test_timer_id);
		}
	}
}

static int test_vk_timer_event_every(void)
{
	int errCode;
	HUINT32 timeout;
	HUINT64 milisecond;

	test_timer_id = 0;
	test_timer_passed = FALSE;
	test_timer_count = 0;
	timeout = TIME_OUT_VAL;
	errCode = VK_TIMER_EventEvery(timeout, timer_event_every, NULL, 0, &test_timer_id);
	if(VK_OK != errCode)
	{
		PrintError("%s(%d) : VK_TIMER_EventAfter Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond1= %lld\n", milisecond);

	VK_TASK_Sleep( (TIME_OUT_VAL+TIME_OUT_ERR_LIMIT) *10 );

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond2= %lld\n", milisecond);

	if(TRUE == test_timer_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}

__error_return_label:
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 지정된 Time후에 등록한 func을 호출하는지를 테스트 한다.
// TIME_OUT_ERR_LIMIT mili정도를 오차 범위로 하였음.
//
void timer_event_after(unsigned long timer_id, void *arg)
{
	HUINT64 milisecond;

	if(test_timer_id == timer_id)
	{
		test_timer_passed = TRUE;
		milisecond = VK_TIMER_GetSystemTick();
		PrintDebug("milisecond3= %lld\n", milisecond);
	}
}

static int test_vk_timer_event_after(void)
{
	int errCode;
	HUINT32 timeout;
	HUINT64 milisecond;

	test_timer_id = 0;
	test_timer_passed = FALSE;
	timeout = TIME_OUT_VAL;
	errCode = VK_TIMER_EventAfter(timeout, timer_event_after, NULL, 0, &test_timer_id);
	if(VK_OK != errCode)
	{
		PrintError("%s(%d) : VK_TIMER_EventAfter Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond1= %lld\n", milisecond);

	VK_TASK_Sleep(TIME_OUT_VAL+TIME_OUT_ERR_LIMIT);

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond2= %lld\n", milisecond);

	if(TRUE == test_timer_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}

__error_return_label:
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}

#define TIMEOUT_DIFF_TEST 5000
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// milisecond단위로 sleep후에 get시에 정확한 sleep시간 차이가 나는지 테스트 한다.
// 오차 범위내에 Pass되면 성공
//
static int test_vk_time_diff(void)
{
	int errCode;
	HUINT64 milisecond;
	HUINT64 milisecond2;
	HUINT64 elapse;

	milisecond = VK_TIMER_GetSystemTick();

	VK_TASK_Sleep(TIMEOUT_DIFF_TEST);

	milisecond2 = VK_TIMER_GetSystemTick();

	elapse = (milisecond2 - milisecond);

	if (TIMEOUT_DIFF_TEST ==  elapse)
	{
	    printf("Success - exact time was returned\n");
	}
	else if ( (elapse > (TIMEOUT_DIFF_TEST*0.9)) && (elapse < (TIMEOUT_DIFF_TEST*1.1)) )
	{
	    printf("Success - error is less than 10%\n");
	}
	else
	{
		printf("Fail - error is more than 10%\n");
		printf("elapse = 0x%llX\n", elapse);
		goto __error_return_label;
	}

    printf("%s [RESULT] Pass\n", __func__);
    return VK_OK;

__error_return_label:
    PrintError("%s [RESULT] Fail\n", __func__);
    return VK_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////VK_SEM TEST START//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
HBOOL test_sem_passed;
HBOOL test_sem_ready;
HUINT32 sem_g;
HUINT32 task_g;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//VK_SEM_GetTimeout()을 테스트 한다.
//
static int test_vk_sem_timeout(void)
{
	HUINT32 testTaskId = 0;
	int ret;
	int i,j;
	HUINT32	tick1, tick2;
	HUINT32	timeouttest;

	test_sem_passed = TRUE;

	ret = VK_TASK_Create(
	        thred_time_disturbance,
	        USER_PRIORITY0,
	        TIMER_TASK_STACK_SIZE,
	        "TestTask1",
	        NULL,
	        &testTaskId, FALSE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Start(testTaskId);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Start Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

    ret = VK_SEM_Create(&sem_g, "sem", VK_SUSPENDTYPE_PRIORITY);
    if(VK_OK != ret)
    {
        PrintError("VK_SEM_Create Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_SEM_Get(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Get Error!![%s(%d)]\n", __func__, __LINE__);
        return;
    }

	for (i=0; i<100; i++)
	{
		timeouttest = i*10;
		tick1 = VK_TIMER_GetSystemTick();
		ret = VK_SEM_GetTimeout(sem_g, timeouttest);
		if(VK_TIMEOUT != ret)
		{
			PrintError("%s(%d) : VK_SEM_GetTimeout Error\n", __func__, __LINE__);
			goto __error_return_label;
		}
		tick2 = VK_TIMER_GetSystemTick();
		if (tick2-tick1 == timeouttest)
		{
			printf("%s(%d) : VK_SEM_GetTimeout pass %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
		else if ((tick2-tick1 > timeouttest*1.1) || (tick2-tick1 < timeouttest*0.9))
		{
			printf("%s(%d) : VK_SEM_GetTimeout Error wanted timeout %d, real timeout %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
			test_sem_passed = FALSE;
		}
		else
		{
			printf("%s(%d) : VK_SEM_GetTimeout OK wanted timeout %d, real timeout %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
	}

	VK_TASK_Destroy(testTaskId);
	
	ret = VK_SEM_Destroy(sem_g);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
		goto __error_return_label;
	}
	sem_g = 0;
	
	if(TRUE == test_sem_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}
	
__error_return_label:
	ret = VK_SEM_Destroy(sem_g);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
		goto __error_return_label;
	}
	sem_g = 0;
	
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//VK_SEM_Create/VK_SEM_Destroy을 반복하므로 인해 메모리 부족이 일어 나는지 테스트 한다.
//
static int test_vk_sem_create_destory(void)
{
    DI_ERR_CODE ret;
    HUINT32 trial_num = 10000;
    int i;

    printf("%s start\n", __func__);

    sem_g = NULL;

    for(i=0; i<trial_num; i++)
    {
        ret = VK_SEM_Create(&sem_g, "sem", VK_SUSPENDTYPE_PRIORITY);
        if(VK_OK != ret)
        {
            PrintError("VK_SEM_Create Error!![%s(%d)]\n", __func__, __LINE__);
            goto __error_return_label;
        }
        printf("VK_SEM_Create %d\n", i);

        ret = VK_SEM_Destroy(sem_g);
        if(ret != VK_OK)
        {
            PrintError("VK_SEM_Destroy Error!![%s(%d)]\n", __func__, __LINE__);
            goto __error_return_label;
        }
        printf("VK_SEM_Destroy %d\n", i);
    }

    printf("%s [RESULT] Pass\n", __func__);
    return VK_OK;

__error_return_label:
    PrintError("%s [RESULT] Fail\n", __func__);
    return VK_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//VK_SEM_CreateWithCount을 테스트 한다.
//
void thread_task_sem_count(void *arg)
{
    DI_ERR_CODE ret;

    ret = VK_SEM_Get(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Get Error!![%s(%d)]\n", __func__, __LINE__);
        return;
    }

    test_sem_passed = TRUE;
}

static int test_vk_sem_count(void)
{
    DI_ERR_CODE ret;
    unsigned long tid;

    printf("%s start\n", __func__);

    test_sem_passed  = FALSE;
    sem_g = NULL;
    task_g = NULL;

    ret = VK_SEM_CreateWithCount(&sem_g, 2, "sem_count", VK_SUSPENDTYPE_PRIORITY);
    if(VK_OK != ret)
    {
        PrintError("VK_SEM_Create Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_SEM_Get(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Get Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_TASK_Create(
        thread_task_sem_count,
        USER_PRIORITY0,
        NORMAL_TASK_STACK_SIZE,
        "thread_task_sem_count",
        (void *)0,
        &task_g,
        FALSE);

    if(VK_OK != ret)
    {
        PrintError("VK_TASK_Create Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_TASK_Start(task_g);
    if(VK_OK != ret)
    {
        PrintError("VK_TASK_Start Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    VK_TASK_Sleep(2000);

    if(test_sem_passed == FALSE)
    {
        PrintError("test_sem_passed FALSE!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }
    ret = VK_SEM_Release(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Release Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_SEM_GetTimeout(sem_g, 0);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_GetTimeout Error!! ->%s(%d)\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_SEM_Release(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Release Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }
    
    ret = VK_SEM_GetTimeout(sem_g, 100);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM___error_return_laeblGetTimeout Error!! -> %s(%d)\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_SEM_Destroy(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Destroy Error!! -> %s(%d)\n", __func__, __LINE__);
        goto __error_return_label;
    }

    printf("%s [RESULT] Pass\n", __func__);
    return VK_OK;

__error_return_label:
    printf("%s [RESULT] Fail\n", __func__);
    return VK_ERROR;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//VK_SEM_Create을 테스트 한다.
//
void thread_task_sem_basic(void *arg)
{
    DI_ERR_CODE ret;

    VK_TASK_Sleep(2000);

    test_sem_ready = TRUE;

    ret = VK_SEM_Release(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Release Error!![%s(%d)]\n", __func__, __LINE__);
        return;
    }

    test_sem_passed = TRUE;
}


static int test_vk_sem_basic(void)
{
    DI_ERR_CODE ret;
    unsigned long tid;

    printf("%s start\n", __func__);

    test_sem_passed  = FALSE;
    test_sem_ready   = FALSE;
    sem_g = NULL;
    task_g = NULL;

    ret = VK_SEM_Create(&sem_g, "sem", VK_SUSPENDTYPE_PRIORITY);
    if(VK_OK != ret)
    {
        PrintError("VK_SEM_Create Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_SEM_Get(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Get Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_TASK_Create(
        thread_task_sem_basic,
        USER_PRIORITY0,
        NORMAL_TASK_STACK_SIZE,
        "thread_task_sem_basic",
        (void *)0,
        &task_g,
        FALSE);

    if(VK_OK != ret)
    {
        PrintError("VK_TASK_Create Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    VK_TASK_Start(task_g);

    ret = VK_SEM_Get(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Get Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    if (test_sem_ready == FALSE) {
        PrintError("The ac_sm_wait didn't wait until sm is available\n");
        goto __error_return_label;
    }

    ret = VK_SEM_GetTimeout(sem_g, 0);
    if(ret != VK_TIMEOUT)
    {
        PrintError("VK_SEM_GetTimeout Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_SEM_GetTimeout(sem_g, 100);
    if(ret != VK_TIMEOUT)
    {
        PrintError("VK_SEM_GetTimeout Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    ret = VK_SEM_Destroy(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Destroy Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }

    if (test_sem_passed == FALSE)
    {
        goto __error_return_label;
    }

    printf("%s [RESULT] Pass\n", __func__);
    return VK_OK;

__error_return_label:
    PrintError("%s [RESULT] Fail\n", __func__);
    return VK_ERROR;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////VK_TASK TEST START//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NUM_OF_TIME_DURATION 8
#define SLEEP_TIME 2000
HBOOL task_test_ready;
HUINT8 task_test_passed;
static HUINT32 test_vk_task_id;
static HUINT32 threadArg = (HUINT32) 0xFEDCBA98;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// thread의 join option을 테스트 한다.
//
void thread_vk_task_join(void *arg)
{
	int i;
	for(i=0; i<10; i++)
	{
		VK_TASK_Sleep(1000);
		printf("%d second\n", i);
	}
	task_test_passed = TRUE;
}

static int test_vk_task_join(void)
{
	int errCode;

	test_vk_task_id = 0;
	task_test_passed = FALSE;

	errCode = VK_TASK_Create(thread_vk_task_join, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_vk_task_join", (void *)0, &test_vk_task_id, FALSE);
	if(errCode != VK_OK)
	{
	    PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
	    goto __error_return_label;
	}
	errCode = VK_TASK_Start(test_vk_task_id);
	if(errCode != VK_OK)
	{
	    PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
	    goto __error_return_label;
	}

	errCode = VK_TASK_Join(test_vk_task_id);
	if(errCode != VK_OK)
	{
	    PrintError("%s(%d) : VK_TASK_Join Error!!\n", __func__, __LINE__);
	    goto __error_return_label;
	}

	if(TRUE == task_test_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}
	__error_return_label:
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// thread의 sleep의 정확도를 테스트 한다.
//
static int test_vk_task_advance_sleep(void)
{
    HUINT32 i;
    HUINT32 time_duration[] =
        { 500, 1000, 2500, 10000, 15000, 20000, 50000, 100000 };

    HUINT64 before, after;
    HUINT32 elapse;

    for( i = 0; i < NUM_OF_TIME_DURATION; i++) {
        before = VK_TIMER_GetSystemTick();

        printf("Sleep %d mill seconds.\n", time_duration[i]);

        VK_TASK_Sleep(time_duration[i]);

        after = VK_TIMER_GetSystemTick();

        elapse = (HUINT32)(after - before);

        if (elapse == time_duration[i]) {
            printf("Exact time elapsed. Success.\n");
        } else if (elapse > (time_duration[i] * 0.9)
                && (elapse < (time_duration[i] * 1.1))) {
            printf("%d elapsed.\n", elapse);
            printf("[MSG] Success: The difference between elapsed");
            printf(
                    " time and expected duration is within 10%% error rate.\n");
        } else {
            printf("%d elapsed.\n", elapse);
            printf("Fail: The diifernce is out of range.\n");
            printf("[RESULT] Fail\n");
            return FALSE;
        }
    }

    printf("[RESULT] Pass\n");
    return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// thread의 priority changing을 테스트 한다.
//
void thread_vk_task_priority1(void  *arg)
{
    HUINT32 i;
    for(i=0; i<0xFFF; i++)
    {
        i++;
        printf("%s(%d) : i = %d\n", __func__, __LINE__, i);
        VK_TASK_Sleep(10);
    }
    task_test_ready = TRUE;
    if(task_test_passed == TRUE)
    {
        task_test_passed = TRUE;
    }
    else
    {
        task_test_passed = FALSE;
    }
}
void thread_vk_task_priority2(void  *arg)
{
    HUINT32 i;
    for(i=0; i<0xFFF; i++)
    {
        i++;
        printf("%s(%d) : i = %d\n", __func__, __LINE__, i);
        VK_TASK_Sleep(10);
    }
    task_test_ready = TRUE;
    if(task_test_passed == FALSE)
    {
        task_test_passed = TRUE;
    }
    else
    {
        task_test_passed = TRUE;
    }
}

static int test_vk_task_priority(void)
{
    HUINT32 task_id = 0;
    HUINT32 task_id2 = 0;
    DI_ERR_CODE errCode = DI_ERR_OK;

    task_test_passed = FALSE;
    task_test_ready = FALSE;

    errCode = VK_TASK_Create(thread_vk_task_priority1, USER_PRIORITY15, NORMAL_TASK_STACK_SIZE, "thread_vk_task_priority1", (void *)0, &task_id, FALSE);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }
    errCode = VK_TASK_Start(task_id);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }

    errCode = VK_TASK_Create(thread_vk_task_priority2, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_vk_task_priority2", (void *)0, &task_id2, FALSE);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }
    errCode = VK_TASK_Start(task_id2);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }

    VK_TASK_Sleep(10000);
    VK_TASK_SetCurrentPriority(task_id, USER_PRIORITY5);
    VK_TASK_SetCurrentPriority(task_id2, USER_PRIORITY7);

    do
    {
        VK_TASK_Sleep(10);
    }while(task_test_ready == FALSE);


    if(task_test_passed != TRUE)
    {
        printf("%s [RESULT] Fail\n", __func__);
        goto __error_return_label;
    }

    printf("%s [RESULT] Pass\n", __func__);
    return VK_OK;

__error_return_label:
    return VK_ERROR;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// thread의 create/destory를 테스트 한다.
//
void thread_vk_task_create_destory(void  *arg)
{
	while (1)
	{
		CHECK;
	    VK_TASK_Sleep(20);
		CHECK;
	    printf("%s create %d\n", __func__, *(int *)arg);
	}
}

void thread_task_create_destory(void  *arg)
{
    HUINT32 task_id = 0;
    DI_ERR_CODE errCode = DI_ERR_OK;
    int i;

    for(i=0; i<100; i++)
    {
        errCode = VK_TASK_Create(thread_vk_task_create_destory, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_vk_task_create_destory", (void *)&i, &task_id, FALSE);
        if(errCode != VK_OK)
        {
            PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
            goto __error_return_label;
        }
        errCode = VK_TASK_Start(task_id);
        if(errCode != VK_OK)
        {
            PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
            goto __error_return_label;
        }

        VK_TASK_Sleep(30);

        errCode = VK_TASK_Destroy(task_id);
        if(errCode != VK_OK)
        {
            PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
            goto __error_return_label;
        }
    }

    printf("%s [RESULT] Pass\n", __func__);
    return VK_OK;

__error_return_label:
    return VK_ERROR;
}


static int test_vk_task_create_destory(void)
{
    HUINT32 task_id = 0;
    DI_ERR_CODE errCode = DI_ERR_OK;
    int i;

	errCode = VK_TASK_Create(thread_task_create_destory, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_task_create_destory", (void *)&i, &task_id, FALSE);
	if(errCode != VK_OK)
	{
		PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
		return 2;
	}
	CHECK;
	errCode = VK_TASK_Start(task_id);
	if(errCode != VK_OK)
	{
		PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
		return 1;
	}

    return VK_ERROR;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Thread에게 argument가 제대로 전달 되는지를 테스트 한다.
//
void thread_vk_task_argument(void *arg)
{
    HUINT32 val;
    val = *(HUINT32 *)arg;
    printf("val = 0x%x\n", val);
    printf("threadArg = 0x%x\n", threadArg);
    if(val == threadArg)
    {
        task_test_passed = TRUE;
    }
}

static int test_vk_task_argument(void)
{
    HUINT32 task_id = 0;
    DI_ERR_CODE errCode = DI_ERR_OK;

    task_test_passed = FALSE;
    errCode = VK_TASK_Create(thread_vk_task_argument, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_vk_task_argument", (void *)&threadArg, &task_id, FALSE);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }
    errCode = VK_TASK_Start(task_id);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }

    VK_TASK_Sleep(SLEEP_TIME);

    if(task_test_passed != TRUE)
    {
        printf("%s [RESULT] Fail\n", __func__);
        goto __error_return_label;
    }

    printf("%s [RESULT] Pass\n", __func__);
    return VK_OK;

__error_return_label:
    return VK_ERROR;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// thread에서 Sleep이 제대로 수행 되는지를 검사 한다.
//
void thread_vk_task_sleep(void *arg)
{
    HUINT32 i;
    PrintDebug("%s(%d) : start\n", __func__, __LINE__);


    for (i = 0 ; i < 20 ; i++) {
        printf("Running in loop No %d\n", i);

        if (i % 5 == 0) {
            printf("Sleeping for %d seconds\n",
                    (SLEEP_TIME / 1000));

            VK_TASK_Sleep(SLEEP_TIME);
        }
    }
    task_test_passed = TRUE;
}

static int test_vk_task_sleep(void)
{
    HUINT32 task_id = 0;
    DI_ERR_CODE errCode = DI_ERR_OK;

    task_test_passed = FALSE;
    errCode = VK_TASK_Create(thread_vk_task_sleep, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "test_vk_task_sleep", 0, &task_id, FALSE);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }
    errCode = VK_TASK_Start(task_id);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }

    VK_TASK_Sleep(SLEEP_TIME * 5);

    if(TRUE == task_test_passed)
    {
        printf("%s [RESULT] Pass\n", __func__);
    }
    else
    {
        printf("%s [RESULT] Fail\n", __func__);
        goto __error_return_label;
    }

    return VK_OK;

__error_return_label:
    return VK_ERROR;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 기본적으로 thread가 제대로 수행 되는지를 테스트 한다.
//
void thread_vk_task_basic(void *arg)
{
    PrintDebug("%s(%d) : start\n", __func__, __LINE__);
    task_test_passed = TRUE;
}

static int test_vk_task_basic(void)
{
    HUINT32 task_id = 0;
    DI_ERR_CODE errCode = DI_ERR_OK;

    task_test_passed = FALSE;
    errCode = VK_TASK_Create(thread_vk_task_basic, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_vk_task_basic", 0, &task_id, FALSE);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }
    errCode = VK_TASK_Start(task_id);
    if(errCode != VK_OK)
    {
        PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
        goto __error_return_label;
    }

    VK_TASK_Sleep(1000);
    if(TRUE == task_test_passed)
    {
        printf("%s [RESULT] Pass\n", __func__);
    }
    else
    {
        printf("%s [RESULT] Fail\n", __func__);
        goto __error_return_label;
    }

    return VK_OK;

__error_return_label:
    return VK_ERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////VK_MSG////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define Q_SIZE 5
#define Q_COUNT 4
HUINT32 msgQid = 0;
HBOOL msg_test_passed = FALSE;
HUINT8 msg_data_send[Q_COUNT][Q_SIZE] = {{0x11,0x12,0x13,0x14,0x14},
									 {0x21,0x22,0x23,0x24,0x25},
 									 {0x31,0x32,0x33,0x34,0x35},
									 {0x41,0x42,0x43,0x44,0x45} };
HUINT32 msg_q_task_id;
HUINT32 msg_q_task_id2;
HUINT32 msg_q_task_id3;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// task의 priority에 따라 receive가 되는지 검사 한다. (message queue의 resource option은 priority로 설정)
//
void thread_vk_msg_priority(void *arg)
{
	int ret;

	ret = VK_MSG_Send(msgQid, msg_data_send[0], Q_SIZE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
	}

	VK_TASK_Sleep(10);

	ret = VK_MSG_Send(msgQid, msg_data_send[1], Q_SIZE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
	}

}

void thread_vk_msg_priority2(void *arg)
{
	int ret;

	ret = VK_MSG_Send(msgQid, msg_data_send[2], Q_SIZE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
	}

	VK_TASK_Sleep(10);

	ret = VK_MSG_Send(msgQid, msg_data_send[3], Q_SIZE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
	}
}

void thread_vk_msg_priority3(void *arg)
{
	HUINT8 msg_data_receive[Q_COUNT][Q_SIZE] = {0, };
	int ret, i, j;

	for(i=0; i<Q_COUNT; i++)
	{
		VK_TASK_Sleep(10);
		ret = VK_MSG_Receive(msgQid, msg_data_receive[i], Q_SIZE);
		if(VK_OK != ret)
		{
			PrintError("%s(%d) : VK_MSG_Receive Error\n", __func__, __LINE__);
			goto __error_return_label;
		}
	}

	for(i=0; i<Q_COUNT; i++)
	{
		PrintDebug("msg_data_receive[%d] = { ", i);
		for(j=0; j<Q_SIZE; j++)
		{
			PrintDebug("0x%X, ", msg_data_receive[i][j]);
		}
		PrintDebug("}\n");

	}

	for(i=0; i<2; i++)
	{
		for(j=0; j<Q_SIZE; j++)
		{
			if(msg_data_send[i][j] !=	msg_data_receive[i+2][j])
			{
				goto __error_return_label;
			}
		}
	}

	for(i=2; i<4; i++)
	{
		for(j=0; j<Q_SIZE; j++)
		{
			if(msg_data_send[i][j] !=	msg_data_receive[i-2][j])
			{
				goto __error_return_label;
			}
		}
	}

	msg_test_passed = TRUE;
	return;

__error_return_label:
	msg_test_passed = FALSE;
}

static int test_vk_msg_priority(void)
{
	int ret;

	msgQid = 0;
	msg_q_task_id = 0;
	msg_q_task_id2 = 0;
	msg_q_task_id3 = 0;
	msg_test_passed = FALSE;

	ret = VK_MSG_Create(Q_COUNT, Q_SIZE, "vk_msg_priority", &msgQid, VK_SUSPENDTYPE_PRIORITY);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Create(thread_vk_msg_priority,
		USER_PRIORITY0,
		NORMAL_TASK_STACK_SIZE,
		"thread_vk_msg_priority",
		(void *)0, &msg_q_task_id, FALSE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Create(thread_vk_msg_priority2,
		USER_PRIORITY2,
		NORMAL_TASK_STACK_SIZE,
		"thread_vk_msg_priority2",
		(void *)0, &msg_q_task_id2, FALSE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}
	ret = VK_TASK_Create(thread_vk_msg_priority3,
		USER_PRIORITY3,
		NORMAL_TASK_STACK_SIZE,
		"thread_vk_msg_priority3",
		(void *)0, &msg_q_task_id3, FALSE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Start(msg_q_task_id);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Start Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Start(msg_q_task_id2);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Start Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Start(msg_q_task_id3);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Start Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	VK_TASK_Sleep(1000);

	if(TRUE == msg_test_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}

__error_return_label:
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// message queue의 create/destory를 테스트 한다.
//
static int test_vk_msg_create_destroy(void)
{
	int ret;
	int i;

	for(i=0; i<10000; i++)
	{
		ret = VK_MSG_Create(Q_COUNT, Q_SIZE, "test_vk_msg_error_return", &msgQid, VK_SUSPENDTYPE_FIFO);
		if(VK_OK != ret)
		{
			PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
			goto __error_return_label;
		}

		ret = VK_MSG_Send(msgQid, &msg_data_send[0], Q_SIZE);
		if(VK_OK != ret)
		{
			PrintError("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
			goto __error_return_label;
		}

		ret = VK_MSG_Destroy(msgQid);
		if(VK_OK != ret)
		{
			PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
			goto __error_return_label;
		}
	}

	printf("%s [RESULT] Pass\n", __func__);
	return VK_OK;

__error_return_label:
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 {0x41,0x42,0x43,0x44,0x45} };
//
// message queue의 Timeout을 테스트 한다.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void thred_vk_msg_timeout2(void *arg)
{
	int ret;
	int i;

	VK_TASK_Sleep(200);
	ret = VK_MSG_Send(msgQid, &msg_data_send[0][0], 1);
	if(VK_OK != ret)
	{
		printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
		return;
	}
	VK_TASK_Sleep(200);
	ret = VK_MSG_Send(msgQid, &msg_data_send[0][1], 1);
	if(VK_OK != ret)
	{
		printf("%s(%d) : VK_MSG_SendTimeout Error\n", __func__, __LINE__);
		return;
	}
}

void thred_vk_msg_timeout1(void *arg)
{
	int ret;
	int i, j;
	unsigned long tick1, tick2;
	unsigned long timeouttest=100;
	HUINT8 msg_data_receive[Q_COUNT][Q_SIZE] = {0,};


	VK_TASK_Sleep(10);
	tick1 = VK_TIMER_GetSystemTick();
	ret = VK_MSG_ReceiveTimeout(msgQid, &msg_data_receive[0][0], 1, timeouttest);
	if(VK_TIMEOUT != ret)
	{
		printf("%s(%d) : VK_MSG_ReceiveTimeout Error\n", __func__, __LINE__);
		return;
	}
	tick2 = VK_TIMER_GetSystemTick();
	if (tick2-tick1 == timeouttest)
	{

	}
	else if ((tick2-tick1 > timeouttest*1.1) || (tick2-tick1 < timeouttest*0.9))
	{
		printf("%s(%d) : thred_vk_msg_timeout1 Error wanted timeout %d, real timeout %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
	}
	
	
 	PrintDebug("msg_data_receive = 0x%x\n", msg_data_receive[0][0]);

	VK_TASK_Sleep(300);
	ret = VK_MSG_ReceiveTimeout(msgQid, &msg_data_receive[0][0], 1, 100);
	if(VK_OK != ret)
	{
		printf("%s(%d) : VK_MSG_ReceiveTimeout Error\n", __func__, __LINE__);
		return;
	}
 	PrintDebug("msg_data_receive = 0x%x\n", msg_data_receive[0][0]);

	VK_TASK_Sleep(100);
	ret = VK_MSG_ReceiveTimeout(msgQid, &msg_data_receive[0][0], 1, 200);
	if(VK_OK != ret)
	{
		printf("%s(%d) : VK_MSG_ReceiveTimeout Error\n", __func__, __LINE__);
		return;
	}
 	PrintDebug("msg_data_receive = 0x%x\n", msg_data_receive[0][0]);

	msg_test_passed = TRUE;
}


static int test_vk_msg_timeout(void)
{
	HUINT32 testTaskId = 0;
	HUINT32 testTaskId2 = 0;
	int ret;
	int i,j;

	msg_test_passed = FALSE;

	ret = VK_MSG_Create(1, Q_SIZE, "test_vk_msg_error_return", &msgQid, VK_SUSPENDTYPE_FIFO);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Create(
	        thred_vk_msg_timeout1,
	        USER_PRIORITY0,
	        TIMER_TASK_STACK_SIZE,
	        "TestTask1",
	        NULL,
	        &testTaskId, FALSE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Start(testTaskId);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Start Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Create(
	        thred_vk_msg_timeout2,
	        USER_PRIORITY0,
	        TIMER_TASK_STACK_SIZE,
	        "TestTask2",
	        NULL,
	        &testTaskId2, FALSE);
	if(VK_OK != ret)
		goto __error_return_label;

	ret = VK_TASK_Start(testTaskId2);
	if(VK_OK != ret)
		goto __error_return_label;

	VK_TASK_Sleep(2000);

	if(TRUE == msg_test_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}

__error_return_label:
        printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}

static int test_vk_msg_send_timeout(void)
{
	HUINT32 testTaskId = 0;
	HUINT32 testTaskId2 = 0;
	int ret;
	int i,j;
	HUINT32	tick1, tick2;
	HUINT32	timeouttest;
	HUINT8 msg_data[Q_COUNT] = {0,};

	msg_test_passed = TRUE;

	ret = VK_TASK_Create(
	        thred_time_disturbance,
	        USER_PRIORITY0,
	        TIMER_TASK_STACK_SIZE,
	        "TestTask1",
	        NULL,
	        &testTaskId, FALSE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Start(testTaskId);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Start Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_MSG_Create(1, Q_SIZE, "test_vk_msg_error_return", &msgQid, VK_SUSPENDTYPE_FIFO);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_MSG_SendTimeout(msgQid, msg_data, Q_SIZE, 0);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_SendTimeout Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	for (i=0; i<100; i++)
	{
		timeouttest = i*10;
		tick1 = VK_TIMER_GetSystemTick();
		ret = VK_MSG_SendTimeout(msgQid, msg_data, Q_SIZE, timeouttest);
		if(VK_TIMEOUT != ret)
		{
			PrintError("%s(%d) : VK_MSG_SendTimeout Error\n", __func__, __LINE__);
			goto __error_return_label;
		}
		tick2 = VK_TIMER_GetSystemTick();
		if (tick2-tick1 == timeouttest)
		{
			printf("%s(%d) : VK_MSG_SendTimeout pass %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
		else if ((tick2-tick1 > timeouttest*1.1) || (tick2-tick1 < timeouttest*0.9))
		{
			printf("%s(%d) : VK_MSG_SendTimeout Error wanted timeout %d, real timeout %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
			msg_test_passed = FALSE;
		}
		else
		{
			printf("%s(%d) : VK_MSG_SendTimeout OK wanted timeout %d, real timeout %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
	}

	VK_TASK_Destroy(testTaskId);
	
	ret = VK_MSG_Destroy(msgQid);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
		goto __error_return_label;
	}
	msgQid = 0;
	
	if(TRUE == msg_test_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}
	
__error_return_label:
	ret = VK_MSG_Destroy(msgQid);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
	}
	msgQid = 0;
	
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}

static int test_vk_msg_receive_timeout(void)
{
	HUINT32 testTaskId = 0;
	HUINT32 testTaskId2 = 0;
	int ret;
	int i,j;
	HUINT32 tick1, tick2;
	HUINT32 timeouttest;
	HUINT8 msg_data[Q_COUNT] = {0,};

	msg_test_passed = TRUE;

	ret = VK_TASK_Create(
	        thred_time_disturbance,
	        USER_PRIORITY0,
	        TIMER_TASK_STACK_SIZE,
	        "TestTask1",
	        NULL,
	        &testTaskId, FALSE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Start(testTaskId);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Start Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_MSG_Create(1, Q_SIZE, "test_vk_msg_error_return", &msgQid, VK_SUSPENDTYPE_FIFO);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	for (i=0; i<100; i++)
	{
		timeouttest = i*10;
		tick1 = VK_TIMER_GetSystemTick();
		ret = VK_MSG_ReceiveTimeout(msgQid, msg_data, Q_SIZE, timeouttest);
		if(VK_TIMEOUT != ret)
		{
			PrintError("%s(%d) : VK_MSG_ReceiveTimeout Error\n", __func__, __LINE__);
			goto __error_return_label;
		}
		tick2 = VK_TIMER_GetSystemTick();
		if (tick2-tick1 == timeouttest)
		{
			printf("%s(%d) : VK_MSG_ReceiveTimeout pass %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
		else if ((tick2-tick1 > timeouttest*1.1) || (tick2-tick1 < timeouttest*0.9))
		{
			printf("%s(%d) : VK_MSG_ReceiveTimeout Error wanted timeout %d, real timeout %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
			msg_test_passed = FALSE;
		}
		else
		{
			printf("%s(%d) : VK_MSG_ReceiveTimeout OK wanted timeout %d, real timeout %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
	}

	VK_TASK_Destroy(testTaskId);
	ret = VK_MSG_Destroy(msgQid);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
		goto __error_return_label;
	}
	msgQid = 0;
	
	if(TRUE == msg_test_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}
	
__error_return_label:
	ret = VK_MSG_Destroy(msgQid);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
	}
	msgQid = 0;
	
	printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// message queue의 FIFO option으로 send/receive를 테스트 한다.
//
void thread_vk_task_send_receive1(void *arg)
{
	int ret;
	int i;

	PrintDebug("msg_data_send[0] = {");
	for(i=0; i<Q_SIZE; i++)
	{
		PrintDebug("0x%x, ", msg_data_send[0][i]);
	}
	PrintDebug("}\n");

	ret = VK_MSG_Send(msgQid, msg_data_send[0], Q_SIZE);
	if(VK_OK != ret)
	{
		printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
		return;
	}

	VK_TASK_Sleep(10);

	PrintDebug("msg_data_send[1] = {");
	for(i=0; i<Q_SIZE; i++)
	{
		PrintDebug("0x%x, ", msg_data_send[1][i]);
	}
	PrintDebug("}\n");
	ret = VK_MSG_Send(msgQid, msg_data_send[1], Q_SIZE);
	if(VK_OK != ret)
	{
		printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
		return;
	}
}

void thread_vk_task_send_receive2(void *arg)
{
	int ret;
	int i;

	PrintDebug("msg_data_send[2] = {");
	for(i=0; i<Q_SIZE; i++)
	{
		PrintDebug("0x%x, ", msg_data_send[2][i]);
	}
	PrintDebug("}\n");
	ret = VK_MSG_Send(msgQid, msg_data_send[2], Q_SIZE);
	if(VK_OK != ret)
	{
		printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
		return;
	}

	VK_TASK_Sleep(10);
	PrintDebug("msg_data_send[3] = {");
	for(i=0; i<Q_SIZE; i++)
	{
		PrintDebug("0x%x, ", msg_data_send[3][i]);
	}
	PrintDebug("}\n");
	ret = VK_MSG_Send(msgQid, msg_data_send[3], Q_SIZE);
	if(VK_OK != ret)
	{
		printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
		return;
	}
}

void thread_vk_task_send_receive3(void *arg)
{
	int ret;
	int i, j;
	HUINT8 msg_data_receive[Q_COUNT][Q_SIZE];

	for(i=0; i<Q_COUNT; i++)
	{
		VK_TASK_Sleep(10);
		ret = VK_MSG_Receive(msgQid, msg_data_receive[i], Q_SIZE);
		if(VK_OK != ret)
		{
			printf("%s(%d) : VK_MSG_Receive Error\n", __func__, __LINE__);
			return;
		}
	}

	for(i=0; i<Q_COUNT; i++)
	{
		PrintDebug("msg_data_receive[%d] = {", i);
		for(j=0; j<Q_SIZE; j++)
		{
			PrintDebug("0x%X, ", msg_data_receive[i][j]);
		}
		PrintDebug("}\n");
	}

	for(j=0; j<Q_SIZE; j++)
	{
		if(msg_data_send[0][j] != msg_data_receive[0][j])
		{
			printf("msg_data_send[0][%d] = 0x%x\n", j, msg_data_send[0][j]);
			printf("msg_data_receive[0][%d] = 0x%x\n", j, msg_data_receive[0][j]);
			msg_test_passed = FALSE;
			return;
		}
	}

	for(j=0; j<Q_SIZE; j++)
	{
		if(msg_data_send[2][j] != msg_data_receive[1][j])
		{
			printf("msg_data_send[0][%d] = 0x%x\n", j, msg_data_send[2][j]);
			printf("msg_data_receive[0][%d] = 0x%x\n", j, msg_data_receive[1][j]);
			msg_test_passed = FALSE;
			return;
		}
	}

	for(j=0; j<Q_SIZE; j++)
	{
		if(msg_data_send[1][j] != msg_data_receive[2][j])
		{
			printf("msg_data_send[0][%d] = 0x%x\n", j, msg_data_send[1][j]);
			printf("msg_data_receive[0][%d] = 0x%x\n", j, msg_data_receive[2][j]);
			msg_test_passed = FALSE;
			return;
		}
	}

	for(j=0; j<Q_SIZE; j++)
	{
		if(msg_data_send[3][j] != msg_data_receive[3][j])
		{
			printf("msg_data_send[0][%d] = 0x%x\n", j, msg_data_send[3][j]);
			printf("msg_data_receive[0][%d] = 0x%x\n", j, msg_data_receive[3][j]);
			msg_test_passed = FALSE;
			return;
		}
	}


	msg_test_passed = TRUE;
}

static int test_vk_msg_send_receive(void)
{
	int ret;
	int i,j;

	msg_test_passed = FALSE;

	ret = VK_MSG_Create(Q_COUNT, Q_SIZE, "test_vk_msg_send_receive", &msgQid, VK_SUSPENDTYPE_FIFO);
	if(VK_OK != ret)
		goto __error_return_label;

	ret = VK_TASK_Create(
	        thread_vk_task_send_receive1,
	        USER_PRIORITY0,
	        TIMER_TASK_STACK_SIZE,
	        "msg_q_task_id",
	        NULL,
	        &msg_q_task_id, FALSE);
	if(VK_OK != ret)
		goto __error_return_label;

	ret = VK_TASK_Create(
	        thread_vk_task_send_receive2,
	        USER_PRIORITY0,
	        TIMER_TASK_STACK_SIZE,
	        "msg_q_task_id2",
	        NULL,
	        &msg_q_task_id2, FALSE);
	if(VK_OK != ret)
		goto __error_return_label;

	ret = VK_TASK_Create(
	        thread_vk_task_send_receive3,
	        USER_PRIORITY0,
	        TIMER_TASK_STACK_SIZE,
	        "msg_q_task_id3",
	        NULL,
	        &msg_q_task_id3, FALSE);
	if(VK_OK != ret)
		goto __error_return_label;

	ret = VK_TASK_Start(msg_q_task_id);
	if(VK_OK != ret)
		goto __error_return_label;


	ret = VK_TASK_Start(msg_q_task_id2);
	if(VK_OK != ret)
		goto __error_return_label;

	ret = VK_TASK_Start(msg_q_task_id3);
	if(VK_OK != ret)
		goto __error_return_label;

	VK_TASK_Sleep(1000);

	if(TRUE == msg_test_passed)
	{
		printf("%s [RESULT] Pass\n", __func__);
		return VK_OK;
	}

__error_return_label:
        printf("%s [RESULT] Fail\n", __func__);
	return VK_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

