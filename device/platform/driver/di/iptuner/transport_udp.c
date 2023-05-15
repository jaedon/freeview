/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   transport_udp.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     Transport Layer (UDP) Body
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
#include "di_iptuner_common.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <errno.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>

#include "htype.h"
#include "vkernel.h"
#include "di_uart.h"
#include "util_getaddrinfo.h"
#include "transport_io.h"
#include "di_iptuner_config.h"

#ifdef HAVE_LINUX_DCCP_H
# include <linux/dccp.h>
# ifndef SOCK_DCCP /* provisional API */
#  define SOCK_DCCP 6
# endif
#endif
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define TRANSPORT_UDP_DEBUG
//#define TRANSPORT_UDP_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef TRANSPORT_UDP_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* The END of DEBUG DEFINE */

#ifdef TRANSPORT_UDP_CRITICAL_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)


#if !defined(SOL_UDPLITE)
# define SOL_UDPLITE IPPROTO_UDPLITE
#endif

#if !defined(IPPROTO_UDPLITE)
# define IPPROTO_UDPLITE 136
#endif

#if !defined(SOL_DCCP)
# define SOL_DCCP IPPROTO_DCCP
#endif

#if !defined(IPPROTO_DCCP)
# define IPPROTO_DCCP 33
#endif

#ifndef SOL_IP
# define SOL_IP IPPROTO_IP
#endif

#if defined(SUPPORT_IPV6_TRANSPORT)
#if !defined(SOL_IPV6)
# define SOL_IPV6 IPPROTO_IPV6
#endif
#if !defined(IPPROTO_IPV6)
# define IPPROTO_IPV6 41
#endif
#endif /* SUPPORT_IPV6_TRANSPORT */

#if defined (HAVE_NETINET_UDPLITE_H)
# include <netinet/udplite.h>
#elif defined (__linux__)
# define UDPLITE_RECV_CSCOV	 11
# define UDPLITE_SEND_CSCOV     10
#endif
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
static int net_SourceSubscribe (int nfd, const struct sockaddr *pSrcAddr, socklen_t srcAddrLen, const struct sockaddr *pGroupAddr, socklen_t groupAddrLen);

/* */
static int net_SetupDgramSocket( int nfd, const struct addrinfo *ptr )
{
#if defined(SO_REUSEPORT)
	setsockopt (nfd, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof (int));
#endif

#if defined(SO_RCVBUF)
    /* Increase the receive buffer size to 1/2MB (8Mb/s during 1/2s)
     * to avoid packet loss caused in case of scheduling hiccups */
	setsockopt (nfd, SOL_SOCKET, SO_RCVBUF, (void *)&(int){ 0x80000 }, sizeof (int));
	setsockopt (nfd, SOL_SOCKET, SO_SNDBUF, (void *)&(int){ 0x80000 }, sizeof (int));
#endif

	if (bind (nfd, ptr->ai_addr, ptr->ai_addrlen))
    {
        PrintError ( "%s (%d) Error> socket bind error \n",__FUNCTION__,__LINE__ ); /* ISO C does not support the '%m' VK_printf format */
		net_Close (nfd);
        return -1;
    }
	return nfd;
}

int net_Subscribe (int nfd, const struct sockaddr *saddr, socklen_t saddrlen)
{
	return net_SourceSubscribe (nfd, NULL, 0, saddr, saddrlen);
}

