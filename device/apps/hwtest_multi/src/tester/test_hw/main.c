/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:			main.c
// Original Author: 	Kim Jin-Hong
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include vkernel */
#include "vkernel.h"

/* include drv */

/* include di */
#include "di_err.h"

/* include linden trace */
#include "linden_trace.h"

//#include "b_os_lib.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define _FOR_HW_TEST_
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


void MAIN_SetTraceModules(void);



int main(void) {

	DI_ERR_CODE	eDiError	= DI_ERR_OK;

#if 0
	B_Error		eBError = 0;

	/* Initialize OS */
	eBError = B_Os_Init();
	if(eBError != DI_ERR_OK)
	{
		TraceError(TRACE_MODULE_MON, "%s->B_Os_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eBError, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}
	else
	{
		TraceInfo(TRACE_MODULE_MON,"%s->B_Os_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
    /* Initialize Vkernel */
	eDiError = VK_Init();
	if (DI_ERR_OK != eDiError)
    {
		TraceError(TRACE_MODULE_MON, "%s->VK_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDiError, __LINE__, __FILE__);
        return VK_ERROR;
    }
	TraceInfo(TRACE_MODULE_MON,"%s->VK_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

	/* Initialize Trace */
	TraceInit();
	TraceInfo(TRACE_MODULE_MON,"%s->TraceInit(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

	MAIN_SetTraceModules();
	TraceInfo(TRACE_MODULE_MON,"%s->MAIN_SetTraceModules(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

	/* Initialize Monitor */
	MON_Init();
	TraceInfo(TRACE_MODULE_MON,"%s->MON_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

#if 0
	/* Initlialize DI */
	eDiError = DI_Init();
	if(eDiError != DI_ERR_OK)
	{
		TraceError(TRACE_MODULE_MON, "%s->DI_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDiError, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}
	else
	{
		TraceInfo(TRACE_MODULE_MON,"%s->DI_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}


	/* Initialize MW */


	/* Initialize App */



#ifdef _FOR_HW_TEST_
	/* Test tuner 1, ASIA SAT2 */
	TEST_TUNER_Tune(0, 0, 1150, 28125, 0, 0);/* */

	/* Test video LUXE HD */
	TEST_VIDEO_PlayStream(370, 370, 5, 372, 1);

	/* Record ASIA SAT2, 6 Programs */
	//TEST_PVR_StartRecord();

	/* Set Fan to maximum speed */
	TEST_FAN_SetMaxSpeed();

#endif

    //TEST_VK();    //vk test
    //TEST_GPIO();  //gpio test
	//TEST_EEPROM(); 	//EEPROM Test
	//TEST_VK_MEM();	    //MEM_test



	TEST_FAN_SetMaxSpeed();
#endif


	while(1)
	{
		HUINT32 i;
/*		VK_TASK_Sleep(1000*3600);*//* 1 hour sleep */
		VK_TASK_Sleep(1000*600);/* 10 min sleep */
		/* hw 신뢰성 테스트용 1시간에 한번씩 저장하고 있는 파일들을 지운다.  */
//		remove("/mnt/hd/videos/*");
		printf("remove recorded files!!\n");
	}


}

void MAIN_SetTraceModules(void)
{

/* VK Module */
	TraceSetModule( TRACE_MODULE_VK);
	TraceSetLevel( TRACE_MODULE_VK,	TRACE_LEVEL_ERROR );

	TraceSetModule( TRACE_MODULE_VK_CLOCK);
	TraceSetLevel( TRACE_MODULE_VK_CLOCK, TRACE_LEVEL_ERROR );


	TraceSetModule( TRACE_MODULE_VK_EVENT);
	TraceSetLevel( TRACE_MODULE_VK_EVENT, TRACE_LEVEL_ERROR );


	TraceSetModule( TRACE_MODULE_VK_MEM);
	TraceSetLevel( TRACE_MODULE_VK_MEM, TRACE_LEVEL_ERROR );


	TraceSetModule( TRACE_MODULE_VK_MSG);
	TraceSetLevel( TRACE_MODULE_VK_MSG, TRACE_LEVEL_ERROR );


	TraceSetModule( TRACE_MODULE_VK_SEM);
	TraceSetLevel( TRACE_MODULE_VK_SEM, TRACE_LEVEL_ERROR );

	TraceSetModule( TRACE_MODULE_VK_TASK);
	TraceSetLevel( TRACE_MODULE_VK_TASK, TRACE_LEVEL_ERROR );

	TraceSetModule( TRACE_MODULE_VK_TIMER);
	TraceSetLevel( TRACE_MODULE_VK_TIMER, TRACE_LEVEL_ERROR );


/* DI Module	*/
	TraceSetModule( TRACE_MODULE_DI_INIT);
	TraceSetLevel( TRACE_MODULE_DI_INIT,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_AUDIO);
	TraceSetLevel( TRACE_MODULE_DI_AUDIO,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_CHANNEL);
	TraceSetLevel( TRACE_MODULE_DI_CHANNEL,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_CI);
	TraceSetLevel( TRACE_MODULE_DI_CI,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_DEMUX);
	TraceSetLevel( TRACE_MODULE_DI_DEMUX,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_EEPROM);
	TraceSetLevel( TRACE_MODULE_DI_EEPROM,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_FAN);
	TraceSetLevel( TRACE_MODULE_DI_FAN,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_FLASH);
	TraceSetLevel( TRACE_MODULE_DI_FLASH,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_HDMI);
	TraceSetLevel( TRACE_MODULE_DI_HDMI,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_KEY);
	TraceSetLevel( TRACE_MODULE_DI_KEY,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_LED);
	TraceSetLevel( TRACE_MODULE_DI_LED,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_OSD);
	TraceSetLevel( TRACE_MODULE_DI_OSD,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_PVR);
	TraceSetLevel( TRACE_MODULE_DI_PVR,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_SCART);
	TraceSetLevel( TRACE_MODULE_DI_SCART,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_SC);
	TraceSetLevel( TRACE_MODULE_DI_SC,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_VIDEO);
	TraceSetLevel( TRACE_MODULE_DI_VIDEO,	TRACE_LEVEL_ERROR);


/* DRV Module	*/
	TraceSetModule( TRACE_MODULE_DRV_GPIO);
	TraceSetLevel( TRACE_MODULE_DRV_GPIO,	TRACE_LEVEL_ERROR);


	TraceSetModule( TRACE_MODULE_DRV_GPIO);
	TraceSetLevel( TRACE_MODULE_DRV_GPIO,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_HDMI);
	TraceSetLevel( TRACE_MODULE_DRV_HDMI,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_I2C);
	TraceSetLevel( TRACE_MODULE_DRV_I2C,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_KEYPAD);
	TraceSetLevel( TRACE_MODULE_DRV_KEYPAD,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_LNB);
	TraceSetLevel( TRACE_MODULE_DRV_LNB,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_MICOM);
	TraceSetLevel( TRACE_MODULE_DRV_MICOM,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_PWM);
	TraceSetLevel( TRACE_MODULE_DRV_PWM,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_REMOCON);
	TraceSetLevel( TRACE_MODULE_DRV_REMOCON,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_SCART);
	TraceSetLevel( TRACE_MODULE_DRV_SCART,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_UART);
	TraceSetLevel( TRACE_MODULE_DRV_UART,	TRACE_LEVEL_ERROR);

/* MON Module - always info */
	TraceSetModule( TRACE_MODULE_MON);
	TraceSetLevel( TRACE_MODULE_MON,	TRACE_LEVEL_INFO);



}

