/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   transport_tcp.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     Transport Layer (TCP) Body
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

#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

#include "htype.h"
#include "di_uart.h"
#include "vkernel.h"
#include "transport_io.h"
#include "transport_tcp.h"
#include "di_iptuner_common.h"
#include "util_getaddrinfo.h"
#include "di_iptuner_config.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define TRANSPORT_TCP_DEBUG
//#define TRANSPORT_TCP_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */
#ifdef TRANSPORT_TCP_DEBUG
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

#ifdef TRANSPORT_TCP_CRITICAL_DEBUG
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
//extern int errno;
/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/

static int P_SOCK_Negotiate ( int nfd ,int iVersion ,const char *pszUser ,const char *pszPasswd )
{
	HUINT8 buf[128+2*256];
    int i_len;
    HBOOL b_auth = FALSE;

	if( iVersion != 5 )
	{
        return DI_IPT_SUCCESS;
	}

    /* We negotiate authentication */
	if( ( pszUser == NULL ) && ( pszPasswd == NULL ) )
	{
        b_auth = TRUE;
	}

	buf[0] = iVersion;	/* SOCKS version */
    if( b_auth )
    {
		buf[1] = 2;				  /* Number of methods */
		buf[2] = 0x00;			   /* - No auth required */
		buf[3] = 0x02;			   /* - USer/Password */
        i_len = 4;
    }
    else
    {
		buf[1] = 1;				  /* Number of methods */
		buf[2] = 0x00;			   /* - No auth required */
        i_len = 3;
    }

	if( net_Write( nfd, NULL, buf, i_len ) != i_len )
	{
        return DI_IPT_EGENERIC;
	}
	if( net_Read( nfd, NULL, buf, 2, TRUE ) != 2 )
	{
        return DI_IPT_EGENERIC;
	}

	PrintDebug ( "%s(%d) socks: v=%d method=%x\n",__FUNCTION__,__LINE__, buf[0], buf[1] );

	if( buf[1] == 0x00 )
    {
        PrintDebug ( "%s(%d) socks: no authentication required\n",__FUNCTION__,__LINE__ );
    }
	else if( buf[1] == 0x02 )
    {
		int i_len1 = (int)__MIN( VK_strlen (pszUser), 255 );
		int i_len2 = (int)__MIN( VK_strlen (pszPasswd), 255 );
        PrintDebug ( "%s(%d) socks: username/password authentication\n",__FUNCTION__,__LINE__ );

        /* XXX: we don't support user/pwd > 255 (truncated)*/
		buf[0] = iVersion;		/* Version */
		buf[1] = i_len1;				 /* User length */
		VK_memcpy( &buf[2], pszUser, i_len1 );
		buf[2+i_len1] = i_len2;		  /* Password length */
		VK_memcpy( &buf[2+i_len1+1], pszPasswd, i_len2 );

        i_len = 3 + i_len1 + i_len2;

		if( net_Write( nfd, NULL, buf, i_len ) != i_len )
		{
            return DI_IPT_EGENERIC;
		}

		if( net_Read( nfd, NULL, buf, 2, TRUE ) != 2 )
		{
            return DI_IPT_EGENERIC;
		}

		PrintDebug ( "%s (%d) socks: v=%d status=%x\n",__FUNCTION__,__LINE__, buf[0], buf[1] );
		if( buf[1] != 0x00 )
        {
            PrintError ( "socks: authentication rejected\n" );
            return DI_IPT_EGENERIC;
        }
    }
    else
    {
        if( b_auth )
		{
			PrintError ( "%s (%d) Error> socks: unsupported authentication method %x\n",__FUNCTION__,__LINE__, buf[0] );
		}
        else
		{
            PrintError ( "%s (%d) Error> socks: authentification needed\n",__FUNCTION__,__LINE__ );
		}
        return DI_IPT_EGENERIC;
    }

    return DI_IPT_SUCCESS;
}

/*
 * (RFC 1928)
 */