/* */
static int P_UDP_ListenSingle (const char *host, int port, int sock_family, int protocol)
{
	struct addrinfo hints, *pRes;

    VK_memset  (&hints, 0, sizeof( hints ));
	hints.ai_family = sock_family;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (host && !*host)
	{
        host = NULL;
	}

    PrintDebug ("%s (%d) net: opening %s datagram port %d\n",__FUNCTION__,__LINE__, host ? host : "any", port);

	int val = util_getaddrinfo (host, port, &hints, &pRes);
    if (val)
    {
        PrintError ("%s (%d) Cannot resolve %s port %d : %s\n",__FUNCTION__,__LINE__, host, port, gai_strerror (val));
        return -1;
    }

    val = -1;

	for (const struct addrinfo *ptr = pRes; ptr != NULL; ptr = ptr->ai_next)
    {
		int nfd = net_Socket (ptr->ai_family, ptr->ai_socktype, protocol ? protocol : ptr->ai_protocol);

		if (nfd == -1)
        {
            PrintError ("%s (%d) Error> socket error\n",__FUNCTION__,__LINE__); /* ISO C does not support the '%m' VK_printf format */
            continue;
        }
		/* else { continue; } */

        if (ptr->ai_next != NULL)
        {
#if defined(SUPPORT_IPV6_TRANSPORT)
#ifdef IPV6_V6ONLY
            if ((ptr->ai_family != AF_INET6)
			 || setsockopt (nfd, SOL_IPV6, IPV6_V6ONLY, &(int){ 0 },
                            sizeof (int)))
            {
                PrintError ("%s (%d) Error> Multiple network protocols present\n",__FUNCTION__,__LINE__);
                PrintError ("%s (%d) Error> Please select network protocol manually\n",__FUNCTION__,__LINE__);
            }
#endif
#endif
        }

		nfd = net_SetupDgramSocket( nfd, ptr );
		if( nfd == -1 )
		{
            continue;
		}
		/* else { continue; } */

		if (net_SockAddrIsMulticast (ptr->ai_addr, ptr->ai_addrlen) && net_Subscribe (nfd, ptr->ai_addr, ptr->ai_addrlen))
        {
			net_Close (nfd);
            continue;
        }
		/* else { continue; } */

		val = nfd;
        break;
    }

	util_freeaddrinfo (pRes);
    return val;
}


static int P_UDP_SetMcastHopLimit( int nfd, int sock_family, int hoplim )
{
	unsigned char buf;
	int iProto, cmd;

	switch( sock_family )
    {
#if defined(IP_MULTICAST_TTL)
        case AF_INET:
			iProto = SOL_IP;
            cmd = IP_MULTICAST_TTL;
            break;
#endif

#if defined(SUPPORT_IPV6_TRANSPORT)
#if defined(IPV6_MULTICAST_HOPS)
        case AF_INET6:
			iProto = SOL_IPV6;
            cmd = IPV6_MULTICAST_HOPS;
            break;
#endif
#endif

        default:
            errno = EAFNOSUPPORT;
            PrintError( "%s (%d) Error> Unknown socket family \n",__FUNCTION__,__LINE__ );
            return DI_IPT_EGENERIC;
    }

	if( setsockopt( nfd, iProto, cmd, &hoplim, sizeof( hoplim ) ) < 0 )
    {
		buf = (unsigned char)(( hoplim > 255 ) ? 255 : hoplim);
		if( setsockopt( nfd, iProto, cmd, &buf, sizeof( buf ) ) )
            return DI_IPT_EGENERIC;
    }

    return DI_IPT_SUCCESS;
}


#if 0
static int net_SetMcastOutIface (int nfd, int sock_family, int scope)
{
	switch (sock_family)
    {
#if defined(SUPPORT_IPV6_TRANSPORT)
#if defined(IPV6_MULTICAST_IF)
        case AF_INET6:
			return setsockopt (nfd, SOL_IPV6, IPV6_MULTICAST_IF,
                               &scope, sizeof (scope));
#endif
#endif

#ifdef __linux__
        case AF_INET:
        {
            struct ip_mreqn req = { .imr_ifindex = scope };

			return setsockopt (nfd, SOL_IP, IP_MULTICAST_IF, &req,
                               sizeof (req));
        }
#endif
    }

    errno = EAFNOSUPPORT;
    return -1;
}
#endif


