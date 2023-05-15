/**************************************************************
 *	@file		tr069.c
 *	TR-069 Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/20
 *	@author			humax
 *	@brief			TR-069 Application
 **************************************************************/
#include <ondk.h>
#include	<oapi.h>
#include	<hapi.h>

#if defined(CONFIG_ENHANCED_SECURITY)
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif

#include <sys/ioctl.h>
#include <trport_common.h>

#include <CWMP_STB_API.h>
#include <CWMP_CPE_API.h>


/**************************************************************
  internal structures and variables
 **************************************************************/
#ifdef	CONFIG_DEBUG
#define		DBG(...)		do { if (s_bTR069DBG) HxLOG_Print(__VA_ARGS__); } while(0)
#else
#define		DBG(...)
#endif

#define		HxCMD_PORT_APP_SUB			HxCMD_PORT_TR069  // telnet port!!!


typedef struct	_TR069Data
{
	HBOOL				show;
} TR069Data_t;

static	TR069Data_t		_stTR069Info;


//#define TEST_CASE
#if defined(TEST_CASE)
static HUINT32 timerid = 0;
#endif


/**************************************************************
  private functions
 **************************************************************/
#if defined(CONFIG_ENHANCED_SECURITY)
static	void dropUserPrevilegeAndChroot(void)
{
	int i;
	struct passwd *u_info;
	struct group  *g_info;
	struct rlimit rlim;

#if (0)
	/* 1. Close open file descriptors */
	if (getrlimit(RLIMIT_NOFILE, &rlim))
	{
		printf("getrlimit() for RLIMIT_NOFILE failed with error %s\n",
				strerror(errno));
		exit(-1);
	}

	for (i = 0; i < rlim.rlim_max; i++)
	{
		while (close(i) != 0 && errno == EINTR);
	}


	/* 2. Chroot */
	if (chroot("/hmx/octo") == -1)
	{
		printf("\n\t\t =============> APP_ChrootAndDropUserPrevilege : error chroot.....\n");
		exit(-1);
	}
	else
	{
		printf("\n\t\t =============> APP_ChrootAndDropUserPrevilege : succ chroot.....\n");
	}

	if (chdir("/"))
	{
		printf("chdir() failed...\n");
		exit(-1);
	}
#endif


	/* 3. Drop root privileges	to a non-privileged uesr */
	/* get GID and UID */
	u_info = getpwnam("settop");
	g_info = getgrnam("settop");

	if ( (u_info != NULL) && (g_info != NULL) )
	{
		uid_t real, eff, saved;
		gid_t gid = (gid_t)g_info->gr_gid;
		uid_t uid = (uid_t)u_info->pw_uid;

		if (setresgid(gid, gid, gid) != 0)
		{
			HxLOG_Print("Failed changing GID to %d with error %s\n",
					gid, strerror(errno));
			exit(-1);
		}

		if (setresuid(uid, uid, uid) != 0)
		{
			HxLOG_Print("Failed changing UID to %d with error %s\n",
					uid, strerror(errno));
			exit(-1);
		}

		if (getresuid(&real, &eff, &saved) != 0)
		{
			HxLOG_Print("Failed reading UID with error %s\n", strerror(errno));
			exit(-1);
		}

		if (real != uid || eff != uid || saved != uid)
		{
			HxLOG_Print("UID sanity check failed\n");
			exit(-1);
		}

		if (getresgid(&real, &eff, &saved) != 0)
		{
			HxLOG_Print("Failed reading GID with error %s\n", strerror(errno));
			exit(-1);
		}

		if (real != gid || eff != gid || saved != gid)
		{
			HxLOG_Print("GID sanity check failed\n");
			exit(-1);
		}

		HxLOG_Print("Dropped root priviliges. Now running as UID %u GID %u\n", uid, gid);
	}
	else
	{
		exit(-1);
	}
}
#endif

#ifdef	CONFIG_DEBUG
/*
APK_SQCSCENARIOJAPAN_StopRFOTA(&ret);
APK_SQCSCENARIOJAPAN_FinalizeIPOTA(&ret);
HAPI_SendSignalNumber(eHSIG_RequestReboot, 0);
*/


