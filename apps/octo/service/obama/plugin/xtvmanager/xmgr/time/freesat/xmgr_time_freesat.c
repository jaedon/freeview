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

STATIC MgrTime_ClockRecoverRefTpInfo_t	s_stClockRefTp_Fsat[] =
{ /* Clock time search will be done according to the order of the below TP info until the clock time is successfully set up.
	Modify the Antenna order and list according to the clock setup scenario */
#if defined(CONFIG_MW_CH_SATELLITE)
	{eREF_SAT_TP, eSatType_ASTRA_2, 11428, 27500, eDxSAT_CODERATE_2_3, eDxSAT_POLAR_HOR, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_020},
#endif
	{eREF_WAKEUP_SVC, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_LAST_SVC, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef CONFIG_MW_CH_SATELLITE
	{eREF_FIRST_SVC_SAT, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
#ifdef CONFIG_MW_CH_TERRESTRIAL
	{eREF_FIRST_SVC_TER, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
#ifdef CONFIG_MW_CH_CABLE
	{eREF_FIRST_SVC_CAB, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
	{eREF_END, 0, 0, 0, 0, 0, 0, 0, 0} /* eREF_END will terminate the clock setup process */
};

#ifdef	CONFIG_PACKAGE_NTPCLIENT
STATIC HUINT32			s_ulNTPClientThreadId = 0;
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR xmgr_time_ClockRecoverGetDstMode_FreeSat (VK_CLOCK_DST_MODE *peDstMode)
{
	return ERR_FAIL;
}

MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_FreeSat (void)
{
	return s_stClockRefTp_Fsat;
}

#if defined(CONFIG_PACKAGE_NTPCLIENT)
typedef void * (*pthread_routine)(void *);

STATIC void xmgr_time_GetNetworkTime_FreeSat(void *pvData)
{
	HUINT8	*pszTimeServer,*pStrPtr;
	HUINT32 ulReadSize=0;

	int 	stat_DEL = 0;

	int 	childPid = 0;
	pid_t	pid;


	/* vfork and exec udhcpc */ /* insmod */
	if((pid = vfork()) < 0)
	{
		HxLOG_Assert(0);

		pthread_detach(pthread_self());
		return;
	}
	else if(pid == 0)
	{
		/* child */
		/* ??ë¶€ë¶„ë???child ?„ë¡œ?¸ì„œ?´ê¸° ?Œë¬¸?? execlp ?„ê¹Œì§€ ?ŒìŠ¤ì½”ë“œ???¬ì‹¤ ?ì‹?„ë¡œ?¸ì„œê°€
		 * ë¶€ëª??„ë¡œ?¸ì„œ??ë©”ëª¨ë¦¬ë? ?¬ìš©?˜ê³  ?ˆëŠ” ê²ƒì´ê¸??Œë¬¸??		 * ?˜ì •???„ìš”??ë³´ìž…?ˆë‹¤.
		 */
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
			HLIB_STD_MemFree(pStrPtr);
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

	s_ulNTPClientThreadId = 0; // ?¬ì‹¤ ?´ëŸ¬??ê²ƒì? ?œë?ë¡???ê²??€ ?„ë‹ˆ??.. ?°ì„ ... :-)...
	pthread_detach(pthread_self());

}
#endif

HERROR xmgr_time_SetGmtOffset_FreeSat(HINT32 nGmtOffset, HBOOL bUpdateTzFile)
{
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
#if 0
 		HxSTD_snprintf( szSystemCmd , 256, "echo \"%s\" > %s" , szTimezone, TZ_NAME_PATH);
 		VK_SYSTEM_Command(szSystemCmd);

 		HxSTD_snprintf( szSystemCmd , 128, "cp -rf %s %s" , TZ_NAME_PATH, TZ_NAME_PATH_TMP);
 		VK_SYSTEM_Command(szSystemCmd);
#endif

#if defined(CONFIG_PACKAGE_NTPCLIENT)
		if ( 0 == s_ulNTPClientThreadId )
		{
			pthread_create( &s_ulNTPClientThreadId, NULL, (pthread_routine)xmgr_time_GetNetworkTime_FreeSat, NULL);
			//INT_APPMGR_GetNetworkTime();
		}
#endif
	}

	return ERR_OK;
}

/*********************** End of File ******************************/