static int P_SOCK_HandshakeTCP ( int nfd, int iVersion,
					const char *pszUser, const char *pszPasswd,
					const char *pszHost, int iPort )
{
	HUINT8 buf[128+2*256];

	if( iVersion != 4 && iVersion != 5 )
    {
		PrintError ( "%s (%d) Error> invalid socks protocol version %d\n",__FUNCTION__,__LINE__, iVersion );
		iVersion = 5;
    }

	if( iVersion == 5 &&
		P_SOCK_Negotiate( nfd, iVersion,
						pszUser, pszPasswd ) )
        return DI_IPT_EGENERIC;

	if( iVersion == 4 )
    {
        struct addrinfo hints, *p_res;

        /* v4 only support ipv4 */
        VK_memset  (&hints, 0, sizeof (hints));
        hints.ai_family = AF_INET;

		if( util_getaddrinfo( pszHost, 0, &hints, &p_res ) )
		{
            return DI_IPT_EGENERIC;
		}

		buf[0] = iVersion;
		buf[1] = 0x01;			   /* CONNECT */
		SetWordBE( &buf[2], iPort );   /* Port */

		VK_memcpy( &buf[4],			 /* Address */
                &((struct sockaddr_in *)(p_res->ai_addr))->sin_addr, 4 );

        util_freeaddrinfo( p_res );

		buf[8] = 0;				  /* Empty user id */

		if ( net_Write( nfd, NULL, buf, 9 ) != 9 )
		{
            return DI_IPT_EGENERIC;
		}

		if ( net_Read( nfd, NULL, buf, 8, TRUE ) != 8 )
		{
            return DI_IPT_EGENERIC;
		}

		PrintDebug ( "%s (%d) socks: v=%d cd=%d\n",__FUNCTION__,__LINE__, buf[0], buf[1] );

		if( buf[1] != 90 )
		{
            return DI_IPT_EGENERIC;
		}
    }
	else if( iVersion == 5 )
    {
		int i_hlen = (int)__MIN(VK_strlen (pszHost ), 255);
        int i_len;

		buf[0] = iVersion;	/* Version */
		buf[1] = 0x01;			   /* Cmd: connect */
		buf[2] = 0x00;			   /* Reserved */
		buf[3] = 3;				  /* ATYP: for now domainname */

		buf[4] = i_hlen;
		VK_memcpy( &buf[5], pszHost, i_hlen );
		SetWordBE( &buf[5+i_hlen], iPort );

        i_len = 5 + i_hlen + 2;


		if( net_Write( nfd, NULL, buf, i_len ) != i_len )
		{
            return DI_IPT_EGENERIC;
		}

        /* Read the header */
		if( net_Read( nfd, NULL, buf, 5, TRUE ) != 5 )
		{
            return DI_IPT_EGENERIC;
		}

		PrintDebug ( "%s (%d) socks: v=%d rep=%d atyp=%d\n",__FUNCTION__,__LINE__, buf[0], buf[1], buf[3] );

		if( buf[1] != 0x00 )
        {
            PrintError ( "%s (%d) Error> socks: CONNECT request failed\n",__FUNCTION__,__LINE__ );
            return DI_IPT_EGENERIC;
        }

        /* Read the remaining bytes */
		if( buf[3] == 0x01 )
		{
            i_len = 4-1 + 2;
		}
		else if( buf[3] == 0x03 )
		{
			i_len = buf[4] + 2;
		}
		else if( buf[3] == 0x04 )
		{
            i_len = 16-1+2;
		}
        else
		{
            return DI_IPT_EGENERIC;
		}

		if( net_Read( nfd, NULL, buf, i_len, TRUE ) != i_len )
		{
            return DI_IPT_EGENERIC;
		}
    }

    return DI_IPT_SUCCESS;
}

int net_ConnectTCP (const char *pHost, int iPort)
{
	PrintData("%s(%d) : host(%s), port(%d) , type(%d) \n",__FUNCTION__,__LINE__, pHost, iPort, SOCK_STREAM);
	return net_Connect (pHost, iPort, SOCK_STREAM, IPPROTO_TCP);
}

/*****************************************************************************
 * net_Connect:
 *****************************************************************************
 * Open a network connection.
 * @return socket handler or -1 on error.
 *****************************************************************************/
