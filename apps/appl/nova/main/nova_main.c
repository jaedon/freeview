/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nova_int.h>
#include "tuner_api.h"
#include "nova_cas.h"

/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/
void nova_main_init(void)
{
	(void)DI_MI_Init();
	NOVA_ServerInit();
	NOVA_MSG_Init();
	NOVA_RPC_Init();
	NOVA_SYS_Init();

#if defined(CONFIG_DEBUG)
	NOVA_CMD_Init();
#endif

	TUNERAPI_Init();

	NOVA_CAS_Init();
}

void nova_main_deinit(void)
{
	TUNERAPI_DeInit();
	NOVA_ServerDeinit();
}

/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/

//#define TUNER_API_TEST

#ifndef TUNER_API_TEST

int main(void)
{
	VK_Init();

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("nova");

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif

	nova_main_init();

	while(1)
	{
		VK_TASK_Sleep(100000);
	}

	nova_main_deinit();
}

#else

#include "nova_test.h"

int g_signalCtrlC = 0;

// function
void signal_int_handler(int sig_num)
{
	char c;

	signal(sig_num, SIG_IGN);
	printf("Do you really want to quit?[y/n]");

	c = getchar();
	if (c == 'y' || c == 'Y')
	{
		g_signalCtrlC = 1;
	}
	else
	{
		signal(SIGINT, signal_int_handler);
	}
}

int main(void)
{
	VK_Init();
	HLIB_EXTRA_DropRootPrivileges("nova");

	signal(SIGINT, signal_int_handler);

	TEST_Init();

	TEST_Do();

	while (g_signalCtrlC == 0)
	{
		HLIB_STD_TaskSleep(100);
	}

	TEST_DeInit();
}

#endif
