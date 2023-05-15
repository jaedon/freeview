/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   transport_io.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:		 Transport Layer (IO) Body
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
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <limits.h>
#include <netdb.h>
#include <sys/time.h>

#include "htype.h"
#include "di_uart.h"
#include "di_iptuner_common.h"
#include "transport_io.h"
#include "vkernel.h"
#include "util_getaddrinfo.h"
#include "transport_udp.h"
#include "di_iptuner_config.h"


#ifdef HAVE_LINUX_DCCP_H
/* TODO: use glibc instead of linux-kernel headers */
# include <linux/dccp.h>
# define SOL_DCCP 269
#endif
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define INADDR_ANY  0x00000000
//#define INADDR_NONE 0xFFFFFFFF
//
//#define TRANSPORT_IO_DEBUG
//#define TRANSPORT_IO_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef TRANSPORT_IO_DEBUG
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

#ifdef TRANSPORT_IO_CRITICAL_DEBUG
#define PrintCriticalDebug	  DI_UART_Print
#else
#define PrintCriticalDebug	  while (0) ((int (*)(char *, ...)) 0)
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

int net_SetupSocket ( int nfd )
{
	int nret=0;
	nret = fcntl (nfd, F_SETFD, FD_CLOEXEC);
	if(nret<0)
	{
		PrintError ("%s (%d) Error> cannot set close-on-exec flag on socket \n",__FUNCTION__,__LINE__);
	}
	nret = fcntl (nfd, F_GETFL, 0);
	if(nret<0)
	{
		PrintError ("%s (%d) Error> cannot get flags on socket\n",__FUNCTION__,__LINE__);
	}
	nret = fcntl (nfd, F_SETFL, nret | O_NONBLOCK);
	if(nret<0)
	{
		PrintError ("%s (%d) Error> cannot set non-blocking flag on socket\n",__FUNCTION__,__LINE__);
	}

	nret = setsockopt (nfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof (int));
	if(nret<0)
	{
		PrintError ("%s (%d) Error> setsockopt fail \n",__FUNCTION__,__LINE__);
	}

	return 0;
}


int net_Close (int nfd)
{
	return close(nfd);
}

int net_Socket (int sock_family, int sock_type, int sock_protocol)
{
#if defined(DCCP_SOCKOPT_SERVICE)
	char pDccps = NULL;
#endif
	int nfd = socket (sock_family, sock_type, sock_protocol);
	if (nfd == -1)
	{
		if (errno != EAFNOSUPPORT)
		{
			PrintError ("%s (%d) Error> cannot create socket \n",__FUNCTION__,__LINE__); /* %m ?? ISO C does not support the '%m' VK_printf format */
		}
		return -1;
	}

	net_SetupSocket (nfd);

#if defined(SUPPORT_IPV6_TRANSPORT)
#if defined(IPV6_V6ONLY)
	/*
	 * Accepts only IPv6 connections on IPv6 sockets.
	 * If possible, we should open two sockets, but it is not always possible.
	 */
	if (sock_family == AF_INET6)
	{
		if(setsockopt (nfd, IPPROTO_IPV6, IPV6_V6ONLY, &(int){ 1 }, sizeof (int)) < 0)
		{
			PrintError("%s (%d) Error> setsockopt failed \n",__FUNCTION__,__LINE__);
		}
	}
#endif
#endif


#if defined(DCCP_SOCKOPT_SERVICE)
	if (sock_type == SOL_DCCP)
	{
		pDccps = var_CreateGetNonEmptyString ("dccp-service");
		if (pDccps != NULL)
		{
			if(setsockopt (nfd, SOL_DCCP, DCCP_SOCKOPT_SERVICE, pDccps,
						(VK_strlen  (pDccps) + 3) & ~3) < 0)
			{
				PrintError("%s (%d) Error> setsockopt failed \n",__FUNCTION__,__LINE__);
			}
			DD_MEM_Free (pDccps);
			pDccps = NULL;
		}
	}
#endif

	return nfd;
}