int net_Connect ( const char *pszHost, int iPort, int sock_type, int sock_proto )
{
	struct addrinfo hints, *pRes, *ptr;
	const char	  *pszRealhost;
	char			*pszSocks;
	int			 iRealPort, iVal, iHandle = -1;
	int				iRetryCount=0;
	int nfd = 0;

	PrintEnter;
	PrintDebug ("%s(%d) : host(%s) port(%d) type(%d) proto(%d)\n",__FUNCTION__,__LINE__, pszHost, iPort, sock_type, sock_proto);

	VK_memset ( &hints, 0, sizeof( hints ) );
	hints.ai_socktype = SOCK_STREAM;

	pszSocks = NULL;// TODO : junsy port 입력사항. var_CreateGetNonEmptyString( p_this, "socks" );
	if( pszSocks != NULL )
	{
		char *psz = VK_strchr( pszSocks, ':' );

		if( psz )
		{
			*psz++ = '\0';
		}

		pszRealhost = pszSocks;
		iRealPort = ( psz != NULL ) ? VK_atoi( psz ) : 1080;
		hints.ai_flags &= ~AI_NUMERICHOST;

		PrintDebug ( "%s(%d) net: connecting to %s port %d (SOCKS) \n" "for %s port %d\n"
					,__FUNCTION__
					,__LINE__
					,pszRealhost
					,iRealPort
					,pszHost
					,iPort );

		/* We only implement TCP with SOCKS */
		switch( sock_type )
		{
			case 0:
				sock_type = SOCK_STREAM;
			case SOCK_STREAM:
				PrintDebug ( "%s(%d) : type is SOCK_STREAM.\n",__FUNCTION__,__LINE__ );
				break;
			default:
				PrintError ( "%s(%d) : Error> Socket type not supported through SOCKS\n",__FUNCTION__,__LINE__ );
				if(pszSocks != NULL)
				{
					DD_MEM_Free ( pszSocks );
					pszSocks = NULL;
				}
				PrintExit;
				return -1;
		}

		switch( sock_proto )
		{
			case 0:
				sock_proto = IPPROTO_TCP;
			case IPPROTO_TCP:
				PrintDebug ( "%s(%d) : proto is IPPROTO_TCP.\n",__FUNCTION__,__LINE__ );
				break;
			default:
				PrintError ( "%s(%d) : Error> Transport not supported through SOCKS\n",__FUNCTION__,__LINE__ );
				if(pszSocks != NULL)
				{
					DD_MEM_Free ( pszSocks );
					pszSocks = NULL;
				}
				PrintExit;
				return -1;
		}
	}
	else
	{
		pszRealhost = pszHost;
		iRealPort = iPort;

		PrintDebug ( "%s(%d) net: connecting to %s port %d\n", __FUNCTION__,__LINE__,pszRealhost, iRealPort );
	}

	iVal = util_getaddrinfo ( pszRealhost, iRealPort, &hints, &pRes );
// 	iVal = getaddrinfo (pszRealhost, iRealPort, &hints, &pRes);

	if(pszSocks != NULL)
	{
		DD_MEM_Free ( pszSocks );
		pszSocks = NULL;
	}

	if( iVal )
	{
		PrintError ( "%s(%d) Error> cannot resolve %s port %d : %s\n",__FUNCTION__,__LINE__, pszRealhost, iRealPort, gai_strerror( iVal ) );
		PrintExit;
		return -1;
	}

	for( ptr = pRes; ptr != NULL; ptr = ptr->ai_next )
	{
		PrintDebug ("%s(%d) : Create Socket type(%d) , proto(%d) \n",__FUNCTION__,__LINE__,sock_type,sock_proto);

		/* Fixed prevent-CID-11978: Resource leak (RESOURCE_LEAK) */
		/* http://10.0.0.92:5467/sourcebrowser.htm?projectId=10014#mergedDefectId=11978&defectInstanceId=269910&fileInstanceId=533838 */
		nfd = net_Socket( ptr->ai_family, sock_type ? sock_type : ptr->ai_socktype, sock_proto ?sock_proto : ptr->ai_protocol );
		if( nfd == -1 )
		{
			PrintDebug ( "%s(%d) : socket error\n",__FUNCTION__,__LINE__ ); /* ISO C does not support the '%m' VK_printf format */
			continue;
		}

#if 0
		if( connect( nfd, ptr->ai_addr, ptr->ai_addrlen ) )
		{
			int timeout;
#if 0
			int val;
#endif

			if( errno != EINPROGRESS && errno != EINTR )
			{
				PrintError ( "%s(%d) : Error> connection failed: %m\n",__FUNCTION__,__LINE__ );
				goto next_ai;
			}
			PrintDebug ( "connection: %m\n" );

#if 0 // junsy TODO: connect timeout value
			timeout = var_CreateGetInteger ("ipv4-timeout");
			if (timeout < 0)
			{
				PrintError ( "invalid negative value for ipv4-timeout\n" );
				timeout = 0;
			}
#else
			timeout = 0;
#endif
		}
#else
		int flags;
		int result;
		flags = fcntl(nfd, F_GETFL, 0);
/*		(void)fcntl(nfd, F_SETFL, flags | O_NONBLOCK);//O_SYNC); */

		PrintData("%s(%d) : ai_flags(0x%x) ai_family(0x%x) ai_socktype(0x%x) ai_protocol(0x%x) ai_addr(%s)\n"
		,__FUNCTION__
		,__LINE__
		, ptr->ai_flags
		, ptr->ai_family
		, ptr->ai_socktype
		, ptr->ai_protocol
		, ptr->ai_addr->sa_data);

		result = connect(nfd, ptr->ai_addr, ptr->ai_addrlen);
		if (result == 0)
		{
//			return nfd;
			break;
		}

		if(result < 0 && (errno != EINPROGRESS))
		{
			PrintError ("%s(%d) Error> Could Not Connect to server\n",__FUNCTION__,__LINE__);

			goto next_ai;
		}
		else
		{
			fd_set		fds_w;
			struct timeval 	time_out;
			HUINT32		error;
			socklen_t len;

reselect :
			FD_ZERO(&fds_w);
			FD_SET(nfd, &fds_w);
			/* Modify Timeout Time 20sec => 20sec */
			time_out.tv_sec = 20;
			time_out.tv_usec = 0;

			PrintDebug ("%s(%d) Error> Waiting Connection \n",__FUNCTION__,__LINE__);

			error = 0;
			len = sizeof(error);
			if ((select(nfd+1, NULL, &fds_w, NULL, &time_out) <= 0))
			{
				PrintError ("%s(%d) Error> Time out or socket Error\n",__FUNCTION__,__LINE__);
				if(iRetryCount == 2)
				{
					goto next_ai;
				}
				else
				{
					iRetryCount++;
					goto reselect;
				}
			}
			else if(!(FD_ISSET(nfd, &fds_w)))
			{
				PrintError ("%s(%d) Error> Known socket\n",__FUNCTION__,__LINE__);
				goto reselect;
			}
			else if (getsockopt(nfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			{
				PrintError ("%s(%d) Error> socket error\n",__FUNCTION__,__LINE__);
				goto next_ai;
			}
			else if(error)
			{
				PrintError ("%s(%d) Error> socket error\n",__FUNCTION__,__LINE__);
				goto next_ai;
			}
		}
#if 0
		if(nfd != 0)
		{
			(void)fcntl(nfd, F_SETFL, flags & ~O_NONBLOCK);
		}
#endif
#endif

		PrintDebug ( "%s(%d) connection succeeded (socket = %d)\n",__FUNCTION__,__LINE__, nfd );
		iHandle = nfd; /* success! */
		break;

next_ai: /* failure */
		net_Close( nfd );
		if(iRetryCount == 3)
		{
			iHandle = -1;
		}
		else
		{
			continue;
		}
	}

	util_freeaddrinfo( pRes );

	if( iHandle == -1 )
	{
		PrintError("%s(%d) Error> iHandle is %d \n",__FUNCTION__,__LINE__,iHandle);
		PrintExit;
		return -1;
	}

	if( pszSocks != NULL )
	{
		/* NOTE: pszSocks already free'd! */
#if 0 // junsy TODO : user passwd input
		char *pszUser = var_CreateGetNonEmptyString( "socks-user" );
		char *psz_pwd  = var_CreateGetNonEmptyString( "socks-pwd" );
#else
		char *pszUser = strdup("test_user");
		char *psz_pwd  = strdup("test_passwd");
#endif

		if( P_SOCK_HandshakeTCP ( iHandle, 5, pszUser, psz_pwd, pszHost, iPort ) )
		{
			PrintError ( "%s(%d) Error> SOCKS handshake failed\n",__FUNCTION__,__LINE__ );
			net_Close( iHandle );
			iHandle = -1;
		}

		if(pszUser != NULL)
		{
			DD_MEM_Free ( pszUser );
			pszUser = NULL;
		}

		if(psz_pwd != NULL)
		{
			DD_MEM_Free ( psz_pwd );
			psz_pwd = NULL;
		}
	}

	PrintExit;
	return iHandle;
}


int net_AcceptSingle (int lfd)
{
	int nfd;

    do
	{
		nfd = accept (lfd, NULL, NULL);
	}
	while (nfd == -1 && errno == EINTR);

	if (nfd == -1)
    {
        if (errno != EAGAIN)
		{
			/* ISO C does not support the '%m' VK_printf format */
            PrintError  ("%s(%d) Error> accept failed (from socket %d)\n",__FUNCTION__,__LINE__, lfd);
		}
        return -1;
    }

	PrintDebug  ("%s(%d) accepted socket %d (from socket %d)\n",__FUNCTION__,__LINE__, nfd, lfd);

	(void) net_SetupSocket (nfd);

	return nfd;
}


/*****************************************************************************
 * __net_Accept:
 *****************************************************************************
 * Accept a connection on a set of listening sockets and return it
 *****************************************************************************/
int net_Accept(int *pFd, HINT64 iWait )
{
	UNUSED_PARAM (pFd);
	UNUSED_PARAM (iWait);

	/* Never used. */
    return -1;
}

void net_ListenClose( int *pFd )
{
	if( pFd != NULL )
    {
        int *pi;

		for( pi = pFd; *pi != -1; pi++ )
		{
            net_Close( *pi );
		}

		DD_MEM_Free ( pFd );
		pFd = NULL;
    }

	return;
}
