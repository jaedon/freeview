/**************************************************************
 *	@file		remoteapp_udp.h
 *	RemoteApp Server UDP Socket API for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/17
 *	@author			humax
 *	@brief			Remote App UDP Socket API Header
 ***************************************************************/

#ifndef __REMOTEAPP_UDP_H__

#define __REMOTEAPP_UDP_H__

#define REMOTEAPP_SERVER_PORT	22558

HERROR	remoteapp_socket_open(HUINT8 in_mode, HINT32 *out_fd, HBOOL isNonBlock);

HERROR	remoteapp_socket_close(HINT32	in_fd);


HERROR	remoteapp_socket_bind(HINT32 in_fd);

HERROR	remoteapp_socket_recvfrom(HINT32 in_fd, HINT32 in_len, HINT8 *out_buf, HINT32 *out_addr, HINT32 *out_port
					, HINT32 *out_len);

HERROR	remoteapp_socket_sendto(HINT32 in_fd, HINT8 *in_buf, HINT32 in_len, HINT32 in_addr, HINT32 in_port
					, HINT32 *out_len);

#endif
