/** **********************************************************************************************************
	@file 		corsair_debug.cpp

	@date		2012/10/11
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#include	"corsair_config.h"
#include 	"corsair_utils.h"
#include 	"corsair.h"
#include 	"corsair_cert.h"
#include 	"corsair_cert_manager.h"
#include 	"corsair_debug.h"
#include 	"corsair_tools.h"
#include 	"corsair_hbbtv_manager.h"
#include 	"corsair_postman.h"
#include 	"corsair_ait_manager.h"

#include 	"gogi_mem_api.h"
#include	"opllocalsystem.h"
#include 	"bplglobal.h"

#include 	"hlib.h"

#include 	<iostream>
#include 	<cstdio>
#include	<vkernel.h>
#include 	<stdio.h>
#include 	<string.h>
#include 	<stdlib.h>


#define MAX_AP_WEB_CMD_SIZE							(30)

#define CORSAIR_SHELL_NAME 							(HINT8 *)"CORSAIR_CMD"

static int cmdParseDSMCC(void *szArgs);
static int cmdParseCookieInfo(void *szArgs);
static int cmdParseDragonfly(void *szArgs);
static int cmdParseUrl(void *szArgs);
static int cmdParseMemInfo(void *szArgs);
static int cmdParseCertInfo(void *szArgs);
static int cmdParseCmd(void *szArgs);
static int cmdWindowList(void *szArgs);
static int cmdAppList(void *szArgList);
static int cmdPortalUrl(void *szArgList);
static int cmdShowAITInfo(void *szArgList);
#if defined(CONFIG_PROD_FVP4000T)
static int cmdMhegControl(void *szArgList);
#endif

static int cmdParseDSMCC(void *szArgList)
{
	char *aucArg=NULL;
	char aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	if ( strcmp(aucArg, "on") == 0 )
	{
		CorsairDebug::debugOnDSMCC();
	}
	else if ( strcmp(aucArg, "off") == 0 )
	{
		CorsairDebug::debugOffDSMCC();
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static int cmdParseUrl(void *szArgList)
{
	char *aucArg=NULL;
	char aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	if ( strcmp(aucArg, "get") == 0 )
	{
		CorsairDebug::debugGetUrl();
	}
	else if ( strcmp(aucArg, "set") == 0 )
	{
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg == NULL )
		{
			LOG_DEBUG("usage : url [set] [ex)www.google.com]\n");
			return HxCMD_ERR;
		}
		CorsairDebug::debugSetUrl((char *)aucArg);
	}
	else if ( strcmp(aucArg, "open") == 0 )
	{
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg == NULL )
		{
			LOG_DEBUG("usage : url [open] [ex)www.google.com]\n");
			return HxCMD_ERR;
		}
		CorsairDebug::debugOpenUrl((char *)aucArg);
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static int cmdParseCookieInfo(void *szArgList)
{
	char *aucArg=NULL;
	char aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	if ( strcmp(aucArg, "info") == 0 )
	{
		CorsairDebug::debugCookieInfo();
	}
	else if ( strcmp(aucArg, "delete") == 0 )
	{
		CorsairDebug::debugCookieDelete();
	}
	else if ( strcmp(aucArg, "rm") == 0 )
	{

		LOG_DEBUG("web cookie remove\n");
		system("rm -rf /var/lib/humaxtv/browser/cookies4.dat");
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static int cmdParseMemInfo(void *szArgList)
{

	char *aucArg=NULL;
	char aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);

	CorsairDebug::debugCoutMemInfo();

#if 0
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	if ( strcmp(aucArg, "info") == 0 )
	{
		CorsairDebug::debugCoutMemInfo();
	}
	else
	{
		return HxCMD_ERR;
	}
#endif

	return HxCMD_OK;
}

static int cmdParseDragonfly(void *szArgList)
{
	char *aucArg=NULL, *szIpaddr=NULL, *szPort=NULL;
	char aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	if ( strcmp(aucArg, "off") == 0 )
	{

		CorsairDebug::debugStopDragonfly();
	}
	else if ( strcmp(aucArg, "on") == 0 )
	{
		szIpaddr = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( szIpaddr == NULL )
		{
			LOG_DEBUG("USAGE: dragonfly on [IP Address] [Port Number]\n");
			LOG_DEBUG("try to connect form usb\n");
			CorsairDebug::debugStartDragonfly(NULL, 0);
			return HxCMD_OK;
		}

		szPort = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( szPort == NULL )
		{
			LOG_DEBUG("USAGE: dragonfly on [IP Address] [Port Number]\n");
			LOG_DEBUG("try to connect form usb\n");
			CorsairDebug::debugStartDragonfly(NULL, 0);
			return HxCMD_OK;
		}
		CorsairDebug::debugStartDragonfly((char *)szIpaddr, atoi((char *)szPort));
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static int cmdParseCertInfo(void *szArgs)
{
	return HxCMD_OK;
}

static int cmdParseCmd(void *szArgList)
{
	char *aucArg=NULL;
	char aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	if ( strcmp(aucArg, "aiton") == 0 )
		{
		if ( !getCorsair() || !getCorsair()->getPostman() ) return HxCMD_ERR;

		CorsairMail *mail = new CorsairMailEnableHbbTV(true);

		if ( !mail ) return HxCMD_ERR;

		getCorsair()->getPostman()->post(mail);
	}
	else if ( strcmp(aucArg, "aitoff") == 0 )
		{
		if ( !getCorsair() || !getCorsair()->getPostman() ) return HxCMD_ERR;

		CorsairMail *mail = new CorsairMailEnableHbbTV(false);

		if ( !mail ) return HxCMD_ERR;

		getCorsair()->getPostman()->post(mail);
	}
	else
	{
		return HxCMD_ERR;
	}
	return HxCMD_OK;
}

extern Corsair* g_corsair;
static int cmdWindowList(void *szArgList)
{
	char *aucArg=NULL;
	char aucStr[20];
	int grab = -1;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;

	OPL_LocalSystem_getOipfGrab(&grab);

	if ( strcmp(aucArg, "oipf") == 0 )
	{
		LOG_DEBUG( "--------------------- << OIPF WINDOW :: %d >> ---------------------------\n",grab);
        if(getCorsair()->getOipfSurface())
    		getCorsair()->getOipfSurface()->showWindowList();
	}
	else if ( strcmp(aucArg, "hbbtv") == 0 )
	{
		LOG_DEBUG( "--------------------- << HBBTV WINDOW >> --------------------------\n");
		g_corsair->getHbbTVSurface()->showWindowList();
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static int cmdAppList(void *szArgList)
{
	char *aucArg=NULL;
	char aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;

	if ( strcmp(aucArg, "list") == 0 )
		getCorsair()->getHbbTVManager()->showApps();
	else
		return HxCMD_ERR;

	return HxCMD_OK;
}

static int cmdPortalUrl(void *szArgList)
{
	char *aucArg=NULL, *aucUrl = NULL;
	char aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;

	if ( strcmp(aucArg, "url") == 0 ){
		CorsairHbbTVApp *app = getCorsair()->getHbbTVManager()->getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
		aucUrl = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		app->setUrl(aucUrl);
	}
	else
		return HxCMD_ERR;

	CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
	if ( hbbtv_manager )
		hbbtv_manager->action(new CorsairHbbTVStartAction(CorsairHbbTVApp::HBBTV_APPTYPE_BI, hbbtv_manager));


	return HxCMD_OK;
}
static int cmdShowAITInfo(void *szArgList)
{
	CorsairAITManager::showAITInfo();
    
	return HxCMD_OK;
}
#if defined(CONFIG_PROD_FVP4000T)
static int cmdMhegControl(void *szArgList)
{
	char *aucArg=NULL;
	char aucStr[20];
	int grab = -1;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ){
        LOG_DEBUG("MHEG STATUS : %s\n",BplGlobalConfig::s_mheg_status?"LOAD":"NOT LOAD");
        return HxCMD_ERR;
	}

	if ( strcmp(aucArg, "on") == 0 )
	{
		CorsairTools::mhegOn();
	}
	else if ( strcmp(aucArg, "off") == 0 )
	{
		CorsairTools::mhegOff();
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}
#endif

static int cmdObjectDebuger(void *szArgList)
{
	char *aucArg=NULL, *szIpaddr=NULL, *szPort=NULL;
	char aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	if ( strcmp(aucArg, "off") == 0 )
	{
		BplGlobalConfig::s_object_debuger_enable = false;
	}
	else if ( strcmp(aucArg, "on") == 0 )
	{
		BplGlobalConfig::s_object_debuger_enable = true;
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}



static HUINT32 gLimit_mem 	= 250; //250mb
static HUINT32 gTotal_Rss 	= 0;   //kB
static HUINT32 gCur_Rss 	= 0;   //kB
static HUINT32 opera_mem 	= 0;	//kB

static HULONG  gTid			= 0;
static HBOOL   gPrint 		= FALSE;

static HxFILE_t gLog		= NULL;
typedef struct{
	HCHAR 	header[512];
	HUINT32 Size; //kB
	HUINT32 Rss; //kB
	HUINT32 Pss; //kB
	/*
	....
	*/
}SMaps;