static inline int P_UDP_SetMcastOutIPv4 (int nfd, struct in_addr ipv4)
{
#if defined(IP_MULTICAST_IF)
	return setsockopt( nfd, SOL_IP, IP_MULTICAST_IF, &ipv4, sizeof (ipv4));
#else
    errno = EAFNOSUPPORT;
    return -1;
#endif
}


#if 0
static int P_UDP_SetMcastOut (int nfd, int sock_family, const char *piface, const char *addr)
{
	if (piface != NULL)
    {
		int scope = if_nametoindex (piface);
        if (scope == 0)
        {
			PrintError ("invalid multicast interface: %s\n", piface);
            return -1;
        }

		if (net_SetMcastOutIface (nfd, sock_family, scope) == 0)
		{
            return 0;
		}

		PrintError ("%s (%d) Error> %s\n",__FUNCTION__,__LINE__, piface); /* ISO C does not support the '%m' VK_printf format */
    }

    if (addr != NULL)
    {
		if (sock_family == AF_INET)
        {
            struct in_addr ipv4;
            if (inet_pton (AF_INET, addr, &ipv4) <= 0)
            {
                PrintError ("invalid IPv4 address for multicast: %s\n",
                         addr);
                return -1;
            }

			if (P_UDP_SetMcastOutIPv4 (nfd, ipv4) == 0)
			{
                return 0;
			}

            PrintError ("%s (%d) Error> %s\n",__FUNCTION__,__LINE__, addr); /* ISO C does not support the '%m' VK_printf format */
        }
    }

    return -1;
}
#endif


int net_ConnectDgram( const char *pszHost, int iPort, int iHopLim, int iProto )
{
	struct addrinfo hints, *pRes, *ptr;
	int			 iVal, iHandle = -1;
	HBOOL	  bUnreach = FALSE;
	int	nfd = 0;

#if 0 // input TTL TODO;
	if( iHopLim < 1 )
		iHopLim = var_CreateGetInteger( "ttl" );
#endif

	VK_memset ( &hints, 0, sizeof( hints ) );
	hints.ai_socktype = SOCK_DGRAM;

	PrintDebug( "net: connecting to [%s]:%d\n", pszHost, iPort );

	iVal = util_getaddrinfo( pszHost, iPort, &hints, &pRes );

	if( iVal )
	{
		PrintError( "cannot resolve [%s]:%d : %s\n", pszHost, iPort, gai_strerror( iVal ) );
		return -1;
	}

	for( ptr = pRes; ptr != NULL; ptr = ptr->ai_next )
	{
		/* Fixed prevent-CID-15514: Resource leak (RESOURCE_LEAK) */
		/* http://10.0.0.92:5467/sourcebrowser.htm?projectId=10014#mergedDefectId=15514&defectInstanceId=269911&fileInstanceId=533840 */
		nfd = net_Socket (ptr->ai_family, ptr->ai_socktype, iProto ?iProto:ptr->ai_protocol);

		if (nfd == -1)
		{
			continue;
		}

#if !defined( SYS_BEOS )
		/*
		 * Increase the receive buffer size
		 *   to 1/2MB (8Mb/s during 1/2s) to avoid packet loss
		 *   caused by scheduling problems
 */
		setsockopt (nfd, SOL_SOCKET, SO_RCVBUF, &(int){ 0x80000 }, sizeof (int));
		setsockopt (nfd, SOL_SOCKET, SO_SNDBUF, &(int){ 0x80000 }, sizeof (int));

		/* Allow broadcast sending */
		setsockopt (nfd, SOL_SOCKET, SO_BROADCAST, &(int){ 1 }, sizeof (int));
#endif

		if( iHopLim > 0 )
		{
			P_UDP_SetMcastHopLimit( nfd, ptr->ai_family, iHopLim );
		}

#if 0 // junsy :input interface addr TODO;
		str = var_CreateGetNonEmptyString ("miface");
		if (str != NULL)
		{
			P_UDP_SetMcastOut (nfd, ptr->ai_family, str, NULL);
			DD_MEM_Free (str);
		}

		str = var_CreateGetNonEmptyString ("miface-addr");
		if (str != NULL)
		{
			P_UDP_SetMcastOut (nfd, ptr->ai_family, NULL, str);
			DD_MEM_Free (str);
		}
#endif

#if 0 // junsy input DSCP value
		P_UDP_SetDSCP (nfd, var_CreateGetInteger ("dscp"));
#endif

		if( connect( nfd, ptr->ai_addr, ptr->ai_addrlen ) == 0 )
		{
			/* success */
			iHandle = nfd;
			break;
		}

		if( errno == ENETUNREACH )
		{
			bUnreach = TRUE;
		}
		else
		{
			PrintError( "%s (%d) Error> %s port %d \n",__FUNCTION__,__LINE__, pszHost, iPort);
			net_Close( nfd );
			continue;
		}
	}

	util_freeaddrinfo( pRes );

	if( iHandle == -1 )
	{
		if( bUnreach )
		{
			PrintError( "%s (%d) Error> Host %s port %d is unreachable\n",__FUNCTION__,__LINE__, pszHost, iPort );
		}
		return -1;
	}

	return iHandle;
}

