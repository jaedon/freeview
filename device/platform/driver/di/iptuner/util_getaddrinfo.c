/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_getaddrinfo.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	getaddrinfo/getnameinfo replacement functions
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
//#define _GNU_SOURCE
#include <stddef.h> /* size_t */
#include <string.h> /* VK_strlen(), memcpy(), memset(), strchr() */
#include <stdlib.h> /* malloc(), free(), strtoul() */
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#include "htype.h"
#include "vkernel.h"
#include "di_uart.h"
#include "di_iptuner_common.h"


#include "util_getaddrinfo.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
#ifndef NO_ADDRESS
#   define NO_ADDRESS  NO_DATA
#endif
#ifndef INADDR_NONE
#   define INADDR_NONE 0xFFFFFFFF
#endif
#ifndef AF_UNSPEC
#   define AF_UNSPEC   0
#endif

//#define UTIL_GETADDRINFO_DEBUG
//#define UTIL_GETADDRINFO_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef UTIL_GETADDRINFO_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter		while (0) ((int (*)(char *, ...)) 0)
#define PrintExit		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* The END of DEBUG DEFINE */

#ifdef UTIL_GETADDRINFO_CRITICAL_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */

/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/



// TODO : junsy
// iconv not supported .
//
// to support , should be modified using iconv
#if 0
char *ToLocale (const char *utf8)
{
	if( utf8 == NULL )
	{
		return NULL;
	}
	return utf8;
}
#endif
void LocaleFree (const char *str)
{
//#if defined (USE_ICONV)
//	if (!find_charset ())
//		DD_MEM_Free ((char *)str);
//#elif defined (USE_MB2MB)
//	DD_MEM_Free ((char *)str);
//#else
	(void)str;
//#endif
}

/* prevent GPL2.0 modify */
int util_getnameinfo( const struct sockaddr *pSa, int nSalen,
                     char *pHost, int nHostlen, int *pPortnum, int flags )
{
    char pszServbuf[6], *pszServ;
    int nServlen, nVal;

    flags |= NI_NUMERICSERV;
    if( pPortnum != NULL )
    {
        pszServ = pszServbuf;
        nServlen = sizeof( pszServbuf );
    }
    else
    {
        pszServ = NULL;
        nServlen = 0;
    }

    nVal = getnameinfo(pSa, nSalen, pHost, nHostlen, pszServ, nServlen, flags);

    if( pPortnum != NULL )
	{
        *pPortnum = VK_atoi( pszServ );
	}

    return nVal;
}

/* prevent GPL2.0 modify */
int util_getaddrinfo( const char *pNode,
                     int nPort, const struct addrinfo *pHints,
                     struct addrinfo **res )
{
    struct addrinfo hints;
    char pszBuff[NI_MAXHOST], pszService[6];
    int nRet;

    if( ( nPort > 65535 ) || ( nPort < 0 ) ) /* port : 1~65535 */
    {
        PrintError ( "Error : invalid port_number %d specified.", nPort );
        return EAI_SERVICE;
    }

    /* cannot overflow : because 65535 */
    VK_snprintf( pszService, 6, "%d", nPort );

    /* Check if we have to force ipv4 or ipv6 */
    VK_memset  (&hints, 0, sizeof (hints));

    if (pHints != NULL)
    {
        const int safe_flags =
            AI_PASSIVE |
            AI_CANONNAME |
            AI_NUMERICHOST |
#if 0
			/* XXX junsy : uClibc 0.9.29 에서는 getaddrinfo에서 AI_PASSIVE|AI_CANONNAME|AI_NUMERICHOST|AI_ADDRCONFIG|AI_V4MAPPED|AI_ALL
			를 제외한 나머지 Flag는 지원하지 않는다. uClibc Source를 참조할것 */
            AI_NUMERICSERV |
#endif
#ifdef AI_ALL
            AI_ALL |
#endif
#ifdef AI_ADDRCONFIG
            AI_ADDRCONFIG |
#endif
#ifdef AI_V4MAPPED
            AI_V4MAPPED |
#endif
            0;

        hints.ai_family = pHints->ai_family;
        hints.ai_socktype = pHints->ai_socktype;
        hints.ai_protocol = pHints->ai_protocol;
        /* Unfortunately, some flags chang the layout of struct addrinfo, so they cannot be copied blindly from pHints to &hints. Therefore, we only copy flags that we know for sure are "safe". */
        hints.ai_flags = pHints->ai_flags & safe_flags;
    }

    /* We only ever use port *numbers* */
#if 1
	/* XXX junsy : uClibc 0.9.29 에서는 getaddrinfo에서 AI_PASSIVE|AI_CANONNAME|AI_NUMERICHOST|AI_ADDRCONFIG|AI_V4MAPPED|AI_ALL
	 * 를 제외한 나머지 Flag는 지원하지 않는다. uClibc Source를 참조할것
	 */
//  hints.ai_flags |= AI_NUMERICSERV;
//  hints.ai_flags |= AI_NUMERICHOST;
#endif

    if( hints.ai_family == AF_UNSPEC )
    {
#if 0 // junsy
#ifdef AF_INET6
        if (var_CreateGetBool ("ipv6"))
            hints.ai_family = AF_INET6;
        else
#endif
        if (var_CreateGetBool ("ipv4"))
#endif
            hints.ai_family = AF_INET;
    }

