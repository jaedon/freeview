
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

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <trauma_int.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define hCmdHandle  "tracmd"

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */

static int	trauma_cmd_login(void *szArgList)
{
	TRAUMA_TVPORTAL_Login();
	return HxCMD_OK;
}

static int	trauma_cmd_authTicket(void *szArgList)
{
	HCHAR *ticket;
	ticket = TRAUMA_TVPORTAL_GetAuthTicket();
	HLIB_CMD_Printf("AuthTicket: %s \n",ticket);
	HLIB_MEM_Free(ticket);
	return HxCMD_OK;
}

static int	trauma_cmd_rpc(void *szArgList)
{
	TRAUMA_RPC_Print();
	return HxCMD_OK;
}

static int	trauma_cmd_acceptPrivacy(void *szArgList)
{
	HCHAR 	*aucArg=NULL;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	if(HxSTD_StrCmp(aucArg,"y") == 0)
		TRAUMA_TVPORTAL_SetAcceptPrivacy(TRUE);
	else if(HxSTD_StrCmp(aucArg,"n") == 0)
		TRAUMA_TVPORTAL_SetAcceptPrivacy(FALSE);
	else
	{
		return HxCMD_ERR;
	}
	return HxCMD_OK;
}

static int	trauma_cmd_log(void *szArgList)
{
	HxList_t *pList,*pRetList;

	pRetList = TRAUMA_STATISTICS_GetLogs();

	pList = pRetList;
	HLIB_CMD_Printf("***********************************************\n");
	while(pList)
	{
		HLIB_CMD_Printf("\n\n %s\n\n",(HCHAR*)HLIB_LIST_Data(pList));
		pList = HLIB_LIST_Next(pList);
	}
	HLIB_CMD_Printf("***********************************************\n");
	HLIB_LIST_RemoveAllFunc(pRetList,HLIB_MEM_Free_CB);
	return HxCMD_OK;
}


static int	trauma_cmd_getlogfilter(void *szArgList)
{
	HBOOL 		bActive;
	HxList_t 	*pAllowList,*pDenyList,*pList;
	HUINT32		ulInterval,ulTTL;

	bActive = TRAUMA_TVPORTAL_GetLogFilterActiveStatus();
	pAllowList = TRAUMA_TVPORTAL_GetLogFilterAllowList();
	pDenyList = TRAUMA_TVPORTAL_GetLogFilterDenyList();
	ulInterval = TRAUMA_TVPORTAL_GetLogFilterInterval();
	ulTTL = TRAUMA_TVPORTAL_GetLogFilterTTL();

	HLIB_CMD_Printf("***********************************************\n");
	HLIB_CMD_Printf("ACTIVE : %s \n",bActive == TRUE ? "TRUE" : "FALSE");
	HLIB_CMD_Printf("INTERVAL : %d, TTL: %d\n",ulInterval,ulTTL);
	HLIB_CMD_Printf("ALLOW : %d \n",pAllowList ? HLIB_LIST_Length(pAllowList) : 0);
	pList = pAllowList;
	while(pList)
	{
		HLIB_CMD_Printf("\t %s \n",(HCHAR*)HLIB_LIST_Data(pList));
		pList = HLIB_LIST_Next(pList);
	}
	HLIB_CMD_Printf("DENY : %d \n",pDenyList ? HLIB_LIST_Length(pDenyList) : 0);
	pList = pDenyList;
	while(pList)
	{
		HLIB_CMD_Printf("\t %s \n",(HCHAR*)HLIB_LIST_Data(pList));
		pList = HLIB_LIST_Next(pList);
	}
	HLIB_CMD_Printf("***********************************************\n");

	HLIB_LIST_RemoveAllFunc(pAllowList,HLIB_MEM_Free_CB);
	HLIB_LIST_RemoveAllFunc(pDenyList,HLIB_MEM_Free_CB);
	return HxCMD_OK;
}


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
void TRAUMA_CMD_Init(void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
		trauma_cmd_login,
		"login",			        /* keyword */
		"Login TVPortal",			/*	 help  */
		"login");	        	    /*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,
		trauma_cmd_authTicket,
		"auth",			        /* keyword */
		"Get AuthTicket",		/*	 help  */
		"auth");	        	   /*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,
		trauma_cmd_rpc,
		"rpc",			        /* keyword */
		"print rpc",		/*	 help  */
		"rpc");	        	   /*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,
		trauma_cmd_acceptPrivacy,
		"privacy",			        /* keyword */
		"Set Accept Privacy [yes/no]",		/*	 help  */
		"privacy[y/n]");	        	   /*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,
		trauma_cmd_log,
		"log",			        /* keyword */
		"print Statistics logs",		/*	 help  */
		"log");	        	   /*	usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
		trauma_cmd_getlogfilter,
		"filter",			        /* keyword */
		"print Logfilter",			/*	 help  */
		"filter");	        	 	 /*	usage  */

}