int net_ConnectUDP (const char *host, int port, int hoplim)
{
	return net_ConnectDgram (host, port, hoplim, IPPROTO_UDP);
}

static int P_UDP_IPv4Join (int nfd, const struct sockaddr_in *pSrcAddr, const struct sockaddr_in *pGroupAddr)
{
#ifdef IP_ADD_MEMBERSHIP
    union
    {
        struct ip_mreq gr4;
# ifdef IP_ADD_SOURCE_MEMBERSHIP
        struct ip_mreq_source gsr4;
# endif
    } opt;
    int cmd;
    struct in_addr id = { .s_addr = INADDR_ANY };
	socklen_t iOptlen;

    /* Multicast interface IPv4 address */
#if 0 // junsy
	char *piface = var_CreateGetNonEmptyString ("miface-addr");
	if ((piface != NULL)
	 && (inet_pton (AF_INET, piface, &id) <= 0))
    {
		PrintError ("invalid multicast interface address %s\n", piface);
		DD_MEM_Free (piface);
        goto error;
    }
	DD_MEM_Free (piface);
#endif

    VK_memset  (&opt, 0, sizeof (opt));
	if (pSrcAddr != NULL)
    {
# ifdef IP_ADD_SOURCE_MEMBERSHIP
        cmd = IP_ADD_SOURCE_MEMBERSHIP;
		opt.gsr4.imr_multiaddr = pGroupAddr->sin_addr;
		opt.gsr4.imr_sourceaddr = pSrcAddr->sin_addr;
        opt.gsr4.imr_interface = id;
		iOptlen = sizeof (opt.gsr4);
# else
        errno = ENOSYS;
        goto error;
# endif
    }
    else
    {
        cmd = IP_ADD_MEMBERSHIP;
		opt.gr4.imr_multiaddr = pGroupAddr->sin_addr;
        opt.gr4.imr_interface = id;
		iOptlen = sizeof (opt.gr4);
    }

    PrintDebug ("IP_ADD_%sMEMBERSHIP multicast request\n",
			 (pSrcAddr != NULL) ? "SOURCE_" : "");

	if (setsockopt (nfd, SOL_IP, cmd, &opt, iOptlen) == 0)
	{
        return 0;
	}

# ifndef IP_ADD_SOURCE_MEMBERSHIP
error:
#endif

#endif

    PrintError ("%s (%d) Error> cannot join IPv4 multicast group\n",__FUNCTION__,__LINE__); /* ISO C does not support the '%m' VK_printf format */
    return -1;
}