    if (pNode != NULL)
    {
        if (pNode[0] == '[')
        {
            /* warning: cast discards qualifiers from pointer target type */
            /* size_t len = VK_strlen  (node + 1); */
            size_t len = VK_strlen  (pNode + 1);
            if ((len <= sizeof (pszBuff)) && (pNode[len] == ']'))
            {
                assert (len > 0);
                VK_memcpy (pszBuff, pNode + 1, len - 1);
                pszBuff[len - 1] = '\0';
                pNode = pszBuff;
            }
        }
        if (pNode[0] == '\0')
		{
            pNode = NULL;
		}
    }

#if 0
    pNode = ToLocale (pNode);
	/* XXX junsy : uClibc 0.9.29 에서는 getaddrinfo에서 AI_PASSIVE|AI_CANONNAME|AI_NUMERICHOST|AI_ADDRCONFIG|AI_V4MAPPED|AI_ALL
	 * 를 제외한 나머지 Flag는 지원하지 않는다. uClibc Source를 참조할것
	 * AI_IDN은 정의는 되어 있으나 사용하지 않는다.(uClibc에서는 정상동작하지 않음)
	 */
#ifdef AI_IDN
    /* Run-time I18n Domain Names support */
    hints.ai_flags |= AI_IDN;
	PrintData ("%s(%d) : 0x%x 0x%x 0x%x 0x%x \n",__FUNCTION__,__LINE__ ,hints.ai_flags ,hints.ai_family ,hints.ai_socktype ,hints.ai_protocol);
    nRet = getaddrinfo (pNode, pszService, &hints, res);
    if (nRet != EAI_BADFLAGS)
	{
		PrintError ("%s(%d) : Error> NO EAI_BADFLAGS (%d) \n",__FUNCTION__,__LINE__, nRet);
        goto out;
	}
    /* IDN not available: disable and retry without it */
    hints.ai_flags &= ~AI_IDN;
#endif
#endif
	PrintData("%s(%d) : pNode(%s) pszService(%s)\n",__FUNCTION__,__LINE__,pNode, pszService);
    nRet = getaddrinfo (pNode, pszService, &hints, res);
	PrintData ("%s(%d) : nRet (%d) \n",__FUNCTION__,__LINE__, nRet);

#if 0 /* AI_IDN dependency */
out:
#endif
    LocaleFree (pNode);
    return nRet;
}


void util_freeaddrinfo( struct addrinfo *infos )
{
    freeaddrinfo (infos);
}

/**
 * inet_pton() replacement
 */
int util_inet_pton (int af, const char *src, void *dst)
{
    return inet_pton( af, src, dst );
}

/**
 * inet_ntop() replacement
 */
const char *util_inet_ntop (int af, const void *src, char *dst, socklen_t cnt)
{
    return inet_ntop( af, src, dst, cnt );
}


/* prevent GPL2.0 modify */
HBOOL net_SockAddrIsMulticast (const struct sockaddr *pAddr, socklen_t nLen)
{
    switch (pAddr->sa_family)
    {
#ifdef IN_MULTICAST
        case AF_INET:
        {
            const struct sockaddr_in *pIpv4 = (const struct sockaddr_in *)pAddr;
            if ((size_t)nLen < sizeof(*pIpv4))
			{
                goto end;
			}
            return IN_MULTICAST (ntohl(pIpv4->sin_addr.s_addr)) != 0;
        }
#endif

#ifdef IN6_IS_ADDR_MULTICAST
        case AF_INET6:
        {
            const struct sockaddr_in6 *pIpv6 = (const struct sockaddr_in6 *)pAddr;
            if ((size_t)nLen < sizeof(*pIpv6))
			{
                goto end;
			}
            return IN6_IS_ADDR_MULTICAST (&pIpv6->sin6_addr) != 0;
        }
#endif
    }

end :
    return FALSE;
}


/* prevent GPL2.0 modify */
int net_GetSockAddress( int nFd, char *pAddr, int *port )
{
    struct sockaddr_storage addr_storage;
    socklen_t addr_storagelen = sizeof( addr_storage );

    return getsockname( nFd, (struct sockaddr *)&addr_storage, &addr_storagelen )
        || util_getnameinfo( (struct sockaddr *)&addr_storage, addr_storagelen, pAddr, NI_MAXNUMERICHOST, port, NI_NUMERICHOST )
        ? DI_IPT_EGENERIC : 0;
}

/* prevent GPL2.0 modify */
int net_GetPeerAddress( int nFd, char *pAddr, int *port )
{
	struct sockaddr_storage addr_storage;
	socklen_t addr_storagelen = sizeof( addr_storage );

	return getpeername( nFd, (struct sockaddr *)&addr_storage, &addr_storagelen )
		|| util_getnameinfo( (struct sockaddr *)&addr_storage, addr_storagelen, pAddr, NI_MAXNUMERICHOST, port, NI_NUMERICHOST )
		? DI_IPT_EGENERIC : 0;
}

/* prevent GPL2.0 modify */
HUINT16 net_GetPort (const struct sockaddr *pAddr)
{
	switch (pAddr->sa_family)
	{
#ifdef AF_INET6
		case AF_INET6:
			return ((const struct sockaddr_in6 *)pAddr)->sin6_port;
#endif
		case AF_INET:
			return ((const struct sockaddr_in *)pAddr)->sin_port;
	}
	return 0;
}

/* prevent GPL2.0 modify */
void net_SetPort (struct sockaddr *pAddr, HUINT16 nPort)
{
	switch (pAddr->sa_family)
	{
#ifdef AF_INET6
		case AF_INET6:
			((struct sockaddr_in6 *)pAddr)->sin6_port = nPort;
			break;
#endif
		case AF_INET:
			((struct sockaddr_in *)pAddr)->sin_port = nPort;
			break;
	}
}
