/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ??2011-2012 Humax Co., Ltd.
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

#include <vkernel.h>
#include <hlib.h>

#include <rema_int.h>
#include	<hapi.h>

#include "rema_cmd.h"
#include "rema_cwmp.h"

#define ___DEFINES___


#define DMAX_ARG					1024
#define DMAX_PARAMETER_NAME			1024
#define DMAX_PARAMETER_VALUE		1024
#define ___MACROS___

#define ___TYPEDEF___
#define     hCmdHandle      "cwmp"

#define ___INTERNAL_FUNCTIONS_DEFINED__

#define hCwmpResult_e HINT32
#define ehCwmp_R_OK HxCMD_OK
#define hCwmpParamType_e HINT32

static hCwmpResult_e   cmd_print_GPN(void *pCtx, const char *pszName, const HBOOL bWritable);
static hCwmpResult_e cmd_print_GPV(void *pCtx, const char *pszName, hCwmpParamType_e eType, const char *pszValue);

#define ___STATIC_VARIABLES___

#define ___INTERNAL_FUNCTIONS___

static hCwmpResult_e cmd_print_GPN(void *pCtx, const char *pszName, const HBOOL bWritable)
{
    RemLOG_Print("name:(%s) wriable:(%s)\n",pszName, (bWritable)? "true":"false" );
    return ehCwmp_R_OK;
}
static hCwmpResult_e cmd_print_GPV(void *pCtx, const char *pszName, hCwmpParamType_e eType, const char *pszValue)
{
    RemLOG_Print("name:(%s) value:(%s)\n",pszName, pszValue );
    return ehCwmp_R_OK;
}

static HINT32   cmd_cwmp_do_action(void *szArgList)
{
    HCHAR    *aucArg=NULL, aucStr[20];

    REM_FUNC_ENTER;

    aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
    if( aucArg == NULL ) 	return HxCMD_ERR;
    if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

    HLIB_STD_StrUpper(aucStr);

    if (VK_strcmp(aucStr, "START") == 0)
    {


	HLIB_CMD_Printf("cwmp start\n");
//		rema_cwmp_Start();
    }
    else if(VK_strcmp(aucStr, "STOP") == 0)
    {
	HLIB_CMD_Printf("cwmp stop\n");
	//	rema_cwmp_Stop();
    }
    else if(VK_strcmp(aucStr, "INFO") == 0)
    {

    }
    else
    {

    }

    REM_FUNC_LEAVE;

    return HxCMD_OK;
}

static HINT32   cmd_cwmp_GPN(void *szArgList)
{
    HCHAR   *pszAucArg=NULL;
    HCHAR   szParamPath[DMAX_ARG];
    HCHAR   szNextLevel[20];
    HBOOL   bNextLevel=FALSE;


    REM_FUNC_ENTER;

    pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
    if( pszAucArg == NULL )
    {
	RemLOG_Debug("pszAucArg is null");
	return HxCMD_ERR;
    }
    if( sscanf(pszAucArg, "%s", (char *)&szParamPath) == 0 )
    {
	RemLOG_Debug("pszAucArg sscanf error");
	return HxCMD_ERR;
    }
    //HLIB_STD_StrUpper(szAucStr1);

    pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
    if( pszAucArg == NULL )
    {
	RemLOG_Debug("pszAucArg #2 is null");
	return HxCMD_ERR;
    }
    if( sscanf(pszAucArg, "%s", (char *)&szNextLevel) == 0 )
    {
	RemLOG_Debug("pszAucArg sscanf error#2");
	return HxCMD_ERR;
    }
    //HLIB_STD_StrUpper(szAucStr2);

    RemLOG_Debug("szParamPath(%s) , szNextLevel(%s)", szParamPath, szNextLevel);

    HLIB_STD_StrUpper(szNextLevel);
    if(!HxSTD_StrCmp(szNextLevel, "TRUE"))
    {
	bNextLevel = TRUE;
    }
    else if(!HxSTD_StrCmp(szNextLevel, "FALSE"))
    {
	bNextLevel = FALSE;
    }
    else if(!HxSTD_StrCmp(szNextLevel, "1"))
    {
	bNextLevel = TRUE;
    }
    else if(!HxSTD_StrCmp(szNextLevel, "0"))
    {
	bNextLevel = FALSE;
    }
    else
    {
	RemLOG_Debug(" %s is error", szNextLevel);
	return HxCMD_ERR;
    }
    RemLOG_Print("=================start=======================\n");
//    rema_cwmp_DMAcessGPN(szParamPath, bNextLevel, cmd_print_GPN);
    RemLOG_Print("=================end=======================\n");

    REM_FUNC_LEAVE;

    return HxCMD_OK;
}