#if 0 /* defined but not used */
static int P_UDP_IPv6Join (int nfd, const struct sockaddr_in6 *pSrcAddr)
{
#if defined(SUPPORT_IPV6_TRANSPORT)
#ifdef IPV6_JOIN_GROUP
    struct ipv6_mreq gr6;
    VK_memset  (&gr6, 0, sizeof (gr6));
	gr6.ipv6mr_interface = pSrcAddr->sin6_scope_id;
	VK_memcpy (&gr6.ipv6mr_multiaddr, &pSrcAddr->sin6_addr, 16);

    PrintDebug ("%s (%d) IPV6_JOIN_GROUP multicast request\n",__FUNCTION__,__LINE__);

	if (!setsockopt (nfd, SOL_IPV6, IPV6_JOIN_GROUP, &gr6, sizeof (gr6)))
	{
        return 0;
	}
#else
	UNUSED_PARAM(pSrcAddr);
	UNUSED_PARAM(nfd);
    errno = ENOSYS;
#endif /* IPV6_JOIN_GROUP */
#else
	UNUSED_PARAM(pSrcAddr);
	UNUSED_PARAM(nfd);
    errno = ENOSYS;
#endif

    PrintError ("%s (%d) Error> cannot join IPv6 any-source multicast group \n",__FUNCTION__,__LINE__);	/* ISO C does not support the '%m' VK_printf format */
    return -1;
}
#endif /* defined but not used */


static int net_SourceSubscribe (int nfd, const struct sockaddr *pSrcAddr, socklen_t srcAddrLen,
					 const struct sockaddr *pGroupAddr, socklen_t groupAddrLen)
{
    int level, iid = 0;

#if 0 // input multicast address TODO;
	char *piface = var_CreateGetNonEmptyString ("miface");
#else
	char *piface = strdup("224.0.0.100"); // for test address
	if (piface != NULL)
    {
		iid = if_nametoindex (piface);
        if (iid == 0)
        {
			PrintError ("invalid multicast interface: %s\n", piface);
			DD_MEM_Free (piface);
            return -1;
        }
		DD_MEM_Free (piface);
    }
#endif

	switch (pGroupAddr->sa_family)
    {
#if defined(SUPPORT_IPV6_TRANSPORT)
#if defined(AF_INET6)
        case AF_INET6:
            level = SOL_IPV6;
			if (((const struct sockaddr_in6 *)pGroupAddr)->sin6_scope_id)
				iid = ((const struct sockaddr_in6 *)pGroupAddr)->sin6_scope_id;
            break;
#endif
#endif

        case AF_INET:
            level = SOL_IP;
            break;

        default:
            errno = EAFNOSUPPORT;
            return -1;
    }

	if (pSrcAddr != NULL)
	{
		switch (pSrcAddr->sa_family)
		{
#if defined(SUPPORT_IPV6_TRANSPORT)
#if defined(AF_INET6)
			case AF_INET6:
				if (VK_memcmp (&((const struct sockaddr_in6 *)pSrcAddr)->sin6_addr, &in6addr_any, sizeof (in6addr_any)) == 0)
				{
					pSrcAddr = NULL;
				}
				break;
#endif
#endif

			case AF_INET:
				if (((const struct sockaddr_in *)pSrcAddr)->sin_addr.s_addr == INADDR_ANY)
				{
					pSrcAddr = NULL;
				}
				break;
		}
	}

#if defined(MCAST_JOIN_SOURCE_GROUP)
    union
    {
        struct group_req gr;
        struct group_source_req gsr;
    } opt;

	socklen_t iOptlen;

    VK_memset  (&opt, 0, sizeof (opt));

	if (pSrcAddr != NULL)
    {
		if ((groupAddrLen > sizeof (opt.gsr.gsr_group))
		 || (srcAddrLen > sizeof (opt.gsr.gsr_source)))
		{
            return -1;
		}

        opt.gsr.gsr_interface = iid;
		VK_memcpy (&opt.gsr.gsr_source, pSrcAddr, srcAddrLen);
		VK_memcpy (&opt.gsr.gsr_group,  pGroupAddr, groupAddrLen);
		iOptlen = sizeof (opt.gsr);
    }
    else
    {
		if (groupAddrLen > sizeof (opt.gr.gr_group))
		{
            return -1;
		}

        opt.gr.gr_interface = iid;
		VK_memcpy (&opt.gr.gr_group, pGroupAddr, groupAddrLen);
		iOptlen = sizeof (opt.gr);
    }

	PrintDebug ("Multicast %sgroup join request\n", pSrcAddr ? "source " : "");

	if (setsockopt (nfd, level, pSrcAddr ? MCAST_JOIN_SOURCE_GROUP : MCAST_JOIN_GROUP, (void *)&opt, iOptlen) == 0)
	{
        return 0;
	}
#endif

    /* Fallback to IPv-specific APIs */
	if ((pSrcAddr != NULL) && (pSrcAddr->sa_family != pGroupAddr->sa_family))
	{
        return -1;
	}

	switch (pGroupAddr->sa_family)
	{
		case AF_INET:
			if ((groupAddrLen < sizeof (struct sockaddr_in)) || ((pSrcAddr != NULL) && (srcAddrLen < sizeof (struct sockaddr_in))))
			{
				return -1;
			}

			if (P_UDP_IPv4Join (nfd, (const struct sockaddr_in *)pSrcAddr,
						(const struct sockaddr_in *)pGroupAddr) == 0)
			{
				return 0;
			}
			break;

#if defined(SUPPORT_IPV6_TRANSPORT)
#if defined(AF_INET6)
		case AF_INET6:
			if ((groupAddrLen < sizeof (struct sockaddr_in6))
					|| ((pSrcAddr != NULL) && (srcAddrLen < sizeof (struct sockaddr_in6))))
			{
				return -1;
			}

			/* IPv6-specific SSM API does not exist. So if we're here
			 * it means IPv6 SSM is not supported on this OS and we
			 * directly fallback to ASM */

			if (P_UDP_IPv6Join (nfd, (const struct sockaddr_in6 *)pGroupAddr) == 0)
			{
				return 0;
			}
			break;
#endif
#endif
	}

    PrintError ("%s (%d) Error> Multicast group join error\n",__FUNCTION__,__LINE__); /* ISO C does not support the '%m' VK_printf format */

	if (pSrcAddr != NULL)
    {
        PrintError ("%s (%d) Error> Trying ASM instead of SSM...\n",__FUNCTION__,__LINE__);
		return net_Subscribe (nfd, pGroupAddr, groupAddrLen);
    }

    PrintError ("%s (%d) Error> Multicast not supported\n",__FUNCTION__,__LINE__);
    return -1;
}