int *net_Listen ( const char *psz_host, int i_port, int sock_protocol)
{
	struct addrinfo hints, *res;
	int sock_type = SOCK_DGRAM;

	switch( sock_protocol )
	{
		case IPPROTO_TCP:
			sock_type = SOCK_STREAM;
			break;
		case 33: /* DCCP */
#ifdef __linux__
# ifndef SOCK_DCCP
#  define SOCK_DCCP 6
# endif
			sock_type = SOCK_DCCP;
#endif
			break;
	}

	VK_memset  (&hints, 0, sizeof( hints ));
	/* Since we use port numbers rather than service names, the socket type
	 * does not really matter. */
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	PrintDebug ("net: listening to %s port %d", psz_host, i_port);

	int i_val = util_getaddrinfo (psz_host, i_port, &hints, &res);
	if (i_val)
	{
		PrintError ( "%s (%d) Error> Cannot resolve %s port %d : %s",__FUNCTION__,__LINE__, psz_host, i_port, gai_strerror (i_val));
		return NULL;
	}
	/* else { continue ; } */

	int *sockv = NULL;
	unsigned sockc = 0;

	for (struct addrinfo *ptr = res; ptr != NULL; ptr = ptr->ai_next)
	{
		int nfd = net_Socket (ptr->ai_family, sock_type, sock_protocol);
		if (nfd == -1)
		{
			PrintError ( "%s (%d) Error> socket error\n",__FUNCTION__,__LINE__);/*  ISO C does not support the '%m' VK_printf format */
			continue;
		}

		/* Bind the socket */
		if (bind (nfd, ptr->ai_addr, ptr->ai_addrlen))
		{
			net_Close (nfd);
#if !defined(WIN32) && !defined(UNDER_CE)
#if 0 /*xxx : root socket binding ???? */
			nfd = rootwrap_bind (ptr->ai_family, sock_type,
								sock_protocol ?: ptr->ai_protocol, ptr->ai_addr,
								ptr->ai_addrlen);
			if (nfd != -1)
			{
				PrintDebug ("got socket %d from rootwrap", nfd);
			}
			else
#endif
#endif
			{
				PrintError ("%s (%d) Error> socket bind error \n",__FUNCTION__,__LINE__); /*  ISO C does not support the '%m' VK_printf format */
				continue;
			}
		}

		if (net_SockAddrIsMulticast (ptr->ai_addr, ptr->ai_addrlen))
		{
			if (net_Subscribe ( nfd, ptr->ai_addr, ptr->ai_addrlen))
			{
				net_Close (nfd);
				continue;
			}
		}

		/* Listen */
		switch (sock_type)
		{
			case SOCK_STREAM:
			case SOCK_RDM:
			case SOCK_SEQPACKET:
#ifdef SOCK_DCCP
			case SOCK_DCCP:
#endif
				if (listen (nfd, INT_MAX))
				{
					PrintError ("%s (%d) Error> socket listen error \n",__FUNCTION__,__LINE__); /*  ISO C does not support the '%m' VK_printf format */
					net_Close (nfd);
					continue;
				}
		}

#if 1 /* TBD */
		int *nsockv = (int *)DD_MEM_Realloc (sockv, (sockc + 2) * sizeof (int));
#else
		int *nsockv = (int *)DD_MEM_Alloc ((sockc + 2) * sizeof (int));
#endif
		if (nsockv != NULL)
		{
			nsockv[sockc++] = nfd;
			sockv = nsockv;
		}
		else
			net_Close (nfd);
	}

	freeaddrinfo (res);

	if (sockv != NULL)
		sockv[sockc] = -1;

	return sockv;
}


/*****************************************************************************
 * __net_Read:
 *****************************************************************************
 * Reads from a network socket.
 *****************************************************************************/