static HINT32   cmd_cwmp_GPV(void *szArgList)
{
    HCHAR    *pszAucArg=NULL;
    HCHAR    szParamPath[DMAX_ARG];

    REM_FUNC_ENTER;

    pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
    if( pszAucArg == NULL )
	return HxCMD_ERR;
    if( sscanf(pszAucArg, "%s", (char *)&szParamPath) == 0 )
	return HxCMD_ERR;

    RemLOG_Debug("szParamPath(%s)\n", szParamPath);
    RemLOG_Print("=================start=======================\n");
//    rema_cwmp_DMAcessGPV(szParamPath, cmd_print_GPV);
    RemLOG_Print("=================end=======================\n");
    REM_FUNC_LEAVE;

    return HxCMD_OK;
}

static HINT32   cmd_cwmp_SPV(void *szArgList)
{

    HCHAR    *pszAucArg=NULL;
    HCHAR    szParamName[DMAX_ARG];
    HCHAR    szValue[DMAX_ARG];
    HCHAR    szType[DMAX_ARG];

    REM_FUNC_ENTER;

    pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
    if( pszAucArg == NULL ) 	return HxCMD_ERR;
    if( sscanf(pszAucArg, "%s", (char *)&szParamName) == 0 )   	return HxCMD_ERR;

    pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
    if( pszAucArg == NULL ) 	return HxCMD_ERR;
    if( sscanf(pszAucArg, "%s", (char *)&szValue) == 0 )   	return HxCMD_ERR;

    pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
    if( pszAucArg == NULL ) 	return HxCMD_ERR;
    if( sscanf(pszAucArg, "%s", (char *)&szType) == 0 )   	return HxCMD_ERR;


    RemLOG_Debug("szParamName(%s), szType(%s), szValue(%s)\n", szParamName, szType, szValue);

    RemLOG_Print("=================start=======================\n");
//    rema_cwmp_DMAcessSPV(szParamName, szType, szValue);
    RemLOG_Print("=================end=======================\n");

    RemLOG_Print("================TDB=================\n");

    REM_FUNC_LEAVE;

    return HxCMD_OK;
}

static HINT32   cmd_cwmp_AddObject(void *szArgList)
{
    HCHAR    *pszAucArg=NULL;
    HCHAR    szObjectName[DMAX_ARG];
    HUINT32   unAddedInx=-1;

    REM_FUNC_ENTER;

    pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
    if( pszAucArg == NULL )
	return HxCMD_ERR;
    if( sscanf(pszAucArg, "%s", (char *)&szObjectName) == 0 )
	return HxCMD_ERR;



    RemLOG_Debug("ObjectName(%s)\n", szObjectName);
//    rema_cwmp_DMAcessAOB(szObjectName, &unAddedInx);
    RemLOG_Print("ok ObjectName(%s) Index(%u)\n", szObjectName, unAddedInx);

    REM_FUNC_LEAVE;

    return HxCMD_OK;
}

static HINT32   cmd_cwmp_DeleteObject(void *szArgList)
{
    HCHAR    *pszAucArg=NULL;
    HCHAR    szObjectName[DMAX_ARG];

/*
    REM_FUNC_ENTER;

    pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
    if( pszAucArg == NULL )
	return HxCMD_ERR;
    if( sscanf(pszAucArg, "%s", (char *)&szObjectName) == 0 )
	return HxCMD_ERR;



    RemLOG_Debug("ObjectName(%s)\n", szObjectName);

    if(rema_cwmp_DMAcessDOB(szObjectName) ==  0)
    {
	RemLOG_Print("ok delete ObjectName(%s)\n", szObjectName);
    }
    else
    {
	RemLOG_Print("failed\n");
    }

    REM_FUNC_LEAVE;
*/
    return HxCMD_OK;
}