static int	CMD_STB_Test(void *szArgList)
{
	HCHAR	 		*aucArg=NULL;
	HCHAR	 		aucStr[20], aucStr2[128];

	/********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 ) return HxCMD_ERR;

    /********************
     *     action       *
     ********************/
	if (HxSTD_StrCmp(aucStr, "log") == 0)
	{
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL ) return HxCMD_ERR;
		if( sscanf(aucArg, "%s", (char *)&aucStr2) == 0 ) return HxCMD_ERR;

		if (HxSTD_StrCmp(aucStr2, "on") == 0)
		{
			;
		}
		else if (HxSTD_StrCmp(aucStr2, "off") == 0)
		{
			;
		}
	}
	else if (HxSTD_StrCmp(aucStr, "getparam") == 0)
	{
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL ) return HxCMD_ERR;
		if( sscanf(aucArg, "%s", (char *)&aucStr2) == 0 ) return HxCMD_ERR;

		TRP_GetParam(aucStr2);
	}
	else if (HxSTD_StrCmp(aucStr, "setparam") == 0)
	{
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL ) return HxCMD_ERR;
		if( sscanf(aucArg, "%s", (char *)&aucStr2) == 0 ) return HxCMD_ERR;

		TRP_SetParam(aucStr2);
	}
	else if (HxSTD_StrCmp(aucStr, "setacs") == 0)
	{
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL ) return HxCMD_ERR;
		if( sscanf(aucArg, "%s", (char *)&aucStr2) == 0 ) return HxCMD_ERR;

		HxLOG_Print("Set ACS address to Native data[%s]\n",aucStr2);
		if(APK_SQCSCENARIOJAPAN_SetAcsURL(aucStr2) != ERR_OK)
		{
			HxLOG_Error("Fail to set ACS address\n");
		}
	}
	else if (HxSTD_StrCmp(aucStr, "dialog") == 0)
	{
		TRP_GFX_CreateWindow();
		TRP_GFX_DrawRebootDialog();
	}
	else if (HxSTD_StrCmp(aucStr, "des") == 0)
	{
		TRP_GFX_DestroyWindow();
	}
	else if (HxSTD_StrCmp(aucStr, "reboot") == 0)
	{
		TRP_EVENT_Reboot();
	}
	else if (HxSTD_StrCmp(aucStr, "showobject") == 0)
	{
		TRP_TEST_PrintAllObject();
	}
	else if (HxSTD_StrCmp(aucStr, "version") == 0)
	{
		HINT32 i = 0, j = 0;
		HINT32 len = 0;
		HCHAR	pszOutput2Buff[32];
		HCHAR	pszOutput2[32];
		pszOutput2Buff[0]='\0';
		pszOutput2[0]='\0';

		APK_SQCBOXINFO_GetItem(eBoxInfo_Software_Version, pszOutput2Buff);
		len = HxSTD_StrLen(pszOutput2Buff);
		HxLOG_Debug("Real Software version(%s), len(%d)\r\n",pszOutput2Buff,len);
		for(i = (TRPORT_SOFTWARE_VERSION_PREFIX+1); i < len; i++)
		{
			pszOutput2[j++] = pszOutput2Buff[i];
			pszOutput2[(len-TRPORT_SOFTWARE_VERSION_PREFIX)] = '\0';
		}
		HxLOG_Debug("New Software version(%s), len(%d)\r\n",pszOutput2,HxSTD_StrLen(pszOutput2));


	}
	else if (HxSTD_StrCmp(aucStr, "msg") == 0)
	{
		TRP_GFX_ShowMsg();
	}
	else
	{
		HLIB_CMD_Printf("[CMD_TEST_ipota] Error, unknown action: %s\r\n", aucStr);
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static int	CMD_CWMP_log(void *szArgList)
{
	HCHAR	 		*aucArg=NULL;
	HCHAR	 		aucStr[20];

	/********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 ) return HxCMD_ERR;

    /********************
     *     action       *
     ********************/
	/* To display cmd_testocto log */

	if(HxSTD_StrCmp(aucStr, "on") == 0)
	{
		Cwmp_Cpe_PrintLog(true);
	}
	else if(HxSTD_StrCmp(aucStr, "off") == 0)
	{
		Cwmp_Cpe_PrintLog(false);
	}
	else
	{
		HLIB_CMD_Printf("[CMD_CWMP_Test] Error, unknown action: %s\r\n", aucStr);
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

void APTR069_CMD_Register(void)
{
		HLIB_CMD_RegisterWord("test",
			CMD_CWMP_log,
			"cwmplog",											/* keyword */
			"on/off CWMP library log printing console",						/*   help  */
			"cwmplog [on/off]");						/*  usage  */

		HLIB_CMD_RegisterWord("test",
			CMD_STB_Test,
			"stb",											/* keyword */
			"for test tr069 process",						/*   help  */
			"stb [log/printjson/getparam/setparam/dialog/setacs/version/msg/reboot/...]");						/*  usage  */
}
#endif

/**************************************************************
  Static Function
 **************************************************************/



/*
   static	void	TR069_cbTimeoutOccur(HUINT32 ulTimerId)
   {
   HxLOG_Debug("TR069_cbTimeoutOccur : [%x]\n", ulTimerId);

#if defined(TEST_CASE)

#endif
}
*/

/**************************************************************
  Event Handler
 **************************************************************/
static	void	TR069_signalNotifier(HSIGNAL_e eSignal, HSIGNALTYPE_e eSignalType, HINT32 size, void *data)
{
	switch (eSignal)
	{
		default:
			break;
	}
}


/**************************************************************
  public functions
 **************************************************************/
int		main(int argc, char **argv)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	dropUserPrevilegeAndChroot();
#endif

	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

	VK_Init();

//	APK_Init(eModule_SQC | eModule_DOWNLOADER | eSubModule_MASK);
	ONDK_Init(argc, argv);

	//	Initialize Internal Structure
	HxSTD_MemSet(&_stTR069Info, 0, sizeof(TR069Data_t));
	HAPI_Register("tr069Hapi",40,0x4000,TR069_signalNotifier);

	ONDK_STRINGS_Init(argc, argv);

#ifdef	CONFIG_DEBUG
	APTR069_CMD_Register();
#endif

	Cwmp_Cpe_open(NULL);
	while (1)
	{
		//OAPI_DispatchMessage("Tr069OapiTask",40,0x4000);
		VK_TASK_Sleep(1000);
	}
	Cwmp_Cpe_close();
	return 0;
}

