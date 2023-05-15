/**
 * unittest_vk.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "linden_trace.h"

#include "unittest.h"

#include "di_err.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_VK
#define VALID_UT_UNIXTIME	(60 * 60 * 24 * 366)	// until 1971/01/02
#define MAX_NUM_SEM 160

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



////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////VK_CLOCK TEST START////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// unix time을 set/get을 테스트 한다.
//

static UT_RESULT ut_vk_clock_time(void)
{
	int errCode;
	HULONG unix_time;

	unix_time = VALID_UT_UNIXTIME;
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

	PrintDebug("unix_time = 0x%16llX\n", unix_time);
	if (unix_time == VALID_UT_UNIXTIME)
	{
		VK_printf("Success - exact time was returned\n");
	}
	else
	{
		VK_printf("\n");
		VK_printf("Fail - error is more than 1s\n");
		goto __error_return_label;
	}

	return UT_OK;

__error_return_label:
	return UT_FAIL;
}


static UT_RESULT ut_vk_clock_offset_time(void)
{
	int errCode;
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

	return UT_OK;

__error_return_label:
	return UT_FAIL;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////VK_TIMER TEST START////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TIME_OUT_VAL  1000
#define TIME_OUT_ERR_LIMIT  10
static HULONG test_timer_id;
static HBOOL test_timer_passed;
static HUINT32 test_timer_count;
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 등록한 Timer가 제대로 Cancel되는지 테스트 한다.
// TIME_OUT_ERR_LIMIT mili정도를 오차 범위로 하였음.
//
static void timer_event_after_cancel(unsigned long timer_id, void *arg)
{
	HUINT64 milisecond;

	if(test_timer_id == timer_id)
	{
		test_timer_passed = FALSE;
		milisecond = VK_TIMER_GetSystemTick();
		PrintDebug("milisecond4= %lld, count = %ld\n", milisecond, test_timer_count);
	}
}

static void timer_event_every_cancel(unsigned long timer_id, void *arg)
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

static UT_RESULT ut_vk_timer_event_cancel(void)
{
	int errCode;
	HUINT64 milisecond;
	UT_RESULT	result = UT_FAIL;

	test_timer_id = 0;
	test_timer_passed = FALSE;
	test_timer_count = 0;

	errCode = VK_TIMER_EventAfter(TIME_OUT_VAL, timer_event_after_cancel, NULL, 0, &test_timer_id);
	if(VK_OK != errCode)
	{
		PrintError("%s(%d) : VK_TIMER_EventAfter Error\n", __func__, __LINE__);
		result = UT_FAIL;
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
		return UT_OK;
	}

__error_return_label:
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 지정된 Time후에 Cancel하기 전까지 등록한 func을 지정된 시간 마다 호출하는지를 테스트 한다.
// TIME_OUT_ERR_LIMIT mili정도를 오차 범위로 하였음.
//
static void timer_event_every(unsigned long timer_id, void *arg)
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

static UT_RESULT ut_vk_timer_event_every(void)
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
		return UT_OK;
	}

__error_return_label:
	return UT_FAIL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 지정된 Time후에 등록한 func을 호출하는지를 테스트 한다.
// TIME_OUT_ERR_LIMIT mili정도를 오차 범위로 하였음.
//
static void timer_event_after(unsigned long timer_id, void *arg)
{
	HUINT64 milisecond;

	if(test_timer_id == timer_id)
	{
		test_timer_passed = TRUE;
		milisecond = VK_TIMER_GetSystemTick();
		PrintDebug("milisecond3= %lld\n", milisecond);
	}
}

static UT_RESULT ut_vk_timer_event_after(void)
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

	/***
	Check test_timer_passed after 500ms
	***/
	VK_TASK_Sleep(TIME_OUT_VAL/2);
	if(FALSE != test_timer_passed)
	{
		return UT_FAIL;
	}

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond1= %lld\n", milisecond);

	VK_TASK_Sleep(TIME_OUT_VAL/2+TIME_OUT_ERR_LIMIT);

	milisecond = VK_TIMER_GetSystemTick();
	PrintDebug("milisecond2= %lld\n", milisecond);

	if(TRUE == test_timer_passed)
	{
		return UT_OK;
	}

__error_return_label:
	return UT_FAIL;
}