#define ___PUBLIC_FUNCTIONS___

void RemaSendMsgCmd(REMA_TR069MSG_e eMsg)
 {
	 Rema_t* rema = rema_GetInstance();
	 REMA_MSG_t 	 msg;
	 RemLOG_Print("================= RemaSendMsgCmd start=======================\n");

#define RemaCmdDefaultWatingTime 3000;
	 HxSTD_MemSet(&msg, 0, sizeof(REMA_MSG_t));

	 msg.eMsg = eMsg;
	 msg.pszProcessName = HLIB_STD_StrDup(HLIB_STD_GetCurProcessName());
	 msg.ulParam1 =   RemaCmdDefaultWatingTime;
	 RemLOG_Print("================= msg(%d) prcess (%s)=======================\n",msg.eMsg,msg.pszProcessName);

	 HLIB_MSG_Send(rema->ulMainMsgQueue, (void*)&msg, sizeof(REMA_MSG_t));
#undef 	RemaCmdDefaultWatingTime
	 rema_ReleaseInstance(rema);
 RemLOG_Print("================= RemaSendMsgCmd end =======================\n");

 }


static HINT32 cmd_cwmp_cpe_cmd(void *szArgList)
{

	HCHAR	 *pszAucArg=NULL;
	HCHAR	 szParamName[DMAX_ARG];
	REMA_TR069MSG_e eRemaMsg = eREMA_TR069_UNKNOWN;

	REM_FUNC_ENTER;

	RemLOG_Print("\n================= TEST cmd_cwmp_print =======================\n");
   /********************
     * get 1st argument *
     ********************/

	pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);

	if( pszAucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(pszAucArg, "%s", (char *)&szParamName) == 0 )	return HxCMD_ERR;

	RemLOG_Print("cpe : (%s)\n", szParamName);

	/********************
	  * Do Action*
	  ********************/
	RemLOG_Print("=================start=======================\n");

	if(HxSTD_StrCmp(szParamName, "open") == 0)
	{
		eRemaMsg = eREMA_TR069CwmpCpeOpen;
	}
	else if(HxSTD_StrCmp(szParamName, "close") == 0){
		eRemaMsg = eREMA_TR069CwmpCpeClose;
	}

	if(	eRemaMsg != eREMA_TR069_UNKNOWN)
	{
		RemaSendMsgCmd(eRemaMsg);
	}

	RemLOG_Print("=================end=======================\n");

	REM_FUNC_LEAVE;

	return HxCMD_OK;
}

static HINT32 cmd_wifi_cmd(void *szArgList)
{

	HCHAR	 *pszAucArg=NULL;
	HCHAR	 szParamName[DMAX_ARG] = {0, };
	REMA_TR069MSG_e eRemaMsg = eREMA_TR069_UNKNOWN;

	char output[1][256] = {0, };

	REM_FUNC_ENTER;

	RemLOG_Print("\n================= TEST cmd_cwmp_print =======================\n");
   /********************
     * get 1st argument *
     ********************/

	pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( pszAucArg == NULL ) 	return HxCMD_ERR;

	if( sscanf(pszAucArg, "%s", (char *)&szParamName) == 0 )	return HxCMD_ERR;

	if(szParamName)
	{
		RemLOG_Print("Wifi param1 : (%s)\n", szParamName);

		if(HxSTD_StrCmp(szParamName, "SSIDNumberOfEntries") == 0)
		{
			RemLOG_Print("Wifi.SSIDNumberOfEntries\n");
			Cwmp_cmd_Stb_getParam("Device.WiFi.SSIDNumberOfEntries",output, 1);
		}
		else if(HxSTD_StrCmp(szParamName, "SSID") == 0)
		{
			RemLOG_Print("Wifi.SSID.\n");

			pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( pszAucArg == NULL ) 	return HxCMD_ERR;

			if( sscanf(pszAucArg, "%s", (char *)&szParamName) == 0 ){
				return HxCMD_ERR;
			}

			RemLOG_Print("Wifi param2: (%s)\n", szParamName);

			/********************
			  * Do Action*
			  ********************/
			RemLOG_Print("=================start=======================\n");

		//	eREMA_TR069CpeGetSSIDNumberOfEntries,
		//	eREMA_TR069CpeGetWifiSSIDEnable,

			if(HxSTD_StrCmp(szParamName, "Enable") == 0)
			{
				RemLOG_Print("================= CALL ENABLE start=======================\n");
				Cwmp_cmd_Stb_getParam("Device.WiFi.SSID.0.Enable",output, 1);
			}
		}
		else if(HxSTD_StrCmp(szParamName, "GPV") == 0)
		{
			RemLOG_Print("Device.ManagementServer.URL\n");

			pszAucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( pszAucArg == NULL ) 	return HxCMD_ERR;

			if( sscanf(pszAucArg, "%s", (char *)&szParamName) == 0 ){
				return HxCMD_ERR;
			}

			RemLOG_Print("Wifi GPV : (%s)\n", szParamName);
			Cwmp_cmd_Stb_getParam(szParamName,output, 1);
		}
	}

	if(	eRemaMsg != eREMA_TR069_UNKNOWN)
	{
		RemaSendMsgCmd(eRemaMsg);
	}

	RemLOG_Print("=================end=======================\n");

	REM_FUNC_LEAVE;

	return HxCMD_OK;
}