static HINT32 memcheck_sortRss(void *src,void *data)
{
	SMaps *src_map = (SMaps*)src;
	SMaps *data_map = (SMaps*)data;

	return data_map->Rss - src_map->Rss;
}


static void memcheck_foreach(void *data)
{
	SMaps *samps = (SMaps*)data;
	gCur_Rss += samps->Rss;
	HCHAR	buffer[512] = {0,};

	HxSTD_snprintf(buffer,sizeof(buffer),"Rss:%d - %s",samps->Rss,samps->header);

	if(gPrint ==TRUE)
		LOG_WARN("%s",buffer);

	if(gLog)
		HLIB_FILE_Write(gLog,buffer,HxSTD_StrLen(buffer),1);
}

static void memcheck_print(void)
{
	HUINT32 curmem = 0;
	HINT32  diffmem;
	HCHAR	buffer[512] = {0,};
	curmem =  op_get_allocated_memory(); //byte..
	curmem =  curmem / 1024; //kB

	diffmem = gCur_Rss - gTotal_Rss;

	if(diffmem > 0)
		HxSTD_snprintf(buffer,sizeof(buffer),"Corsair Rss %d Kb, [UP %d Kb]\n",gCur_Rss ,diffmem);
	else if(diffmem < 0 )
		HxSTD_snprintf(buffer,sizeof(buffer),"Corsair Rss %d Kb [Down %d Kb]\n",gCur_Rss, diffmem);
	else
		HxSTD_snprintf(buffer,sizeof(buffer),"Corsair Rss %d Kb [Same]\n",gCur_Rss);

	if(gPrint)
	{
		LOG_WARN("********************************************************\n");
		LOG_WARN("%s\n",buffer);
	}
	if(gLog)
		HLIB_FILE_Write(gLog,buffer,HxSTD_StrLen(buffer),1);

	diffmem = curmem - opera_mem;

	if(diffmem > 0 )
		HxSTD_snprintf(buffer,sizeof(buffer),"Opera core %d Kb, [UP %d Kb] \n",curmem ,diffmem);
	else if(diffmem < 0 )
		HxSTD_snprintf(buffer,sizeof(buffer),"Opera core %d Kb, [Down %d Kb] \n",curmem, diffmem);
	else
		HxSTD_snprintf(buffer,sizeof(buffer),"Opera core %d Kb, [Same] \n",curmem);

	if(gPrint)
	{
		LOG_WARN("%s\n",buffer);
		LOG_WARN("********************************************************\n");
	}

	if(gLog)
		HLIB_FILE_Write(gLog,buffer,HxSTD_StrLen(buffer),1);

	opera_mem = curmem;
	gTotal_Rss = gCur_Rss;
	gCur_Rss = 0;

}

