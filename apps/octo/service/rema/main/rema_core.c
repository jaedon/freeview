/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

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

#define ___HEADER_FILES___

#include <time.h>
#include <vkernel.h>
#include <hlib.h>
#include <napi.h>
#include <hapi.h>

#include <rema_int.h>

#include "rema_core.h"
#include "rema_cwmp.h"
#include "rema_cmd.h"
#include "rema_MOD.h"

#define ___DEFINES___
// After middleware is started, video displaying needs about 60sec in YSR-2000
#define WAIT_TIME_CNT 	60	

#define STANDARD_YEAR	2005
#define STANDARD_MONTH	1
#define	STANDARD_MDAY	1

#define SYSTEM_CMD_MAX	512


#define ___MACROS___

#define ___TYPEDEF___

#define ___STATIC_VARIABLES___
/*
#include "homma_bus.h"
#include "homma_server.h"
#include "homma_system.h"
*/
#define ___INTERNAL_FUNCTIONS___
#define ___PUBLIC_FUNCTIONS___


STATIC HERROR rema_core_Remove_DB(void)
{
	HERROR hRet = ERR_OK;
	HINT32 vResult = 0;

	char cSysCmd[SYSTEM_CMD_MAX];

	VK_MEM_Memset(cSysCmd, 0x0, SYSTEM_CMD_MAX);
	VK_snprintf(cSysCmd, sizeof(cSysCmd), "%s", "rm -f /var/lib/humaxtv/tr069/*");

	vResult = VK_SYSTEM_Command((char *)cSysCmd);
	if(vResult != VK_OK)
	{
		RemLOG_Error("VK_SYSTEM_Command(%s) failed!\n", cSysCmd);
		return ERR_FAIL;
	}

	return hRet;
}

STATIC HERROR rema_core_onFactoryDefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HERROR hRet = ERR_OK;
	hRet = HAPI_RemoveRegistry();
	hRet = rema_core_Remove_DB();

	return hRet;
}

HBOOL rema_core_Init(void)
{
	REM_FUNC_ENTER;

	rema_module_Init();

	TR181_NAPI_Init(); // When NAP is registed, It' not problem that receiving NAPI events. 

	rema_MOD_Service_Init();

    //UAPI_Init();
    //UAPI_FS_Init();
	//UAPI_Init();

	HAPI_ConnectSignal("signal:onFactoryDefault", NULL, rema_core_onFactoryDefault);
	
	RemLOG_Info("Process Start!!!!\n");

	REM_FUNC_LEAVE;

	return TRUE;
}

void rema_core_Deinit(void)
{
	REM_FUNC_ENTER;

//	rema_CMD_Deinit();
//	rema_module_Deinit();

	REM_FUNC_LEAVE;
}

void rema_core_cpe_open(Rema_t *rema)
{
	HUINT32 tryCnt = 0;
	HUINT32 time = 0;
	
	time_t standTime;
	struct tm stStandardTime;

	stStandardTime.tm_year = STANDARD_YEAR-1900;
	stStandardTime.tm_mon = STANDARD_MONTH-1;
	stStandardTime.tm_mday = STANDARD_MDAY;
	stStandardTime.tm_hour = 0;	   
	stStandardTime.tm_min = 0;
	stStandardTime.tm_sec = 0;
	stStandardTime.tm_isdst = -1;			// Is DST (Summer Time ) on? 1 = yes, 0 = no, -1 = unknown
	standTime = mktime(&stStandardTime);

	RemLOG_Debug("standTime is %ld \n", __FILE__, __FUNCTION__, __LINE__, (long)standTime);
		
	if(TRUE != rema->bCwmpStbOpen)
	{
		while(TRUE)
		{
			time = HLIB_STD_GetSystemTime();
			if( time > standTime )
			{
				RemLOG_Info("Getting time(%d) is successful!!!\n", time);
				break;
			}

			if(tryCnt > WAIT_TIME_CNT)
			{
				RemLOG_Warning("Can not get time !!!\n");
				break;
			}

			tryCnt++;

			VK_TASK_Sleep(1000);
			RemLOG_Info("Wait for time to be set...\n");
		}
	
		Cwmp_Cpe_open(NULL);
		rema->bCwmpStbOpen = TRUE;
	}	
}

void rema_core_cpe_close(Rema_t *rema)
{
	if(rema->bCwmpStbOpen == TRUE){
		rema->bCwmpStbOpen = FALSE;
		Cwmp_Cpe_close();
	}
}

#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

