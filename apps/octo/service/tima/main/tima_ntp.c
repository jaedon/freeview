/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  tima_ntp.c
	@brief

	Description:  									\n
	Module: TiMa NTP 								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>     /* gethostbyname */
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include	"tima_int.h"

/*
NTP Protocol
Discription is in src dir.

0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| LI| VN  | MODE| STRATUM       | POLL          | PRECISION     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

32 bits		|LI|VN |MODE|STRATUM|POLL|PRECISION|
32 bits		ROOT DELAY
32 bits		ROOT DISPERSION
32 bits		REFERENCE IDENTIFIER
64 bits		REFERENCE TIMESTAMP
64 bits		ORIGINATE TIMESTAMP
64 bits		RECEIVE TIMESTAMP
64 bits		TRANSMIT TIMESTAMP
96 bits		AUTHENTICATOR (optional)
*/

#define NTP_POS_LI 			30
#define NTP_POS_VN 			27
#define NTP_POS_MODE 		24
#define NTP_POS_STRATUM 	16
#define NTP_POS_POLL 		8
#define NTP_POS_PREC 		0

#define NTP_LI 				0
#define NTP_VN 				3
#define NTP_MODE 			3
#define NTP_STRATUM 		0
#define NTP_POLL 			4
#define NTP_PREC 			-6

#define __TIMA_NTP_PORT (123)
#define __TIMA_URL_MAX	(256)

#define __TIMA_NTP_MAX_TRY_CNT	(5)

#define LINUX_DEFAULT_SECOND  0x83AA7E80      /* 1970 - 1900 in seconds */


#if defined(CONFIG_PROD_OS_EMULATOR)
#define sockaddr_in emu_sockaddr_in
#define hostent		emu_hostent
#define timeval		emu_timeval
#endif
static HERROR tima_SetNtpTime(HUINT32 ulUnixTime)
{
	TIMA_t	*pstTima;
	pstTima = Tima_GetInstance();
	if(pstTima->nSource == 0)
	{
		HxLOG_Print("UnixTime[%x]\n", ulUnixTime);
		pstTima->nSource = TxSOURCE_SET_BY_NTP;
		SETAPI_SetTime(ulUnixTime);
		HLIB_RPC_Notify(pstTima->nRPCHandle, "tima:onSourceChanged", "i", pstTima->nSource);
	}
	else
	{
		HUINT32 ulOldTime = HLIB_STD_GetSystemTime();
		HxLOG_Print("another Source Set the time[%x].\n", ulOldTime);
		if(VALID_UNIXTIME >= ulOldTime &&  VALID_UNIXTIME < ulUnixTime)
		{
			pstTima->nSource = TxSOURCE_SET_BY_NTP;
			SETAPI_SetTime(ulUnixTime);
			HLIB_RPC_Notify(pstTima->nRPCHandle, "tima:onSourceChanged", "i", pstTima->nSource);
		}
	}
	Tima_ReleaseInstance(pstTima);
	return ERR_OK;
}

static HUINT32 tima_SendOrgTime(int uSockfd)
{
	HUINT32 ulTimeSec = 0;
	HUINT32 pData[12];

	memset(pData,0,sizeof(pData));
	pData[0] = htonl (
		( NTP_LI << NTP_POS_LI ) | ( NTP_VN << NTP_POS_VN ) | ( NTP_MODE << NTP_POS_MODE ) |
		( NTP_STRATUM << NTP_POS_STRATUM) | ( NTP_POLL << NTP_POS_POLL ) | ( NTP_PREC & 0xff ) );
	pData[1] = htonl(1<<16);
	pData[2] = htonl(1<<16);
	ulTimeSec = HLIB_STD_GetSystemTime();
	pData[10] = htonl(ulTimeSec);
	pData[11] = 0;
	HxLOG_Print("send_packet %x\n", ulTimeSec);
	if(send(uSockfd,pData,48,0) < 0)
		return 0;
	else
	return ulTimeSec;
}
#define GET_NETDATA(data, address) ntohl(((HUINT32 *)data)[address])