HBOOL _rema_CMD_Register_cwmp(void)
{


    REM_FUNC_ENTER;

    HLIB_CMD_RegisterWord( hCmdHandle, cmd_cwmp_do_action, "do", "start | stop", "cwmp do [action]");

    HLIB_CMD_RegisterWord( hCmdHandle, cmd_cwmp_GPN, "gpn", "GetParameternames [ParameterPath] [NextLevel]", "[ParameterPath] [NextLevel] \n ex)gpn Device. true");
    HLIB_CMD_RegisterWord( hCmdHandle, cmd_cwmp_GPV, "gpv", "GetParameterValues [ParameterPath]", "[ParameterPath]  \n ex)gpv Device.");
    HLIB_CMD_RegisterWord( hCmdHandle, cmd_cwmp_SPV, "spv", "SetParameterValues [Parametername] [value]", "[Parametername] [value] \n ex)gpv Device.ManagementServer.PeriodicInformInterval 100");
    HLIB_CMD_RegisterWord( hCmdHandle, cmd_cwmp_AddObject, "aob", "AddObject [Objectname]", "[Objectname]\n ex) aob Device.PeriodicStatistics.SampleSet. ");
    HLIB_CMD_RegisterWord( hCmdHandle, cmd_cwmp_DeleteObject, "dob", "DeleteObject [Objectname]", "[Objectname] ex) dob Device.PeriodicStatistics.SampleSet.1. ");

    HLIB_CMD_RegisterWord( hCmdHandle, cmd_cwmp_cpe_cmd, "cpe", "cpe command input .. ", "cwmp cpe [open][close]");
    HLIB_CMD_RegisterWord( hCmdHandle, cmd_wifi_cmd, "wifi", "wifi command input .. ", "wifi ssid [enable]");

    REM_FUNC_LEAVE;

    return TRUE;
}

void rema_CMD_Deinit(void)
{
    REM_FUNC_ENTER;

    REM_FUNC_LEAVE;
}


HBOOL rema_CMD_Init(void)
{
    REM_FUNC_ENTER;
	_rema_CMD_Register_cwmp();
    REM_FUNC_LEAVE;
    return TRUE;
}


HBOOL Cwmp_cmd_Stb_getParam (char *key, char **output, int nextlevel)
{
	RemLOG_Print("Cwmp_cmd_Stb_getParam ###################\n\n");

	REM_ENTER;

	if(output == NULL)
	{
		RemLOG_Print("Cwmp_cmd_Stb_getParam Null pointer!!\n\n");
		return CWMP_STB_RES_FAIL;
	}

	RemLOG_Print("Cwmp_cmd_Stb_getParam key:[%s], nextlevel:[%d]\n", key, nextlevel);

	*output = rema_cwmp_GetParam(key);
	if(*output==NULL)
	{
		RemLOG_Print("couldn't get parameter value!\n");
		REM_LEAVE(CWMP_STB_RES_FAIL);
	}

	RemLOG_Print("################ Cwmp_cmd_Stb_getParam output:[%s]\n", *output);


	REM_LEAVE(CWMP_STB_RES_SUCCESS);
}


#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