#define TIMEOUT_DIFF_TEST 5000
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// milisecond단위로 sleep후에 get시에 정확한 sleep시간 차이가 나는지 테스트 한다.
// 오차 범위내에 Pass되면 성공
//
static UT_RESULT ut_vk_time_diff(void)
{
	HUINT64 milisecond;
	HUINT64 milisecond2;
	HUINT64 elapse;

	milisecond = VK_TIMER_GetSystemTick();

	VK_TASK_Sleep(TIMEOUT_DIFF_TEST);

	milisecond2 = VK_TIMER_GetSystemTick();

	elapse = (milisecond2 - milisecond);

	if (TIMEOUT_DIFF_TEST ==  elapse)
	{
	    VK_printf("Success - exact time was returned\n");
	}
	else if ( (elapse > (TIMEOUT_DIFF_TEST*0.9)) && (elapse < (TIMEOUT_DIFF_TEST*1.1)) )
	{
	    VK_printf("Success - error is less than 10\n");
	}
	else
	{
		VK_printf("Fail - error is more than 10\n");
		VK_printf("elapse = 0x%llX\n", elapse);
		goto __error_return_label;
	}

	return UT_OK;

__error_return_label:
	return UT_FAIL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////VK_SEM TEST START//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
HBOOL test_sem_passed;
HBOOL test_sem_ready;
HULONG sem_g;
HULONG task_g;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//VK_SEM_GetTimeout()을 테스트 한다.
//
// system time disturbance task
static void thred_time_disturbance(void *arg)
{
	unsigned long unixTime;

	VK_printf("start system time disturbance task\n");
	unixTime = 0x12345678;
	for (;;)
	{
		VK_TASK_Sleep(10);
		unixTime += 1000;
		VK_CLOCK_SetTime(0, unixTime);
	}
}

static UT_RESULT ut_vk_sem_timeout(void)
{
	HULONG testTaskId = 0;
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
        goto __error_return_label;
    }

	for (i=0; i<50; i++)
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
			VK_printf("%s(%4d) : VK_SEM_GetTimeout Pass %d, %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
		else if (tick2-tick1 < timeouttest)
		{
			VK_printf("%s(%4d) : VK_SEM_GetTimeout Error, wanted timeout %3d, real timeout %3d\n", __func__, __LINE__, timeouttest, tick2-tick1);
			test_sem_passed = FALSE;
		}
		else
		{
			VK_printf("%s(%4d) : VK_SEM_GetTimeout OK   , wanted timeout %3d, real timeout %3d\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}

	}

	VK_TASK_Destroy(testTaskId);

	if(TRUE == test_sem_passed)
	{
		return UT_OK;
	}

__error_return_label:
	ret = VK_SEM_Destroy(sem_g);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
		goto __error_return_label;
	}
	sem_g = 0;

	return UT_FAIL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//VK_SEM_Create/VK_SEM_Destroy을 반복하므로 인해 메모리 부족이 일어 나는지 테스트 한다.
//
static UT_RESULT ut_vk_sem_create_destroy(void)
{
    DI_ERR_CODE ret;
    HUINT32 trial_num = 10000;
    HUINT32 i;

    VK_printf("%s start\n", __func__);

    sem_g = 0;

    for(i=0; i<trial_num; i++)
    {
        ret = VK_SEM_Create(&sem_g, "sem", VK_SUSPENDTYPE_PRIORITY);
        if(VK_OK != ret)
        {
            PrintError("VK_SEM_Create Error!![%s(%d)]\n", __func__, __LINE__);
            goto __error_return_label;
        }
        VK_printf("VK_SEM_Create %d\n", i);

        ret = VK_SEM_Destroy(sem_g);
        if(ret != VK_OK)
        {
            PrintError("VK_SEM_Destroy Error!![%s(%d)]\n", __func__, __LINE__);
            goto __error_return_label;
        }
        VK_printf("VK_SEM_Destroy %d\n", i);
    }

	return UT_OK;

__error_return_label:
	return UT_FAIL;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//VK_SEM_CreateWithCount을 테스트 한다.
//
static void thread_task_sem_count(void *arg)
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

static UT_RESULT ut_vk_sem_count(void)
{
    DI_ERR_CODE ret;

    VK_printf("%s start\n", __func__);

    test_sem_passed  = FALSE;
    sem_g = 0;
    task_g = 0;

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
        NULL,
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

	return UT_OK;

__error_return_label:
	return UT_FAIL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//VK_SEM_Create을 테스트 한다.
//
void thread_task_sem_basic(void *arg)
{
    DI_ERR_CODE ret;

	DI_UART_Print("[thread_task_sem_basic] begin\n");
	DI_UART_Print("[thread_task_sem_basic] sleep 2000\n");
    VK_TASK_Sleep(2000);
	DI_UART_Print("[thread_task_sem_basic] wakeup after\n");

    test_sem_ready = TRUE;

    ret = VK_SEM_Release(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Release Error!![%s(%d)]\n", __func__, __LINE__);
        return;
    }
	DI_UART_Print("[thread_task_sem_basic] test semaphore released\n");

    test_sem_passed = TRUE;
}


static UT_RESULT ut_vk_sem_basic(void)
{
    DI_ERR_CODE ret;
    unsigned long tid;
    unsigned int index, index2;
    unsigned long sema_array[MAX_NUM_SEM];


    DI_UART_Print("%s start\n", __func__);

    test_sem_passed  = FALSE;
    test_sem_ready   = FALSE;
    sem_g = 0;
    task_g = 0;

    for(index=0; index<MAX_NUM_SEM; index++)
    {
        ret = VK_SEM_Create(&sema_array[index], "sem", VK_SUSPENDTYPE_PRIORITY);
        if(VK_OK != ret){
            for(index2=0; index2<index; index2++){
                VK_SEM_Destroy(sema_array[index2]);
                if(ret != VK_OK)
                {
                    PrintError("VK_SEM_Destroy Error!![%s(%d)]\n", __func__, __LINE__);
                    goto __error_return_label;
                }
            }
            break;
        }
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
        goto __error_return_label;
    }

    ret = VK_TASK_Create(
        thread_task_sem_basic,
        USER_PRIORITY0,
        NORMAL_TASK_STACK_SIZE,
        "thread_task_sem_basic",
        NULL,
        &task_g,
        FALSE);

    if(VK_OK != ret)
    {
        PrintError("VK_TASK_Create Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }
	DI_UART_Print("test task created (0x%08X)\n", task_g);

    VK_TASK_Start(task_g);
	DI_UART_Print("test task started (0x%08X)\n", task_g);

	DI_UART_Print("waiting sempahore (0x%08X)\n", task_g);
    ret = VK_SEM_Get(sem_g);
    if(ret != VK_OK)
    {
        PrintError("VK_SEM_Get Error!![%s(%d)]\n", __func__, __LINE__);
        goto __error_return_label;
    }
	DI_UART_Print("waiting sempahore PASSED (0x%08X)\n", task_g);

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
	sem_g = 0;

    if (test_sem_passed == FALSE)
    {
        goto __error_return_label;
    }

	return UT_OK;

__error_return_label:
	return UT_FAIL;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////VK_TASK TEST START//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NUM_OF_TIME_DURATION 8
#define SLEEP_TIME 2000
HBOOL task_test_ready;
HBOOL task_test_passed;
static HULONG ut_vk_task_id;
static HUINT32 threadArg = (HUINT32) 0xFEDCBA98;
static HULONG ut_current_task_id;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// thread의 join option을 테스트 한다.
//
void thread_vk_task_join(void *arg)
{
	int i;
	VK_TASK_GetCurrentId(&ut_current_task_id);
	for(i=0; i<10; i++)
	{
		VK_TASK_Sleep(1000);
		VK_printf("%d second\n", i);
	}
	task_test_passed = TRUE;
}

static UT_RESULT ut_vk_task_join(void)
{
	int errCode;

	ut_vk_task_id = 0;
	task_test_passed = FALSE;

	errCode = VK_TASK_Create(thread_vk_task_join, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_vk_task_join", NULL, &ut_vk_task_id, TRUE);
	if(errCode != VK_OK)
	{
	    PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
	    goto __error_return_label;
	}
	errCode = VK_TASK_Start(ut_vk_task_id);
	if(errCode != VK_OK)
	{
	    PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
	    goto __error_return_label;
	}

	errCode = VK_TASK_Join(ut_vk_task_id);
	if(errCode != VK_OK)
	{
	    PrintError("%s(%d) : VK_TASK_Join Error!!\n", __func__, __LINE__);
	    goto __error_return_label;
	}

	if(TRUE == task_test_passed)
	{
		return UT_OK;
	}

__error_return_label:
	return UT_FAIL;
}

static UT_RESULT ut_vk_task_getcurrentid(void)
{
	int errCode;

	ut_vk_task_id = 0;
	task_test_passed = FALSE;

	errCode = VK_TASK_Create(thread_vk_task_join, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_vk_task_join", NULL, &ut_vk_task_id, TRUE);
	if(errCode != VK_OK)
	{
	    PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
	    goto __error_return_label;
	}
	errCode = VK_TASK_Start(ut_vk_task_id);
	if(errCode != VK_OK)
	{
	    PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
	    goto __error_return_label;
	}

	VK_TASK_Sleep(100);

	errCode = VK_TASK_Destroy(ut_vk_task_id);
	if(errCode != VK_OK)
	{
	    PrintError("%s(%d) : VK_TASK_Join Error!!\n", __func__, __LINE__);
	    goto __error_return_label;
	}
	if (ut_current_task_id == ut_vk_task_id)
	{
		return UT_OK;
	}

__error_return_label:
	return UT_FAIL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// thread의 sleep의 정확도를 테스트 한다.
//
static UT_RESULT ut_vk_task_advance_sleep(void)
{
    HUINT32 i;
    HUINT32 time_duration[] =
        { 500, 1000, 2500, 10000, 15000, 20000, 50000, 100000 };

    HUINT64 before, after;
    HUINT32 elapse;

    for( i = 0; i < NUM_OF_TIME_DURATION; i++) {
        before = VK_TIMER_GetSystemTick();

        VK_printf("Sleep %d mill seconds.\n", time_duration[i]);

        VK_TASK_Sleep(time_duration[i]);

        after = VK_TIMER_GetSystemTick();

        elapse = (HUINT32)(after - before);

        if (elapse == time_duration[i]) {
            VK_printf("Exact time elapsed. Success.\n");
        } else if (elapse > (time_duration[i] * 0.9)
                && (elapse < (time_duration[i] * 1.1))) {
            VK_printf("%d elapsed.\n", elapse);
            VK_printf("[MSG] Success: The difference between elapsed");
            VK_printf(
                    " time and expected duration is within 10%% error rate.\n");
        } else {
            VK_printf("%d elapsed.\n", elapse);
            VK_printf("Fail: The diifernce is out of range.\n");
			return UT_FAIL;
        }
    }

	return UT_OK;
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
        VK_printf("%s(%d) : i = %d\n", __func__, __LINE__, i);
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
        VK_printf("%s(%d) : i = %d\n", __func__, __LINE__, i);
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

static UT_RESULT ut_vk_task_priority(void)
{
    HULONG task_id = 0;
    HULONG task_id2 = 0;
    DI_ERR_CODE errCode = DI_ERR_OK;

    task_test_passed = FALSE;
    task_test_ready = FALSE;

    errCode = VK_TASK_Create(thread_vk_task_priority1, USER_PRIORITY14, NORMAL_TASK_STACK_SIZE, "thread_vk_task_priority1", NULL, &task_id, FALSE);
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

    errCode = VK_TASK_Create(thread_vk_task_priority2, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_vk_task_priority2", NULL, &task_id2, FALSE);
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
		return UT_FAIL;
    }

	return UT_OK;

__error_return_label:
	return UT_FAIL;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// thread의 create/destroy를 테스트 한다.
//
void thread_vk_task_create_destroy(void  *arg)
{
	while (1)
	{
	    VK_TASK_Sleep(20);
	    DI_UART_Print("%s runs id = %d\n", __func__, *(int *)arg);
	}
}

static unsigned long s_tread_task_create_destroy_id;
void thread_task_create_destroy(void  *arg)
{
    HULONG task_id = 0;
    DI_ERR_CODE errCode = DI_ERR_OK;
    int i;

    for(i=0; i<64; i++)
    {
        errCode = VK_TASK_Create(thread_vk_task_create_destroy, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_vk_task_create_destroy", (void *)&i, &task_id, FALSE);
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
		DI_UART_Print("[thread_task_create_destroy] task start command (%d)\n", i);

        VK_TASK_Sleep(30);

        errCode = VK_TASK_Destroy(task_id);
        if(errCode != VK_OK)
        {
            PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
            goto __error_return_label;
        }
		DI_UART_Print("[thread_task_create_destroy] task Destroy command (%d)\n", i);
		VK_TASK_Sleep(30);
    }


	DI_UART_Print("[thread_task_create_destroy] task self Destroy command task id (%08X)\n", s_tread_task_create_destroy_id);
	task_test_passed = TRUE;
	task_test_ready = FALSE;

	// destroy task by self.
	errCode = VK_TASK_Destroy(s_tread_task_create_destroy_id);
	if(errCode != VK_OK)
	{
		PrintError("%s(%d) : VK_TASK_Destroy Error!!\n", __func__, __LINE__);
		goto __error_return_label;
	}

	VK_TASK_Sleep(1000);	// this task will be destroyed at here.

	task_test_passed = FALSE;
	task_test_ready = FALSE;

	return ;

__error_return_label:
	task_test_passed = FALSE;
    task_test_ready = FALSE;

	// destroy task by self.
	errCode = VK_TASK_Destroy(*(unsigned long*)arg);
	if(errCode != VK_OK)
	{
		PrintError("%s(%d) : VK_TASK_Destroy Error!!\n", __func__, __LINE__);
	}

    return ;
}

static UT_RESULT ut_vk_task_create_destroy(void)
{
    DI_ERR_CODE errCode = DI_ERR_OK;
    int i;

    task_test_ready = TRUE;
	errCode = VK_TASK_Create(thread_task_create_destroy, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "thread_task_create_destroy", (void *)&i, &s_tread_task_create_destroy_id, FALSE);
	if(errCode != VK_OK)
	{
		PrintError("%s(%d) : VK_TASK_Create Error!!\n", __func__, __LINE__);
		return 2;
	}

	errCode = VK_TASK_Start(s_tread_task_create_destroy_id);
	if(errCode != VK_OK)
	{
		PrintError("%s(%d) : VK_TASK_Start Error!!\n", __func__, __LINE__);
		return 1;
	}

	while (task_test_ready)
	{
		VK_TASK_Sleep(10);
	}

	if (task_test_passed == TRUE)
		return UT_OK;


    return UT_FAIL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Thread에게 argument가 제대로 전달 되는지를 테스트 한다.
//
void thread_vk_task_argument(void *arg)
{
    HUINT32 val;
    val = *(HUINT32 *)arg;
    VK_printf("val = 0x%x\n", val);
    VK_printf("threadArg = 0x%x\n", threadArg);
    if(val == threadArg)
    {
        task_test_passed = TRUE;
    }
}

static UT_RESULT ut_vk_task_argument(void)
{
    DI_ERR_CODE errCode = DI_ERR_OK;
    HULONG task_id = 0;

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
		return UT_FAIL;
    }

	return UT_OK;

__error_return_label:
	return UT_FAIL;
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
        VK_printf("Running in loop No %d\n", i);

        if (i % 5 == 0) {
            VK_printf("Sleeping for %d seconds\n",
                    (SLEEP_TIME / 1000));

            VK_TASK_Sleep(SLEEP_TIME);
        }
    }
    task_test_passed = TRUE;
}

static UT_RESULT ut_vk_task_sleep(void)
{
    HULONG task_id = 0;
    DI_ERR_CODE errCode = DI_ERR_OK;

    task_test_passed = FALSE;
    errCode = VK_TASK_Create(thread_vk_task_sleep, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "ut_vk_task_sleep", 0, &task_id, FALSE);
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
    }
    else
    {
		return UT_FAIL;
    }

	return UT_OK;

__error_return_label:
	return UT_FAIL;
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

static UT_RESULT ut_vk_task_basic(void)
{
    HULONG task_id = 0;
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
    }
    else
    {
		return UT_FAIL;
    }

	return UT_OK;

__error_return_label:
	return UT_FAIL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////VK_MSG////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define Q_SIZE 5
#define Q_COUNT 4
static HULONG msgQid = 0;
static HBOOL msg_test_passed = FALSE;
static HUINT8 msg_data_send[Q_COUNT][Q_SIZE] = {{0x11,0x12,0x13,0x14,0x14},
									 {0x21,0x22,0x23,0x24,0x25},
 									 {0x31,0x32,0x33,0x34,0x35},
									 {0x41,0x42,0x43,0x44,0x45} };
static HULONG msg_q_task_id;
static HULONG msg_q_task_id2;
static HULONG msg_q_task_id3;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// task의 priority에 따라 receive가 되는지 검사 한다. (message queue의 resource option은 priority로 설정)
//
void thread_vk_msg_send_priority1(void *arg)
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

void thread_vk_msg_send_priority2(void *arg)
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

void thread_vk_msg_receive_and_cheak(void *arg)
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

/***
This test (msg priority) does not affect in Linux environment
***/
static UT_RESULT ut_vk_msg_priority(void)
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

	ret = VK_TASK_Create(thread_vk_msg_send_priority1,
		USER_PRIORITY0,
		NORMAL_TASK_STACK_SIZE,
		"thread_vk_msg_send_priority1",
		NULL, &msg_q_task_id, FALSE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_TASK_Create(thread_vk_msg_send_priority2,
		USER_PRIORITY2,
		NORMAL_TASK_STACK_SIZE,
		"thread_vk_msg_send_priority2",
		NULL, &msg_q_task_id2, FALSE);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		goto __error_return_label;
	}
	ret = VK_TASK_Create(thread_vk_msg_receive_and_cheak,
		USER_PRIORITY3,
		NORMAL_TASK_STACK_SIZE,
		"thread_vk_msg_receive_and_cheak",
		NULL, &msg_q_task_id3, FALSE);
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
		return UT_OK;
	}

__error_return_label:
	return UT_FAIL;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// message queue의 create/destroy를 테스트 한다.
//
static UT_RESULT ut_vk_msg_create_destroy(void)
{
	int ret;
	int i;

	for(i=0; i<10000; i++)
	{
		ret = VK_MSG_Create(Q_COUNT, Q_SIZE, "ut_vk_msg_error_return", &msgQid, VK_SUSPENDTYPE_FIFO);
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

	return UT_OK;

__error_return_label:
	return UT_FAIL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 {0x41,0x42,0x43,0x44,0x45} };
//
// message queue의 Timeout을 테스트 한다.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void thred_vk_msg_timeout2(void *arg)
{
	int ret;

	VK_TASK_Sleep(200);
	ret = VK_MSG_Send(msgQid, &msg_data_send[0][0], 1);
	if(VK_OK != ret)
	{
		VK_printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
		return;
	}
	VK_TASK_Sleep(200);
	ret = VK_MSG_Send(msgQid, &msg_data_send[0][1], 1);
	if(VK_OK != ret)
	{
		VK_printf("%s(%d) : VK_MSG_SendTimeout Error\n", __func__, __LINE__);
		return;
	}
}

void thred_vk_msg_timeout1(void *arg)
{
	int ret;
	unsigned long tick1, tick2;
	unsigned long timeouttest=100;
	HUINT8 msg_data_receive[Q_COUNT][Q_SIZE] = {0,};


	VK_TASK_Sleep(10);
	tick1 = VK_TIMER_GetSystemTick();
	ret = VK_MSG_ReceiveTimeout(msgQid, &msg_data_receive[0][0], 1, timeouttest);
	if(VK_TIMEOUT != ret)
	{
		VK_printf("%s(%d) : VK_MSG_ReceiveTimeout Error\n", __func__, __LINE__);
		return;
	}
	tick2 = VK_TIMER_GetSystemTick();
	if (tick2-tick1 == timeouttest)
	{

	}
	else if ((tick2-tick1 > timeouttest*1.1) || (tick2-tick1 < timeouttest*0.9))
	{
		VK_printf("%s(%d) : thred_vk_msg_timeout1 Error wanted timeout %lu, real timeout %lu\n", __func__, __LINE__, timeouttest, tick2-tick1);
	}


 	PrintDebug("msg_data_receive = 0x%x\n", msg_data_receive[0][0]);

	VK_TASK_Sleep(100);
	ret = VK_MSG_ReceiveTimeout(msgQid, &msg_data_receive[0][0], 1, 100);
	if(VK_OK != ret)
	{
		VK_printf("%s(%d) : VK_MSG_ReceiveTimeout Error\n", __func__, __LINE__);
		return;
	}
 	PrintDebug("msg_data_receive = 0x%x\n", msg_data_receive[0][0]);

	VK_TASK_Sleep(100);
	ret = VK_MSG_ReceiveTimeout(msgQid, &msg_data_receive[0][0], 1, 200);
	if(VK_OK != ret)
	{
		VK_printf("%s(%d) : VK_MSG_ReceiveTimeout Error\n", __func__, __LINE__);
		return;
	}
 	PrintDebug("msg_data_receive = 0x%x\n", msg_data_receive[0][0]);

	msg_test_passed = TRUE;
}


static UT_RESULT ut_vk_msg_timeout(void)
{
	HULONG testTaskId = 0;
	HULONG testTaskId2 = 0;
	int ret;

	msg_test_passed = FALSE;

	ret = VK_MSG_Create(1, Q_SIZE, "ut_vk_msg_error_return", &msgQid, VK_SUSPENDTYPE_FIFO);
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
		return UT_OK;
	}

__error_return_label:
	return UT_FAIL;
}

static UT_RESULT ut_vk_msg_send_timeout(void)
{
	HULONG testTaskId = 0;
	HULONG testTaskId2 = 0;
	int ret;
	int i;
	HULONG	tick1, tick2;
	HULONG	timeouttest;
	HUINT8 msg_data = msg_data_send[0][0];

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
		return UT_FAIL;
	}

	ret = VK_TASK_Start(testTaskId);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Start Error\n", __func__, __LINE__);
		return UT_FAIL;
	}

	ret = VK_MSG_Create(1, Q_SIZE, "ut_vk_msg_error_return", &msgQid, VK_SUSPENDTYPE_FIFO);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		return UT_FAIL;
	}

	ret = VK_MSG_SendTimeout(msgQid, &msg_data, Q_SIZE, 0);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_SendTimeout happened Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_MSG_SendTimeout(msgQid, &msg_data, Q_SIZE, 0);
	if(VK_OK == ret)
	{
		PrintError("%s(%d) : VK_MSG_SendTimeout should be Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	for (i=0; i<100; i++)
	{
		timeouttest = i*10;
		tick1 = VK_TIMER_GetSystemTick();
		ret = VK_MSG_SendTimeout(msgQid, &msg_data, Q_SIZE, timeouttest);
		if(VK_TIMEOUT != ret)
		{
			PrintError("%s(%d) : VK_MSG_SendTimeout Error\n", __func__, __LINE__);
			goto __error_return_label;
		}
		tick2 = VK_TIMER_GetSystemTick();
		if (tick2-tick1 == timeouttest)
		{
			VK_printf("%s(%d) : VK_MSG_SendTimeout pass %u,%u\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
		else if ((tick2-tick1 > timeouttest*1.1) || (tick2-tick1 < timeouttest*0.9))
		{
			VK_printf("%s(%d) : VK_MSG_SendTimeout Error wanted timeout %u, real timeout %u\n", __func__, __LINE__, timeouttest, tick2-tick1);
			msg_test_passed = FALSE;
		}
		else
		{
			VK_printf("%s(%d) : VK_MSG_SendTimeout OK wanted timeout %u, real timeout %u\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
	}

	VK_TASK_Destroy(testTaskId);

	ret = VK_MSG_Destroy(msgQid);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
		return UT_FAIL;
	}
	msgQid = 0;

	if(TRUE == msg_test_passed)
	{
		return UT_OK;
	}

__error_return_label:
	ret = VK_MSG_Destroy(msgQid);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
	}
	msgQid = 0;

	return UT_FAIL;
}

static UT_RESULT ut_vk_msg_send_urgent_timeout(void)
{
	HULONG testTaskId = 0;
	HULONG testTaskId2 = 0;
	int ret;
	int i;
	HULONG	tick1, tick2;
	HULONG	timeouttest;
	HUINT8 msg_data = msg_data_send[0][0];

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
		return UT_FAIL;
	}

	ret = VK_TASK_Start(testTaskId);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_TASK_Start Error\n", __func__, __LINE__);
		return UT_FAIL;
	}

	ret = VK_MSG_Create(1, Q_SIZE, "ut_vk_msg_error_return", &msgQid, VK_SUSPENDTYPE_FIFO);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Create Error\n", __func__, __LINE__);
		return UT_FAIL;
	}

	ret = VK_MSG_SendUrgentTimeout(msgQid, &msg_data, Q_SIZE, 0);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_SendUrgentTimeout happened Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	ret = VK_MSG_SendUrgentTimeout(msgQid, &msg_data, Q_SIZE, 0);
	if(VK_OK == ret)
	{
		PrintError("%s(%d) : VK_MSG_SendUrgentTimeout should be Error\n", __func__, __LINE__);
		goto __error_return_label;
	}

	for (i=0; i<100; i++)
	{
		timeouttest = i*10;
		tick1 = VK_TIMER_GetSystemTick();
		ret = VK_MSG_SendUrgentTimeout(msgQid, &msg_data, Q_SIZE, timeouttest);
		if(VK_TIMEOUT != ret)
		{
			PrintError("%s(%d) : VK_MSG_SendUrgentTimeout Error\n", __func__, __LINE__);
			goto __error_return_label;
		}
		tick2 = VK_TIMER_GetSystemTick();
		if (tick2-tick1 == timeouttest)
		{
			VK_printf("%s(%d) : VK_MSG_SendUrgentTimeout pass %u,%u\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
		else if ((tick2-tick1 > timeouttest*1.1) || (tick2-tick1 < timeouttest*0.99))
		{
			VK_printf("%s(%d) : VK_MSG_SendUrgentTimeout Error wanted timeout %u, real timeout %u\n", __func__, __LINE__, timeouttest, tick2-tick1);
			msg_test_passed = FALSE;
		}
		else
		{
			VK_printf("%s(%d) : VK_MSG_SendUrgentTimeout OK wanted timeout %u, real timeout %u\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
	}

	VK_TASK_Destroy(testTaskId);

	ret = VK_MSG_Destroy(msgQid);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
		return UT_FAIL;
	}
	msgQid = 0;

	if(TRUE == msg_test_passed)
	{
		return UT_OK;
	}

__error_return_label:
	ret = VK_MSG_Destroy(msgQid);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
	}
	msgQid = 0;

	return UT_FAIL;
}


static UT_RESULT ut_vk_msg_receive_timeout(void)
{
	HULONG testTaskId = 0;
	HULONG testTaskId2 = 0;
	int ret;
	int i;
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

	ret = VK_MSG_Create(1, Q_SIZE, "ut_vk_msg_error_return", &msgQid, VK_SUSPENDTYPE_FIFO);
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
			VK_printf("%s(%d) : VK_MSG_ReceiveTimeout pass %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
		}
		else if ((tick2-tick1 > timeouttest*1.1) || (tick2-tick1 < timeouttest*0.9))
		{
			VK_printf("%s(%d) : VK_MSG_ReceiveTimeout Error wanted timeout %d, real timeout %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
			msg_test_passed = FALSE;
		}
		else
		{
			VK_printf("%s(%d) : VK_MSG_ReceiveTimeout OK wanted timeout %d, real timeout %d\n", __func__, __LINE__, timeouttest, tick2-tick1);
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
		return UT_OK;
	}

__error_return_label:
	ret = VK_MSG_Destroy(msgQid);
	if(VK_OK != ret)
	{
		PrintError("%s(%d) : VK_MSG_Destroy Error\n", __func__, __LINE__);
	}
	msgQid = 0;

	return UT_FAIL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// message queue의 FIFO option으로 send/receive를 테스트 한다.
//

static HUINT32 eventToTask1;
static HUINT32 eventToTask2;


void thread_vk_task_send_receive1(void *arg)
{
	int ret;
	int i;

	eventToTask1 = 0;
	while (eventToTask1 == 0) VK_TASK_Sleep(10);

	PrintDebug("msg_data_send[0] = {");
	for(i=0; i<Q_SIZE; i++)
	{
		PrintDebug("0x%x, ", msg_data_send[0][i]);
	}
	PrintDebug("}\n");

	ret = VK_MSG_Send(msgQid, msg_data_send[0], Q_SIZE);
	if(VK_OK != ret)
	{
		VK_printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
		return;
	}

	eventToTask2 = 1;

	eventToTask1 = 0;
	while (eventToTask1 == 0) VK_TASK_Sleep(10);

	PrintDebug("msg_data_send[1] = {");
	for(i=0; i<Q_SIZE; i++)
	{
		PrintDebug("0x%x, ", msg_data_send[1][i]);
	}
	PrintDebug("}\n");
	ret = VK_MSG_Send(msgQid, msg_data_send[1], Q_SIZE);
	if(VK_OK != ret)
	{
		VK_printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
		return;
	}

	eventToTask2 = 1;
}

void thread_vk_task_send_receive2(void *arg)
{
	int ret;
	int i;

	eventToTask2 = 0;
	while (eventToTask2 == 0) VK_TASK_Sleep(10);

	PrintDebug("msg_data_send[2] = {");
	for(i=0; i<Q_SIZE; i++)
	{
		PrintDebug("0x%x, ", msg_data_send[2][i]);
	}
	PrintDebug("}\n");
	ret = VK_MSG_Send(msgQid, msg_data_send[2], Q_SIZE);
	if(VK_OK != ret)
	{
		VK_printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
		return;
	}

	eventToTask1 = 1;

	eventToTask2 = 0;
	while (eventToTask2 == 0) VK_TASK_Sleep(10);

	PrintDebug("msg_data_send[3] = {");
	for(i=0; i<Q_SIZE; i++)
	{
		PrintDebug("0x%x, ", msg_data_send[3][i]);
	}
	PrintDebug("}\n");
	ret = VK_MSG_Send(msgQid, msg_data_send[3], Q_SIZE);
	if(VK_OK != ret)
	{
		VK_printf("%s(%d) : VK_MSG_Send Error\n", __func__, __LINE__);
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
			VK_printf("%s(%d) : VK_MSG_Receive Error\n", __func__, __LINE__);
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
			VK_printf("msg_data_send[0][%d] = 0x%x\n", j, msg_data_send[0][j]);
			VK_printf("msg_data_receive[0][%d] = 0x%x\n", j, msg_data_receive[0][j]);
			msg_test_passed = FALSE;
			return;
		}
	}

	for(j=0; j<Q_SIZE; j++)
	{
		if(msg_data_send[2][j] != msg_data_receive[1][j])
		{
			VK_printf("msg_data_send[0][%d] = 0x%x\n", j, msg_data_send[2][j]);
			VK_printf("msg_data_receive[0][%d] = 0x%x\n", j, msg_data_receive[1][j]);
			msg_test_passed = FALSE;
			return;
		}
	}

	for(j=0; j<Q_SIZE; j++)
	{
		if(msg_data_send[1][j] != msg_data_receive[2][j])
		{
			VK_printf("msg_data_send[0][%d] = 0x%x\n", j, msg_data_send[1][j]);
			VK_printf("msg_data_receive[0][%d] = 0x%x\n", j, msg_data_receive[2][j]);
			msg_test_passed = FALSE;
			return;
		}
	}

	for(j=0; j<Q_SIZE; j++)
	{
		if(msg_data_send[3][j] != msg_data_receive[3][j])
		{
			VK_printf("msg_data_send[0][%d] = 0x%x\n", j, msg_data_send[3][j]);
			VK_printf("msg_data_receive[0][%d] = 0x%x\n", j, msg_data_receive[3][j]);
			msg_test_passed = FALSE;
			return;
		}
	}


	msg_test_passed = TRUE;
}

static UT_RESULT ut_vk_msg_send_receive(void)
{
	int ret;

	msg_test_passed = FALSE;

	ret = VK_MSG_Create(Q_COUNT, Q_SIZE, "ut_vk_msg_send_receive", &msgQid, VK_SUSPENDTYPE_FIFO);
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

	VK_TASK_Sleep(500);

	eventToTask1 = 1;

	VK_TASK_Sleep(1000);

	if(TRUE == msg_test_passed)
	{
		return UT_OK;
	}

__error_return_label:
	return UT_FAIL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// VK RPC Test
//
#define UT_RPC_PORT    61001

static unsigned long ulServerTaskId;
static unsigned long ulClientTaskId;
static HUINT32 hServerSock;
static HUINT32 hClientSock;
static HUINT32 hServerRpc;
static HUINT32 hClientRpc;

/*---------------------------------------------------------------------------------------
** server side
**-------------------------------------------------------------------------------------*/
static HINT32 ut_vk_rpc_testfn(HUINT32 nSockClient, HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserdata)
{
	int i;
	VKRPC_OBJECT_T *obj;
	HUINT32 hRpc = (HUINT32)pvUserdata;

	VK_printf("ut_vk_rpc_testfn called......\n");
	for(i=0;i<nArgc;i++) {
		obj = apArgv[i];
		VK_printf("[%s] obj type: %c\n", __FUNCTION__, obj->type);
		switch(obj->type) {
		case VkOBJECT_TYPE_INT:
			VK_printf("[%s] obj val: 0x%x\n", __FUNCTION__, obj->u.i.num);
			break;
		case VkOBJECT_TYPE_INT64:
			VK_printf("[%s] obj val: 0x%llx\n", __FUNCTION__, obj->u.l.num64);
			break;
		case VkOBJECT_TYPE_STR:
			VK_printf("[%s] obj val: %s\n", __FUNCTION__, obj->u.s.str);
			break;
		case VkOBJECT_TYPE_BIN:
			VK_printf("[%s] obj val: size(%d)\n", __FUNCTION__, obj->u.b.size);
			VK_printf("[%s] obj val: data(0x%x)\n", __FUNCTION__, (HUINT32)obj->u.b.data);
			break;
		default:
			break;
		}
	}

	VK_printf("ut_vk_rpc_testfn replied......\n");
	VK_RPC_Response(hRpc, nSockClient, nRequestId, "ut.rpc.test", "is", 0, "this is server's response");

	return 0;
}

static void ut_vk_rpc_p_server_receive_pkt(void *userdata, HUINT32 sock, HUINT32 req_id, void *pvData, HUINT32 size)
{
	VK_RPC_ProcessRpc((HUINT32)userdata, req_id, pvData, size, sock);
}

static void ut_vk_rpc_p_server_thread(void *pParam)
{
	(void)pParam;
	
	while(1) {
		VK_SOCKET_Listen(hServerSock, 0xFFFFFFFE, ut_vk_rpc_p_server_receive_pkt, (void*)hServerRpc);		
	}
}

/*---------------------------------------------------------------------------------------
** client side
**-------------------------------------------------------------------------------------*/
static HINT32 ut_vk_rpc_client_testfn(HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserData)
{
	int i;
	VKRPC_OBJECT_T *obj;
	int *ret = (int*)pvUserData;

	(void)nRequestId;

	VK_printf("ut_vk_rpc_client_testfn called......\n");
	for(i=0;i<nArgc;i++) {
		obj = apArgv[i];
		VK_printf("[%s] obj type: %c\n", __FUNCTION__, obj->type);
		switch(obj->type) {
		case VkOBJECT_TYPE_INT:
			VK_printf("[%s] obj val: 0x%x\n", __FUNCTION__, obj->u.i.num);
			break;
		case VkOBJECT_TYPE_INT64:
			VK_printf("[%s] obj val: 0x%llx\n", __FUNCTION__, obj->u.l.num64);
			break;
		case VkOBJECT_TYPE_STR:
			VK_printf("[%s] obj val: %s\n", __FUNCTION__, obj->u.s.str);
			break;
		case VkOBJECT_TYPE_BIN:
			VK_printf("[%s] obj val: size(%d)\n", __FUNCTION__, obj->u.b.size);
			VK_printf("[%s] obj val: data(0x%x)\n", __FUNCTION__, (HUINT32)obj->u.b.data);
			break;
		default:
			break;
		}
	}
	*ret = UT_OK;
	return 0;
}

static void ut_vk_rpc_p_client_receive_pkt(void *userdata, HUINT32 sock, HUINT32 req_id, void *pvData, HUINT32 size)
{
	VK_RPC_ProcessRpc((HUINT32)userdata, req_id, pvData, size, sock);
}

static void ut_vk_rpc_p_client_thread(void *pParam)
{
	(void)pParam;
	
	while(1) {
		VK_SOCKET_Listen(hClientSock, 0xFFFFFFFE, ut_vk_rpc_p_client_receive_pkt, (void*)hClientRpc);		
	}
}

static HINT32 ut_vk_rpc_p_client_eventcb(HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserData)
{
	int i;
	VKRPC_OBJECT_T *obj;
	int *ret = (int*)pvUserData;

	(void)nArgc;
	(void)apArgv;
	(void)nRequestId;

	VK_printf("ut_vk_rpc_p_client_eventcb called......\n");
	for(i=0;i<nArgc;i++) {
		obj = apArgv[i];
		VK_printf("[%s] obj type: %c\n", __FUNCTION__, obj->type);
		switch(obj->type) {
		case VkOBJECT_TYPE_INT:
			VK_printf("[%s] obj val: 0x%x\n", __FUNCTION__, obj->u.i.num);
			break;
		case VkOBJECT_TYPE_INT64:
			VK_printf("[%s] obj val: 0x%llx\n", __FUNCTION__, obj->u.l.num64);
			break;
		case VkOBJECT_TYPE_STR:
			VK_printf("[%s] obj val: %s\n", __FUNCTION__, obj->u.s.str);
			break;
		case VkOBJECT_TYPE_BIN:
			VK_printf("[%s] obj val: size(%d)\n", __FUNCTION__, obj->u.b.size);
			VK_printf("[%s] obj val: data(0x%x)\n", __FUNCTION__, (HUINT32)obj->u.b.data);
			break;
		default:
			break;
		}
	}
	*ret = UT_OK;
	return 0;
}

static UT_RESULT ut_vk_rpc_test(void)
{
	int *ret;
	UT_RESULT ur;
	HUINT32 ut_rpc_port = 61001;
	HUINT8 data[] = {1,2,3,4};

	ret = VK_malloc(4);
	if (!ret)
		return UT_FAIL;
	
	VK_SOCKET_SERVER_Open(VK_SOCKET_TYPE_INET, &ut_rpc_port, &hServerSock);
	VK_RPC_Open(hServerSock, 1001, &hServerRpc);
	VK_RPC_RegisterCall(hServerRpc, "ut.rpc.test", "ilsb", ut_vk_rpc_testfn, (void*)hServerRpc);

	VK_SOCKET_CLIENT_Open(VK_SOCKET_TYPE_INET, NULL, &ut_rpc_port, 10, &hClientSock);
	VK_RPC_Open(hClientSock, 1002, &hClientRpc);
	VK_RPC_RegisterNotifier(hClientRpc, "notifier", (void*)ret, ut_vk_rpc_p_client_eventcb);
	
	VK_TASK_Create(ut_vk_rpc_p_server_thread,
		           USER_PRIORITY0,
	               TIMER_TASK_STACK_SIZE,
		           "rpcserver",
		           NULL,
		           &ulServerTaskId,
		           VK_SUSPENDTYPE_FIFO);
	VK_TASK_Create(ut_vk_rpc_p_client_thread,
		           USER_PRIORITY0,
	               TIMER_TASK_STACK_SIZE,
		           "rpcserver",
		           NULL,
		           &ulClientTaskId,
		           VK_SUSPENDTYPE_FIFO);
	VK_TASK_Start(ulServerTaskId);
	VK_TASK_Start(ulClientTaskId);

	VK_TASK_Sleep(2000);

	/* Request Test */
	*ret = UT_FAIL;
	VK_RPC_Request(hClientRpc, ut_vk_rpc_client_testfn, 10*1000, (void*)ret, "ut.rpc.test", "ilsb", 
		           0xcafebebe,
		           0xcafebebecafebebe,
		           "hello, i'm client",
		           data,
		           4);
	ur = *ret;
	if (ur != UT_OK)
		goto done;

	/* Notify Test */
	*ret = UT_FAIL;
	VK_RPC_Notify(hServerRpc, "notifier", "s", "Broadcasting EVENT");
	VK_TASK_Sleep(2000);
	ur = *ret;

done:
	VK_free(ret);
	VK_RPC_Close(hServerRpc);
	VK_RPC_Close(hClientRpc);
	VK_SOCKET_Close(hClientSock);
	VK_SOCKET_Close(hServerSock);
	VK_TASK_Destroy(ulServerTaskId);
	VK_TASK_Destroy(ulClientTaskId);
	
	return ur;
}


static UT_MENU_ITEM_T	menu_sema[] =
{
	MENU_START,
	MENU_FUNC(ut_vk_sem_basic, 10),
	MENU_FUNC(ut_vk_sem_count, 10),
	MENU_FUNC(ut_vk_sem_create_destroy, 100),
	MENU_FUNC(ut_vk_sem_timeout, 10),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_msg[] =
{
	MENU_START,
	MENU_FUNC(ut_vk_msg_send_receive, 3),
	MENU_FUNC(ut_vk_msg_timeout, 3),
	MENU_FUNC(ut_vk_msg_send_timeout, 10),
	MENU_FUNC(ut_vk_msg_send_urgent_timeout, 10),
	MENU_FUNC(ut_vk_msg_receive_timeout, 60),
	MENU_FUNC(ut_vk_msg_create_destroy, 1),
	MENU_FUNC(ut_vk_msg_priority, 1),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_task[] =
{
	MENU_START,
	MENU_FUNC(ut_vk_task_basic, 5),
	MENU_FUNC(ut_vk_task_sleep, 10),
	MENU_FUNC(ut_vk_task_argument, 10),
	MENU_FUNC(ut_vk_task_create_destroy, 100),
	MENU_FUNC(ut_vk_task_priority, 10),
	MENU_FUNC(ut_vk_task_advance_sleep, 200),
	MENU_FUNC(ut_vk_task_join, 10),
	MENU_FUNC(ut_vk_task_getcurrentid, 1),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_timer[] =
{
	MENU_START,
	MENU_FUNC(ut_vk_time_diff, 10),
	MENU_FUNC(ut_vk_timer_event_after, 10),
	MENU_FUNC(ut_vk_timer_event_every, 10),
	MENU_FUNC(ut_vk_timer_event_cancel, 10),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_clock[] =
{
	MENU_START,
	MENU_FUNC(ut_vk_clock_time, 10),
	MENU_FUNC(ut_vk_clock_offset_time, 10),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_rpc[] =
{
	MENU_START,
	MENU_FUNC(ut_vk_rpc_test, 10),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_vernel[] =
{
	MENU_START,
	MENU_SUB(menu_sema),
	MENU_SUB(menu_msg),
	MENU_SUB(menu_task),
	MENU_SUB(menu_timer),
	MENU_SUB(menu_clock),
	MENU_SUB(menu_rpc),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_vk[] =
{
	MENU_SUB(menu_vernel),
};


void UT_VKERNEL_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_vk)/sizeof(menu_main_vk[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_vk[i]);
	}
	array_size = sizeof(menu_sema)/sizeof(menu_sema[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_sema[i]);
	}
	array_size = sizeof(menu_msg)/sizeof(menu_msg[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_msg[i]);
	}
	array_size = sizeof(menu_task)/sizeof(menu_task[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_task[i]);
	}
	array_size = sizeof(menu_timer)/sizeof(menu_timer[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_timer[i]);
	}
	array_size = sizeof(menu_clock)/sizeof(menu_clock[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_clock[i]);
	}
	array_size = sizeof(menu_rpc)/sizeof(menu_rpc[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_rpc[i]);
	}
	array_size = sizeof(menu_vernel)/sizeof(menu_vernel[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_vernel[i]);
	}
}