static int tima_GetNtpTimeFromServer(HUINT32 *data, HUINT32 ulArrivalTime, HUINT32 ulOrgTime)
{
	HINT32 ulNtpLi, ulNtpVn, ulNtpMode, ulNtpStratum, ulNtpPoll, ulNtpPrecision, ulNtpDelay, ulNtpDisp, refid;
	HUINT32 ulRefSec, ulRefMliSec,	uOrgSec, ulOrgMliSec, uRecSec,ulLastSec, ulLastMliSec;

	ulNtpLi			= GET_NETDATA(data,0) >> NTP_POS_LI & 0x03;
	ulNtpVn			= GET_NETDATA(data,0) >> NTP_POS_VN & 0x07;
	ulNtpMode		= GET_NETDATA(data,0) >> NTP_POS_MODE & 0x07;
	ulNtpStratum		= GET_NETDATA(data,0) >> NTP_POS_STRATUM & 0xff;
	ulNtpPoll			= GET_NETDATA(data,0) >>  NTP_POS_POLL & 0xff;
	ulNtpPrecision		= GET_NETDATA(data,0) & 0xff;
	if (ulNtpPrecision & 0x80) ulNtpPrecision|=0xffffff00;
	ulNtpDelay		= GET_NETDATA(data,1);
	ulNtpDisp			= GET_NETDATA(data,2);
	refid				= GET_NETDATA(data,3);
	ulRefSec			= GET_NETDATA(data,4) - LINUX_DEFAULT_SECOND;
	ulRefMliSec		= GET_NETDATA(data,5);
	uOrgSec			= GET_NETDATA(data,6);
	ulOrgMliSec		= GET_NETDATA(data,7);
	uRecSec			= GET_NETDATA(data,8) - LINUX_DEFAULT_SECOND;
	ulOrgMliSec		= GET_NETDATA(data,9);
	ulLastSec		= GET_NETDATA(data,10) - LINUX_DEFAULT_SECOND;
	ulLastMliSec		= GET_NETDATA(data,11);

	HxLOG_Print("Reference %x\n", ulRefSec);
	HxLOG_Print("(sent)    %x\n", ulOrgTime);
	HxLOG_Print("Originate %x\n", uOrgSec);
	HxLOG_Print("Receive   %x\n", uRecSec);
	HxLOG_Print("Transmit  %x\n", ulLastSec);
	HxLOG_Print("Our recv  %x\n", ulArrivalTime);

	if (ulNtpLi == 3 || ulNtpVn < 3 || ulNtpMode != 4 || uOrgSec != ulOrgTime
		|| (ulLastSec == 0 && ulLastMliSec == 0)
		|| ulNtpDelay > 65536 || ulNtpDelay < -65536
		|| ulNtpDisp  > 65536 || ulNtpDisp  < -65536 || ulNtpStratum == 0)
	{
		HxLOG_Critical("ulNtpLi=%d	ulNtpVn=%d	ulNtpMode=%d  ulNtpStratum=%d  ulNtpPoll=%d  ulNtpDelay=%d, ulNtpDisp=%d ulNtpPrecision=%d\n",
			ulNtpLi, ulNtpVn, ulNtpMode, ulNtpStratum, ulNtpPoll, ulNtpDelay, ulNtpDisp, ulNtpPrecision);
		return 0;
	}
	else
	{
		return ulLastSec;
	}
}

static void tima_FinishedNtpThread(void)
{
	TIMA_t	*pstTima = NULL;
	pstTima = Tima_GetInstance();
	pstTima->ulNtpThreadId = 0;
	Tima_ReleaseInstance(pstTima);
}