int net_OpenDgram ( const char *pszBind, int iBind,
					 const char *pszServer, int iServer,
					 int sock_family, int protocol )
		{
	if ((pszServer == NULL) || (pszServer[0] == '\0'))
		{
		return P_UDP_ListenSingle (pszBind, iBind, sock_family, protocol);
	}

	PrintDebug ("net: connecting to [%s]:%d from [%s]:%d\n", pszServer, iServer, pszBind, iBind);

    struct addrinfo hints, *loc, *rem;
    struct addrinfo *ptr;
    struct addrinfo *ptr2;
    int val;

    VK_memset  (&hints, 0, sizeof (hints));
	hints.ai_family = sock_family;
    hints.ai_socktype = SOCK_DGRAM;

	val = util_getaddrinfo (pszServer, iServer, &hints, &rem);
    if (val)
    {
		PrintError ("cannot resolve %s port %d : %s\n", pszBind, iBind, gai_strerror (val));
        return -1;
    }

    hints.ai_flags = AI_PASSIVE;
	val = util_getaddrinfo (pszBind, iBind, &hints, &loc);
    if (val)
    {
		PrintError ("cannot resolve %s port %d : %s\n", pszBind, iBind, gai_strerror (val));
        util_freeaddrinfo (rem);
        return -1;
    }

    for (ptr = loc; ptr != NULL; ptr = ptr->ai_next)
    {
		int nfd = net_Socket (ptr->ai_family, ptr->ai_socktype, protocol ?protocol: ptr->ai_protocol);

		if (nfd == -1)
		{
            continue; // usually, address family not supported
		}

		nfd = net_SetupDgramSocket( nfd, ptr );
		if( nfd == -1 )
		{
            continue;
		}

        val = -1;
        for (ptr2 = rem; ptr2 != NULL; ptr2 = ptr2->ai_next)
        {
            if ((ptr2->ai_family != ptr->ai_family)
             || (ptr2->ai_socktype != ptr->ai_socktype)
             || (ptr2->ai_protocol != ptr->ai_protocol))
			{
                continue;
			}

            if (net_SockAddrIsMulticast (ptr->ai_addr, ptr->ai_addrlen)
			  ? net_SourceSubscribe (nfd, ptr2->ai_addr, ptr2->ai_addrlen,
                                     ptr->ai_addr, ptr->ai_addrlen)
			  : connect (nfd, ptr2->ai_addr, ptr2->ai_addrlen))
            {
				PrintError ("%s (%d) Error> cannot connect to %s port %d\n",__FUNCTION__,__LINE__, pszServer, iServer); /* ISO C does not support the '%m' VK_printf format */
                continue;
            }
			val = nfd;
            break;
        }

        if (val != -1)
		{
            break;
		}

		net_Close (nfd);
    }

    util_freeaddrinfo (rem);
    util_freeaddrinfo (loc);
    return val;
}