static void	memcheck_thread (void *arg)
{
	HCHAR 	szSmapsPath[PATH_MAX] = {0,};
	HINT32 	pid = (HINT32)arg;

	HxSTD_snprintf(szSmapsPath,sizeof(szSmapsPath),"proc/%d/smaps" ,pid);

	while(gTid != 0)
	{
		HxFILE_t fp;
		HxList_t *pMapList = NULL;

		fp = HLIB_FILE_Open((HUINT8*)szSmapsPath,(HUINT8*)"r");
		if(fp)
		{
			gLog = HLIB_FILE_Open((HUINT8*)"/corsair_mem.txt",(HUINT8*)"a+");
			while(HLIB_FILE_EOF(fp) ==0)
			{
				HCHAR		pLine[1024] ={0,};
				SMaps 		*map;

				map =  (SMaps*)HLIB_STD_MemCalloc(sizeof(SMaps));

				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops;
					HxSTD_StrNCpy(map->header,pLine,512);

				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops;
				if(sscanf(pLine, "Size: %d kB", &map->Size) != 1)  	 goto oops;

				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops;
				if(sscanf(pLine, "Rss: %d kB", &map->Rss) != 1)  	 goto oops;

				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops;
				if(sscanf(pLine, "Pss: %d kB", &map->Pss) != 1)  	 goto oops;

				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops; //Shared_Clean
				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops; //Shared_Dirty
				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops; //Private_Clean
				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops; //Shared_Clean
				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops; //Private_Dirty

				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops; //Referenced
				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops; //Swap
				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops; //KernelPageSize
				if(HLIB_FILE_GetString(fp,(HUINT8*)pLine,1024) == 0) goto oops; //MMUPageSize

				if(map->Rss==0)
				{
				oops:
					HLIB_STD_MemFree(map);
					continue;
				}
				pMapList = HLIB_LIST_AppendSortEx(pMapList,map,memcheck_sortRss);
			} // end of while
			HLIB_FILE_Close(fp);


			if(pMapList)
			{
				HLIB_LIST_Foreach(pMapList,memcheck_foreach);
				if(gTotal_Rss > (gLimit_mem * 1024 )) // diff kB..
				{
					gPrint = TRUE;
					// TODO: 모니터하는 PID 의 Total Rss 보다, 사용량이 많을 경우..
				}

				memcheck_print();
				HLIB_LIST_Foreach(pMapList , HLIB_STD_MemFree_CB);
				HLIB_LIST_RemoveAll(pMapList);
				pMapList = NULL;
			}

			if(gLog)
			{
				HLIB_FILE_Close(gLog);
				gLog = NULL;
			}
		}
		VK_TASK_Sleep(10000);
	}
}