static void tima_NtpThread(void *arg)
{

	TIMA_t	*pstTima = NULL;
	HINT32	uSockfd = -1;
	HCHAR 	usNtpUrl[__TIMA_URL_MAX] = {0,};
	struct sockaddr_in stSockClient, stSockServer;

	pstTima = Tima_GetInstance();
	if(pstTima->usNtpUrl) HxSTD_StrNCpy(usNtpUrl, pstTima->usNtpUrl, __TIMA_URL_MAX-1);
	Tima_ReleaseInstance(pstTima);

	if(usNtpUrl[0] == 0)
	{
		HxLOG_Error("There is No NTP URL\n");
		tima_FinishedNtpThread();
		return;
	}
	if ((uSockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
	{
		HxLOG_Error("Socket Create Failed\n");
		tima_FinishedNtpThread();
		return;
	}

	HxSTD_memset(&stSockClient,0,sizeof stSockClient);
	HxSTD_memset(&stSockServer,0,sizeof stSockServer);

	stSockClient.sin_family=AF_INET;
	stSockClient.sin_addr.s_addr=htonl(INADDR_ANY);
	stSockClient.sin_port=htons(0);
	if(bind(uSockfd, (struct sockaddr*)&stSockClient,sizeof(stSockClient)) != -1)
	{
		struct addrinfo stHints;
		struct addrinfo *pstRes = NULL;
		struct addrinfo *pstRp = NULL;
		HINT32 nRet;

		HxSTD_memset(&stHints, 0, sizeof(stHints));
		stHints.ai_socktype = SOCK_DGRAM;
		stHints.ai_family = AF_INET;
		nRet = getaddrinfo(usNtpUrl, "ntp", &stHints, &pstRes);
		if (nRet != 0) {
			HxLOG_Error("GetHostByName Fail [%s]\n", usNtpUrl);
			goto END;
		}
		stSockServer.sin_family=AF_INET;
		stSockServer.sin_port=htons(__TIMA_NTP_PORT);

		for (pstRp = pstRes; pstRp != NULL; pstRp = pstRp->ai_next)
		{
			HxSTD_memset(&(stSockServer.sin_addr.s_addr), 0x00, sizeof(stSockServer.sin_addr.s_addr));

			switch (pstRes->ai_family)
			{
			case AF_INET:
				HxSTD_memcpy(&(stSockServer.sin_addr.s_addr), &((struct sockaddr_in *) pstRes->ai_addr)->sin_addr, sizeof(pstRes->ai_addrlen));
				break;
			case AF_INET6:
				HxSTD_memcpy(&(stSockServer.sin_addr.s_addr), &((struct sockaddr_in6 *) pstRes->ai_addr)->sin6_addr, sizeof(pstRes->ai_addrlen));
				break;
			}

			if (connect(uSockfd,(struct sockaddr *)&stSockServer,sizeof stSockServer)!=-1)
			{
				break;
			}
		}

		if (pstRp == NULL)			/* No address succeeded */
		{
			HxLOG_Error("connect Fail\n");
			freeaddrinfo(pstRes);			/* No longer needed */
			goto END;
		}

		freeaddrinfo(pstRes);			/* No longer needed */

		{
			static HUINT32	pReveivedDAta[325];
			HINT32			lSelectError = 0, lReceivedDataLen = 0;
			HUINT32 		ulSendTime = 0;
			HUINT32			ulTimeOutCnt = 0;
			fd_set			stFds;
			struct timeval 	stTimeOut;

			stTimeOut.tv_sec=60;
			stTimeOut.tv_usec=0;

			while(1)
			{

				if(ulSendTime == 0) ulSendTime = tima_SendOrgTime(uSockfd); // Request
				FD_ZERO(&stFds);
				FD_SET(uSockfd,&stFds);

				lSelectError=select(uSockfd+1,&stFds,NULL,NULL,&stTimeOut);

				if ((lSelectError!=1)||(!FD_ISSET(uSockfd,&stFds)))
				{
					ulTimeOutCnt++;
					if(ulTimeOutCnt > __TIMA_NTP_MAX_TRY_CNT)
					{
						break;
					}
					HxLOG_Error("NTP Server does not respond [%d] Max[%d].\n", ulTimeOutCnt, __TIMA_NTP_MAX_TRY_CNT);
					continue;
				}

				lReceivedDataLen =recvfrom(uSockfd, pReveivedDAta,325,0, NULL, NULL);
				if (lReceivedDataLen >0 && (unsigned)lReceivedDataLen <325)
				{
					HUINT32 ulNtpTime = 0;
					if((ulNtpTime = tima_GetNtpTimeFromServer(pReveivedDAta, HLIB_STD_GetSystemTime(), ulSendTime)) != 0)
					{
						tima_SetNtpTime(ulNtpTime);
						break;
					}
				}
				else
				{
					HxLOG_Error("lReceivedDataLen =%d is invalid\n",lReceivedDataLen);
				}
			}
		}
	}
	else
	{
		HxLOG_Error("Socket Bind\n");
	}

END:
	if(uSockfd) close(uSockfd);
	tima_FinishedNtpThread();
}


HERROR TIMA_CreateNtp(HCHAR * ulURL)
{
	TIMA_t	*pstTima;
	HERROR	hErr = ERR_FAIL;
	pstTima = Tima_GetInstance();

	if(pstTima->ulNtpThreadId == 0)
	{
		if(pstTima->usNtpUrl)	HLIB_STD_MemFree(pstTima->usNtpUrl);
		pstTima->usNtpUrl = HLIB_STD_StrDup(ulURL);
		VK_TASK_Create(tima_NtpThread, VK_TASK_PRIORITY_MW_REF, SIZE_1K, (const char*)"NtpThread", (void*)pstTima, (unsigned long *)&(pstTima->ulNtpThreadId), 0);
		VK_TASK_Start(pstTima->ulNtpThreadId);
		hErr = ERR_OK;
	}
	Tima_ReleaseInstance(pstTima);
	return hErr;
}