int net_ListenUDP1 (const char *host, int port)
{
    return net_OpenDgram (host, port, NULL, 0, 0, IPPROTO_UDP);
}

int net_SetCSCov (int nfd, int sendcov, int recvcov)
{
    int type;

	if (getsockopt (nfd, SOL_SOCKET, SO_TYPE, &type, &(socklen_t){ sizeof (type) }))
	{
        return DI_IPT_EGENERIC;
	}

    switch (type)
    {
#ifdef UDPLITE_RECV_CSCOV
        case SOCK_DGRAM: /* UDP-Lite */
            if (sendcov == -1)
			{
                sendcov = 0;
			}
            else
			{
                sendcov += 8; /* partial */
			}
			if (setsockopt (nfd, SOL_UDPLITE, UDPLITE_SEND_CSCOV, &sendcov, sizeof (sendcov)))
			{
                return DI_IPT_EGENERIC;
			}

            if (recvcov == -1)
			{
                recvcov = 0;
			}
            else
			{
                recvcov += 8;
			}
			if (setsockopt (nfd, SOL_UDPLITE, UDPLITE_RECV_CSCOV, &recvcov, sizeof (recvcov)))
			{
                return DI_IPT_EGENERIC;
			}

            return DI_IPT_SUCCESS;
#endif
#ifdef DCCP_SOCKOPT_SEND_CSCOV
        case SOCK_DCCP: /* DCCP and its ill-named socket type */
            if ((sendcov == -1) || (sendcov > 56))
			{
                sendcov = 0;
			}
            else
			{
                sendcov = (sendcov + 3) / 4;
			}

			if (setsockopt (nfd, SOL_DCCP, DCCP_SOCKOPT_SEND_CSCOV, &sendcov, sizeof (sendcov)))
			{
                return DI_IPT_EGENERIC;
			}

            if ((recvcov == -1) || (recvcov > 56))
			{
                recvcov = 0;
			}
            else
			{
                recvcov = (recvcov + 3) / 4;
			}
			if (setsockopt (nfd, SOL_DCCP, DCCP_SOCKOPT_RECV_CSCOV, &recvcov, sizeof (recvcov)))
			{
                return DI_IPT_EGENERIC;
			}

            return DI_IPT_SUCCESS;
#endif
    }
#if !defined( UDPLITE_RECV_CSCOV ) && !defined( DCCP_SOCKOPT_SEND_CSCOV )
    UNUSED_PARAM (sendcov);
    UNUSED_PARAM (recvcov);
#endif

    return DI_IPT_EGENERIC;
}