ssize_t net_Read (int nfd, const struct virtual_socket_t *vs, HUINT8 *restrict p_buf, size_t i_buflen, HBOOL waitall)
{
	int i_ret = DI_IPT_SUCCESS;
	size_t i_total = 0;
	fd_set			fds_r;
	struct timeval 	time_out;
	HUINT32			retErr;
	socklen_t		len=0;

	while (i_buflen > 0)
	{
		ssize_t n=0;

		FD_ZERO(&fds_r);
		FD_SET(nfd, &fds_r);
		time_out.tv_sec = 5;
		time_out.tv_usec = 0;

//		PrintError ("%s(%d) Debug> Waiting Connection \n",__FUNCTION__,__LINE__);

		retErr = 0;
		len = sizeof(retErr);

		if (vs != NULL)
		{
			/*
			 * if virtual socket is created by SSL,
			 * we should check the ssl internal buffer whether it is empty or not before select call.
			 * Even if select() is a common unix idiom, its use with openssl is far from clean
			 * and requires understanding of some subtleties of SSL.
			 *
			 * For understanding the difference between ssl buffer and socket buffer,
			 * Please refer to http://www.rtfm.com/openssl-examples/.
			 */
			if( vs->pf_checkbuf (vs->p_sys))
			{
				n = vs->pf_recv (vs->p_sys, p_buf, i_buflen);
				goto after_read;
			}
		}

		if ((select(nfd+1, &fds_r, NULL, NULL, &time_out) <= 0))
		{
			PrintError ("%s(%d) Error> Time out or socket Error\n",__FUNCTION__,__LINE__);
			i_ret = DI_IPT_ETIMEOUT;
			goto error;
		}
		else if(!(FD_ISSET(nfd, &fds_r)))
		{
			PrintError ("%s(%d) Error> Known socket\n",__FUNCTION__,__LINE__);
			i_ret = DI_IPT_ENOITEM;
			goto error;
		}
		else if (getsockopt(nfd, SOL_SOCKET, SO_ERROR, &retErr, &len) < 0)
		{
			PrintError ("%s(%d) Error> socket error\n",__FUNCTION__,__LINE__);
			i_ret = DI_IPT_EGENERIC;
			goto error;
		}
		else if (retErr)
		{
			PrintError ("%s(%d) Error> socket error [%d]\n",__FUNCTION__,__LINE__,retErr);
			i_ret = DI_IPT_EGENERIC;
			goto error;
		}

		if (vs != NULL)
		{
			n = vs->pf_recv (vs->p_sys, p_buf, i_buflen);
		}
		else
		{
			n = read (nfd, p_buf, i_buflen);
		}

after_read:
		if (n == -1)
		{
			switch (errno)
			{
				case EAGAIN: /* spurious wakeup or no TLS data */
				case EINTR:  /* asynchronous signal */
					continue;
			}
		i_ret = DI_IPT_EGENERIC;
			goto error;
		}

		if (n == 0)
		{
			/* For streams, this means end of file, and there will not be any
			 * further data ever on the stream. For datagram sockets, this
			 * means empty datagram, and there could be more data coming.
			 * However, it makes no sense to set <waitall> with datagrams in the
			 * first place.
			 */
			break; // EOF
		}

		i_total += n;
		p_buf += n;
		i_buflen -= n;

		if (!waitall)
		{
			break;
		}
	}

	return i_total;

error:
	PrintError ("%s (%d) Error> Read error (errno: %d) \n",__FUNCTION__,__LINE__,errno);/*  ISO C does not support the '%m' VK_printf format */
#if 0
silent:
#endif
	return i_ret;
}


