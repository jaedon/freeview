/*
 * hxlog.c
 *
 *  Created on: 2011. 1. 17.
 */


#include <vkernel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <hlib.h>

#define	OS_malloc	HLIB_MEM_Malloc
#define	OS_free		HLIB_MEM_Free
#define	OS_memset	memset
#define	OS_memcpy	memcpy

#define	OS_snprintf		snprintf
#define	OS_vsnprintf	vsnprintf
#define OS_GetPID		HLIB_STD_GetPID
#define OS_GetTID		HLIB_STD_GetTID


#define	OS_printf		Hx_Print


STATIC HUINT32				s_ulSemID = 0;
STATIC HINT32				s_nServerSock = 0;
STATIC struct sockaddr_in 	s_stServerAddr;

STATIC void hlib_log_Init(HCHAR *server_ip, HINT32 nPort)
{
	if(s_ulSemID == 0)
	{
		if (HxSEMT_Create(&s_ulSemID, "HxLOG", HxSEMT_FIFO) != 0)
		{
			HxLOG_Error("[%s:%d] HxSEMT_Create error!!\r\n", __FUNCTION__, __LINE__);
			return;
		}
	}

	if(s_nServerSock == 0)
	{
		HxSEMT_Get(s_ulSemID);

		if(s_nServerSock == 0)
		{
		  	int sockfd;
		    int clilen;

		    struct sockaddr_in serveraddr;

		    clilen = sizeof(serveraddr);
		    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		    if (sockfd < 0)
		    {
				HxLOG_Error("[%s:%d] socket error!!\r\n", __FUNCTION__, __LINE__);
				return;
		    }

			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = inet_addr(server_ip);
			serveraddr.sin_port = htons(nPort);

			HxSTD_memcpy(&s_stServerAddr, &serveraddr, sizeof(struct sockaddr_in));
			s_nServerSock = sockfd;

			HxLOG_Print("[%s:%d] Log Sock Init OK\r\n", __FUNCTION__, __LINE__);
		}

		HxSEMT_Release(s_ulSemID);
	}

}


void HLIB_LOG_Init (HBOOL bTemp)
{
	hlib_log_Init("192.168.49.10", 23000);
}


void HLIB_LOG (const HCHAR *file, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv, const HCHAR *format, ...)
{
	va_list	ap;

	va_start(ap, format);
	HLIB_LOG_v(file, function, line, lv, format, ap);
	va_end(ap);

}


void	HLIB_LOG_v (const HCHAR *path, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv, const HCHAR *fmt, va_list ap)
{
	HCHAR buf[8192] = {0, };
	HCHAR printBuf[1024] = {0, };

	hlib_log_Init("192.168.49.10", 23000);


	switch (lv)
	{
	case HxLOG_PRINT:
		HxSTD_snprintf(buf, 8192, "%s(%d:%d);P;<%s:%s:%d>;", HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line); /* fix for prevent */
		break;

	case HxLOG_MESSAGE:
		HxSTD_snprintf(buf, 8192, "%s(%d:%d);M;<%s:%s:%d>;", HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line);
		break;

	case HxLOG_INFO:
		HxSTD_snprintf(buf, 8192, "%s(%d:%d);I;<%s:%s:%d>;", HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line);
		break;

	case HxLOG_DEBUG:
		HxSTD_snprintf(buf, 8192, "%s(%d:%d);D;<%s:%s:%d>;", HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line);
		break;

	case HxLOG_TRACE:
		HxSTD_snprintf(buf, 8192, "%s(%d:%d);T;<%s:%s:%d>;", HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line);
		break;

	case HxLOG_WARNING:
		HxSTD_snprintf(buf, 8192, HxANSI_COLOR_CYAN("%s(%d:%d);W;<%s:%s:%d>;"), HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line);
		break;

	case HxLOG_ERROR:
		HxSTD_snprintf(buf, 8192, HxANSI_COLOR_RED("%s(%d:%d);E;<%s:%s:%d>;"), HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line);
		break;

	case HxLOG_CRITICAL:
		HxSTD_snprintf(buf, 8192, HxANSI_COLOR_RED("%s(%d:%d);C;<%s:%s:%d>;"), HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line);
		break;

	case HxLOG_ASSERT:
		HxSTD_snprintf(buf, 8192, HxANSI_COLOR_RED("%s(%d:%d);A;<%s:%s:%d>;"), HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line);
		break;

	default:
		HxSTD_snprintf(buf, 8192, "%s(%d:%d);U;<%s:%s:%d>;", HLIB_STD_GetCurProcessName(), OS_GetPID(), OS_GetTID(), path, function, line);
		break;
	}

	OS_vsnprintf(printBuf, 1024, fmt, ap);
	HxSTD_StrCat(buf, printBuf);

	sendto(s_nServerSock, (void *)buf, HxSTD_StrLen(buf), 0, (struct sockaddr *)&s_stServerAddr, sizeof(struct sockaddr));
}


void	HLIB_LOG_Dump (HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail)
{
	HxLOG_Print("[%s:%d] \r\n", __FUNCTION__, __LINE__);
	HxLOG_Print("[%s:%d] \r\n", __FUNCTION__, __LINE__);
	HxLOG_Print("[%s:%d] \r\n", __FUNCTION__, __LINE__);
	HxLOG_Print("[%s:%d] \r\n", __FUNCTION__, __LINE__);

}

// TODO: delete
void	HLIB_LOG_SetLevel	(const HCHAR *domain, HxLOG_Level_e lv)
{
	(void)domain;
	(void)lv;
}

HxLOG_Level_e	HLIB_LOG_GetLevel	(const HCHAR *domain)
{
	(void)domain;
	return HxLOG_NONE;
}

void	HLIB_LOG_SetHandler (const HCHAR *domain, HxLOG_Cb_t handler, void *userdata)
{
	(void)domain;
	(void)handler;
	(void)userdata;
}

void	HLIB_LOG_SetGlobalLevel (HxLOG_Level_e lv)
{
	(void)lv;
}


void	HLIB_LOG_PrintDomain(void)
{
}


#if HxLOG_FILE_WITHOUT_PATH == 1 && defined(__FILE__)
const char *	HLIB_LOG_RemovePath(const char *file)
{
	char *ch;

	if (file == NULL)
		return file;

	ch = strrchr(file, (int)'/');
	if (ch == NULL)
	{
#if defined(WIN32)
		ch = strrchr(file, (int)'\\');
		if (ch == NULL)
#endif
		return file;
	}

	return ++ch;
}
#endif