static int cmdMemCheck_start(void *szArgList)
{
	HUINT32  pid =0;
	HCHAR	 *aucArg,*szMem = NULL;

	pid = HLIB_STD_GetPID();

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		gLimit_mem = atoi(aucArg);
	}
	LOG_WARN( "Start: Limit[%d MB]\n", gLimit_mem);

	if(gTid==0)
	{
		VK_TASK_Create(memcheck_thread, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "mem_check", (void*)pid, &gTid, 0);
		VK_TASK_Start(gTid);
	}

	return HxCMD_OK;
}

static int cmdMemCheck_stop(void *szArgList)
{
	gTid = 0;
	return HxCMD_OK;
}
static int cmdMemCheck_print(void *szArgList)
{
	HCHAR *aucArg=NULL;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;

	if ( HxSTD_StrCmp(aucArg, "on") == 0 )
	{
		gPrint = TRUE;
		LOG_WARN("Set print on\n");
	}
	else if ( HxSTD_StrCmp(aucArg, "off") == 0 )
	{
		gPrint = FALSE;
		LOG_WARN("Set print off\n");
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

/** **************************************************************************************
	@class	CorsairDebug
 	@brief 	this class is for debuging Opera functions.
****************************************************************************************** */
CorsairDebug::CorsairDebug()
{
}

CorsairDebug::~CorsairDebug()
{
}

int CorsairDebug::getRSS(void)
{
    FILE *in;
    char buff[512];
    char cmd [512] = {0, };
    int ret  = 0;

    snprintf(cmd, sizeof(cmd), "cat /proc/%d/status | grep VmRSS | sed 's/[^ ]* *//;s/ .*//;q'", getpid());

    if(!(in = popen(cmd, "r"))) return -1;

    while(fgets(buff, sizeof(buff), in)!=NULL){
        ret = atoi(buff);
    }

    pclose(in);

    return ret;
}


void CorsairDebug::debugCoutMemInfo(void)
{
	LOG_DEBUG("web meminfo :: opera_heap_size = %d \n",op_get_heap_size());
	LOG_DEBUG("web meminfo :: opera_allocated_size = %d \n",op_get_allocated_memory());
#if 0 /* temp code - for write memory status */
    time_t now = time(0);
    tm *ltm = localtime(&now);

    char cmd[1024] = {0,};
    snprintf(cmd, sizeof(cmd),"echo \"%d:%d:%d,%d,%d,%d,%d \" >> /mem_result.csv",
            ltm->tm_hour, ltm->tm_min, ltm->tm_sec,
            op_get_heap_size()/(1024*1024), op_get_allocated_memory()/(1024*1024), getRSS()/1024);
    system(cmd);
#endif
}

void CorsairDebug::debugCookieInfo(void)
{
	unsigned int cookie_count;
	GogiCookie *gogi_cookie = NULL;

	if ( getCorsair() == NULL ) return;

	if ( op_get_cookie_list(getCorsair()->getOpera(), &gogi_cookie, &cookie_count) != GOGI_STATUS_OK )
	{
		return;
	}

	if( cookie_count == 0 )
		LOG_DEBUG("cookie count = 0\n");

	for ( int i=0; i<cookie_count; ++i )
	{
		LOG_DEBUG("cookie [%d].domain = %s\n",i+1,gogi_cookie[i].domain);
		LOG_DEBUG("cookie [%d].path = %s\n",i+1,gogi_cookie[i].path);
		LOG_DEBUG("cookie [%d].value = %s\n",i+1,gogi_cookie[i].value);
		LOG_DEBUG("cookie [%d].expires = %d\n",i+1,gogi_cookie[i].expires);
		LOG_DEBUG("cookie [%d].secure = %d\n",i+1,gogi_cookie[i].secure);
	}

	op_free_cookie_list(getCorsair()->getOpera());
}

void CorsairDebug::debugCookieDelete(void)
{
	LOG_DEBUG("web cookie delete\n");

	unsigned int cookie_count;
	GogiCookie *gogi_cookie = NULL;

	if ( getCorsair() == NULL ) return;

	if ( op_get_cookie_list(getCorsair()->getOpera(), &gogi_cookie, &cookie_count) != GOGI_STATUS_OK )
	{
		return;
	}

	if( cookie_count == 0 )
		LOG_DEBUG("cookie count = 0\n");

	for ( int i=0; i<cookie_count; ++i )
	{
		LOG_DEBUG("delete cookie [%d]\n",i+1);
		op_delete_cookie(getCorsair()->getOpera(),&gogi_cookie[i]);
	}

	op_free_cookie_list(getCorsair()->getOpera());
}

void CorsairDebug::debugStartDragonfly(const char *ip_addr, unsigned short port)
{
	bool bConncet = false;
	if( CorsairTools::startDragonFly(ip_addr, port) != true )
	{
		bConncet = CorsairTools::startDragonFlyWithUsb();
		if( !bConncet )
			LOG_DEBUG("Fail to connect dragonfly \n");
	}
}

void CorsairDebug::debugStopDragonfly(void)
{
	LOG_DEBUG("stop dragonfly\n");
	CorsairTools::stopDragonFly();
}

void CorsairDebug::debugGetUrl(void)
{
	std::string url;

	if ( !getCorsair() ) return;

	CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
	if ( !hbbtv_manager ) return;

	CorsairHbbTVApp *app = hbbtv_manager->getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
	if ( app )
	{
	CorsairTools::setPreferenceLanguage();
		LOG_DEBUG("current URL : %s \n",app->getUrl().c_str());
	}
}

void CorsairDebug::debugSetUrl(const char* url)
{
	if ( !getCorsair() ) return;

	CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
	if ( !hbbtv_manager ) return;

	CorsairHbbTVApp *app = hbbtv_manager->getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
	if ( app )
	{
		CorsairTools::setPreferenceLanguage();
		app->setUrl(url);
		LOG_DEBUG("setting url = %s \n",url);
	}
}

void CorsairDebug::debugOpenUrl(const char* url)
{
	if ( !getCorsair() || !getCorsair()->getPostman() ) return;

	CorsairMail *mail = new CorsairMailOpenUrl(url);
	if ( !mail ) return;

	getCorsair()->getPostman()->post(mail);
	LOG_DEBUG("loading url = %s \n",url);
}

void CorsairDebug::debugReloadMainAppFromUSB()
{
	CorsairTools::ReloadMainAppFromUSB();
}

void CorsairDebug::debugOnDSMCC()
{

	LOG_DEBUG("dsmc on\n");
	opera_hbbtv_send_event(E_CONTROL_BROADBAND_NOT_OPERATIONAL, NULL);
}

void CorsairDebug::debugOffDSMCC()
{
	LOG_DEBUG("dsmc off\n");
	opera_hbbtv_send_event(E_CONTROL_BROADBAND_OPERATIONAL, NULL);
}

void CorsairDebug::debugGetCertificate(void)
{
	return;
}

/** **************************************************************************************
	@class	CorsairDebugThread
 	@brief 	this class is to regist command tool
****************************************************************************************** */
static HULONG		s_threadId;

bool CorsairDebugThread::start()
{
#if	defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord("web", cmdParseUrl,
					 (const HCHAR *)"url",
					 (const HCHAR *)"url get/set/open",
					 (const HCHAR *)"url [get|set|open]");

	HLIB_CMD_RegisterWord("web", cmdParseDSMCC,
					 (const HCHAR *)"dsmcc",
					 (const HCHAR *)"dsmcc on/off",
					 (const HCHAR *)"dsmcc [on|off]");

	HLIB_CMD_RegisterWord("web", cmdParseCookieInfo,
					 (const HCHAR *)"cookie",
					 (const HCHAR *)"cookie info/delete/rm",
					 (const HCHAR *)"cookie [info|delete|rm]");

	HLIB_CMD_RegisterWord("web", cmdParseMemInfo,
					 (const HCHAR *)"meminfo",
					 (const HCHAR *)"meminfo",
					 (const HCHAR *)"meminfo");

	HLIB_CMD_RegisterWord("web", cmdParseDragonfly,
					 (const HCHAR *)"dragonfly",
					 (const HCHAR *)"dragonfly on/off",
					 (const HCHAR *)"dragonfly [on|off]");

	HLIB_CMD_RegisterWord("web", cmdParseCertInfo,
					 (const HCHAR *)"cert",
					 (const HCHAR *)"cert",
					 (const HCHAR *)"cert");

	HLIB_CMD_RegisterWord("web", cmdParseCmd,
					 (const HCHAR *)"cmd",
					 (const HCHAR *)"cmd aiton/aitoff",
					 (const HCHAR *)"cmd [aiton/aitoff]");

	HLIB_CMD_RegisterWord("web", cmdWindowList,
					 (const HCHAR *)"window",
					 (const HCHAR *)"window oipf/hbbtv",
					 (const HCHAR *)"window [oipf/hbbtv]");

	HLIB_CMD_RegisterWord("web", cmdAppList,
					 (const HCHAR *)"app",
					 (const HCHAR *)"app list",
					 (const HCHAR *)"app [list]");

	HLIB_CMD_RegisterWord("web", cmdPortalUrl,
					 (const HCHAR *)"home",
					 (const HCHAR *)"home url",
					 (const HCHAR *)"home [url]");

	HLIB_CMD_RegisterWord("web", cmdShowAITInfo,
					 (const HCHAR *)"ait",
					 (const HCHAR *)"show current channel's AIT information",
					 (const HCHAR *)"ait");
#if defined(CONFIG_PROD_FVP4000T)
	HLIB_CMD_RegisterWord("web", cmdMhegControl,
					 (const HCHAR *)"mhegctl",
					 (const HCHAR *)"mhegctl on/off",
					 (const HCHAR *)"mhegctl [on|off]");

	HLIB_CMD_RegisterWord("web", cmdMemCheck_start,
					 (const HCHAR *)"start",
					 (const HCHAR *)"start [arg1] or NULL",
					 (const HCHAR *)"start memory check arg1 = set limit [MB](>default 250mb)");

	HLIB_CMD_RegisterWord("web", cmdMemCheck_stop,
					 (const HCHAR *)"stop",
					 (const HCHAR *)"stop memory check",
					 (const HCHAR *)"stop memory check");

	HLIB_CMD_RegisterWord("web", cmdMemCheck_print,
					 (const HCHAR *)"print",
					 (const HCHAR *)"print memory",
					 (const HCHAR *)"print memory");

	HLIB_CMD_RegisterWord("web", cmdObjectDebuger,
					 (const HCHAR *)"objdebug",
					 (const HCHAR *)"objdebug on/off",
					 (const HCHAR *)"objdebug [on/off]");
#endif
#endif

	return true;
}

bool CorsairDebugThread::stop()
{
	VK_TASK_Stop(s_threadId);

	return true;
}