/* Write exact amount requested */
ssize_t net_Write( int nfd, const struct virtual_socket_t *p_vs, const HUINT8 *p_data, size_t i_data )
{
	size_t i_total = 0;

	while( i_data > 0 )
	{
		ssize_t val;
		if (p_vs != NULL)
		{
			val = p_vs->pf_send (p_vs->p_sys, p_data, i_data);
		}
		else
		{
#if 1
			val = write (nfd, p_data, i_data);
#else
			struct timeval  time_out;
			fd_set	  wfds;
			int		 status = 0;

			do
			{
				FD_ZERO(&wfds);
				FD_SET(nfd, &wfds);

				time_out.tv_sec = 10;
				time_out.tv_usec = 0; //

				status = select(nfd+1, NULL, &wfds, NULL, &time_out);
#if 0
				if ( TRUE == s_nIsStopReceiving )
				{
					PrintError ("%s (%d) Error>  User stopped receiving !!!\n",__FUNCTION__,__LINE__);
					nTotalLen = -1;
					goto exit_fn;
				}
#endif
				// Timeout
				if(status == 0)
				{
					PrintError ("%s (%d) Error>  TimeOut in download over HTTP !!!\n",__FUNCTION__,__LINE__);
					continue;
				}
				// unknown io
				else if(status > 0 && !FD_ISSET(nfd, &wfds))
				{
					PrintError ("%s (%d) Error>  unexpected data !!!\n",__FUNCTION__,__LINE__);
					continue;
				}
				// error
				else if(status < 0)
				{
					PrintError ("%s (%d) Error>  Network Error in download over HTTP!!!\n",__FUNCTION__,__LINE__);
				}

				val = write (nfd, p_data, i_data);
				if (val < 0 /*&& (errno == ENOSPC || errno == EIO)*/)
				{
					PrintError ("%s (%d) Error>  write error!!\n",__FUNCTION__,__LINE__);
					continue;
				}
				usleep(0);
			} while (!val);
#endif
		}

		if (val == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			PrintError ("%s (%d) Error>  Write error\n",__FUNCTION__,__LINE__);/*  ISO C does not support the '%m' VK_printf format */
			break;
		}

		p_data += val;
		i_data -= val;
		i_total += val;
	}

	if ((i_total > 0) || (i_data == 0))
	{
		return i_total;
	}

	return -1;
}

char *net_Gets(int nfd, const struct virtual_socket_t *p_vs )
{
	char *psz_line = NULL, *ptr = NULL;
	size_t  i_line = 0, i_max = 0;


	for( ;; )
	{
		if( i_line == i_max )
		{
			i_max += 1024;
			psz_line = DD_MEM_Realloc( psz_line, i_max );
			ptr = psz_line + i_line;
		}

		if( net_Read( nfd, p_vs, (HUINT8 *)ptr, 1, TRUE ) != 1 )
		{
			if( i_line == 0 )
			{
				if(psz_line != NULL)
				{
					DD_MEM_Free( psz_line );
					psz_line = NULL;
				}
				return NULL;
			}
			break;
		}

		if ( *ptr == '\n' )
		{
			break;
		}

		i_line++;
		ptr++;
	}

	*ptr-- = '\0';

	if( ( ptr >= psz_line ) && ( *ptr == '\r' ) )
	{
		*ptr = '\0';
	}

	PrintData ("%s(%d) Receive Message\n",__FUNCTION__,__LINE__);
	PrintData ("<== %s",ptr);

	return psz_line;
}

ssize_t net_Printf (int nfd, const struct virtual_socket_t *p_vs, const char *psz_fmt, ... )
{
	int i_ret;
	va_list args;

	va_start( args, psz_fmt );
	i_ret = net_vaPrintf( nfd, p_vs, psz_fmt, args );
	va_end( args );

	return i_ret;
}

ssize_t net_vaPrintf( int nfd, const struct virtual_socket_t *p_vs, const char *psz_fmt, va_list args )
{
	char	*psz;
	int	  i_ret;

	int i_size = vasprintf( &psz, psz_fmt, args );
	if( i_size == -1 ) return -1;

	PrintData("%s(%d) Sending Message \n",__FUNCTION__,__LINE__);
	PrintData("==> %s",psz);
	i_ret = net_Write( nfd, p_vs, (HUINT8 *)psz, i_size ) < i_size ? -1 : i_size;

	if(psz != NULL)
	{
   		VK_free( psz );
   		psz = NULL;
	}

	return i_ret;
}

ssize_t net_Sendmsg (int s, struct msghdr *hdr, int flags)
{
	return sendmsg (s, hdr, flags);
}

ssize_t net_Recvmsg (int s, struct msghdr *hdr, int flags)
{
	return recvmsg (s, hdr, flags);
}

