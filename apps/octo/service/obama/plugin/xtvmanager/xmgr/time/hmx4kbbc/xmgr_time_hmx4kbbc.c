/**
	@file     	xmgr_clockrecover_base.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_param.h>
#include <db_svc.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_time.h>
#include <pmgr_time.h>
#include "../local_include/_xmgr_time.h"

#include <pthread.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#ifdef	CONFIG_PACKAGE_NTPCLIENT
STATIC HUINT32			s_ulNTPClientThreadId_4kbbc = 0;
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#if defined(CONFIG_PACKAGE_NTPCLIENT)
typedef void * (*pthread_routine)(void *);

STATIC void xmgr_time_GetNetworkTime_Hmx4kbbc(void *pvData)
{
	HUINT8	*pszTimeServer,*pStrPtr;
	HUINT32 ulReadSize=0;

	int 	stat_DEL = 0;

	int 	childPid = 0;
	pid_t	pid;
	HUINT8	cmdStr[2048];


#if 0
	/* vfork and exec udhcpc */ /* insmod */
	if((pid = vfork()) < 0)
	{
		HxLOG_Assert(0);

		pthread_detach(pthread_self());
		return;
	}
	else if(pid == 0)
	{
		HLIB_FILE_ReadFile((const HUINT8 *)"/media/drive1/time_server.txt", &pszTimeServer, &ulReadSize);

		if (0 != ulReadSize && NULL != pszTimeServer)
		{
			pStrPtr = HLIB_STD_StrDup((const HCHAR *)pszTimeServer);
			HLIB_STD_MemFree(pszTimeServer);
		}
		else
		{
			pStrPtr = HLIB_STD_StrDup((const HCHAR *)"uk.pool.ntp.org");//"ntp2d.mcc.ac.uk";
		}

		VK_Print("\n\nTimeServer : %s\n\n",pStrPtr);
		/* child */
		/* exec udhcp, try 5 seconds per 1 reuest, all 6 request */
//						if(execlp("rdate", "rdate","-s", pStrPtr, (char *)0) < 0)
		if(execlp("ntpclient", "ntpclient","-s", "-h", pStrPtr, (char *)0) < 0)
		{
			if (pStrPtr)
				HLIB_STD_MemFree(pStrPtr);
			//HxLOG_Assert(0);
			_exit(EXIT_FAILURE);
			return;
		}
		if (pStrPtr)
		{
			HLIB_STD_MemFree(pStrPtr);
		}
		VK_Print("\n\nSuccess with server : %s\n\n",pStrPtr);
	}
	else
	{
		/* parent */
		childPid = waitpid(pid, &stat_DEL , 0);
		if ( -1 == childPid )
		{
			HxLOG_Assert(0);
		}

		VK_Print("\n-------------------------------------------\n");
		VK_Print("\n(*) ntpclient exit!!\n");
		VK_Print("\n-------------------------------------------\n");
		VK_SYSTEM_Command("touch " OBAMA_DIR_DATA "/resolv.conf");
	}

	s_ulNTPClientThreadId_4kbbc = 0;
	pthread_detach(pthread_self());
#else
	HLIB_FILE_ReadFile((const HUINT8 *)"/media/drive1/time_server.txt", &pszTimeServer, &ulReadSize);

	if (0 != ulReadSize && NULL != pszTimeServer)
	{
		pStrPtr = HLIB_STD_StrDup((const HCHAR *)pszTimeServer);
		HLIB_STD_MemFree(pszTimeServer);
	}
	else
	{
		pStrPtr = HLIB_STD_StrDup((const HCHAR *)"uk.pool.ntp.org");//"ntp2d.mcc.ac.uk";
	}

	HxLOG_Error("\n\nTimeServer : %s\n\n",pStrPtr);

	while (1)
	{
		snprintf(cmdStr, 2408, "%s%s%s", "ntpclient -s -h ", pStrPtr, " -i 10 &> /dev/null");
		VK_SYSTEM_Command(cmdStr);
		sleep(20);
	}
	s_ulNTPClientThreadId_4kbbc = 0;
#endif

}
#endif

HERROR xmgr_time_SetGmtOffset_Hmx4kbbc(HINT32 nGmtOffset, HBOOL bUpdateTzFile)
{
	HxLOG_Error("\n\n NTP Working nGmtOffset=(%d), bUpdateTzFile=(%d): \n\n", nGmtOffset, bUpdateTzFile);

	DB_PARAM_SetItem(eDB_PARAM_ITEM_GMT_OFFSET_MINUTES, nGmtOffset, 0);

	if (bUpdateTzFile)
	{
		HCHAR	szTimezone[MAX_TIMEZONE_STR_LEN];
		HCHAR	szSystemCmd[384];

		//	Set TZ
		HxSTD_PrintToStrN(szTimezone, MAX_TIMEZONE_STR_LEN, "PST0PDT-1,M3.5.0/1,M10.5.0/2");

		// Write down to /etc/TZ file.
		HxSTD_snprintf(szSystemCmd, 128, "%s%c", szTimezone, 0x0a);
		HLIB_FILE_WriteFile((const HUINT8 *)TZ_NAME_PATH, szSystemCmd, HxSTD_StrLen(szSystemCmd));
		HLIB_FILE_Delete((const HUINT8 *)TZ_NAME_PATH_TMP);
		HLIB_FILE_Copy((const HUINT8 *)TZ_NAME_PATH, (const HUINT8 *)TZ_NAME_PATH_TMP);

#if defined(CONFIG_PACKAGE_NTPCLIENT)
		if ( 0 == s_ulNTPClientThreadId_4kbbc )
		{
			pthread_create( &s_ulNTPClientThreadId_4kbbc, NULL, (pthread_routine)xmgr_time_GetNetworkTime_Hmx4kbbc, NULL);
		}
#endif
	}

	return ERR_OK;
}

HERROR xmgr_time_Init_Hmx4kbbc(void)
{
	HINT32	nGmtOffset = 60;
	HBOOL	bUpdateTzFile = TRUE;

	xmgr_time_SetGmtOffset_Hmx4kbbc(nGmtOffset, bUpdateTzFile);

	return ERR_OK;
}


/*********************** End of File ******************************/
