/**************************************************************
 *	@file		remoteapp_main.c
 *	RemoteApp Server Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/25
 *	@author			humax
 *	@brief			Remote App Receiver
 ***************************************************************/

#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include 	<sys/ioctl.h>

#include	<vkernel.h>
#include	<hlib.h>
#include	<ondk.h>
#include	<oapi.h>
#include	<hapi.h>
#include	<apk.h>

#include 	"remoteapp_udp.h"


HERROR	remoteapp_socket_open(HUINT8 in_mode, HINT32 *out_fd, HBOOL isNonBlock)
{
	HERROR	hErr = ERR_OK;
	HINT32 sock = 0;

	HxLOG_Info("%s(in_mode=%d) +\n", __FUNCTION__, in_mode);

	sock = socket(AF_INET, in_mode, 0);
	if (sock == -1)
	{
		HxLOG_Error("%s() Fail to open socket (errno-%d)\n", __FUNCTION__, errno);
		hErr = errno;
		goto exit;
	}

	if (isNonBlock)
	{
		hErr = ioctl(sock, FIONBIO, (int)&isNonBlock);  /*  Turns on/off non-blocking I/O.  */
		if (hErr != 0)
		{
			HxLOG_Error("Fail to ioctrl for FIONBIO.(fd=%d,errno=%d)\n", sock, errno);
			hErr = errno;
			close(sock);
			goto exit;
		}
	}

	*out_fd = sock;

exit:
	HxLOG_Info("%s() -\n", __FUNCTION__);

	return hErr;
}

HERROR	remoteapp_socket_close(HINT32	in_fd)
{
	HERROR	hErr = 0;

	HxLOG_Info("%s(in_fd=%d) +\n", __FUNCTION__, in_fd);

	hErr = close(in_fd);
	if (hErr != 0)
	{
		HxLOG_Error("Fail to close (fd=%d,errno=%d)\n", in_fd, errno);
		hErr = errno;
	}

	HxLOG_Info("%s() -\n", __FUNCTION__);

	return hErr;
}

HERROR	remoteapp_socket_bind(HINT32 in_fd)
{
	HERROR			hErr = ERR_FAIL;
	struct sockaddr_in 	sin;

	HxLOG_Info("%s() +\n", __FUNCTION__);

	bzero((char*)&sin, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(REMOTEAPP_SERVER_PORT);

	hErr = bind(in_fd, (struct sockaddr *)&sin, sizeof(sin));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("%s() Fail to bind socket (fd=%d, errno=%d)\n", __FUNCTION__, in_fd, errno);
		hErr = errno;
		goto exit;
	}

	hErr = ERR_OK;

exit:
	HxLOG_Info("%s() -\n", __FUNCTION__);
	return hErr;
}

HERROR	remoteapp_socket_recvfrom(HINT32 in_fd, HINT32 in_len, HINT8 *out_buf, HINT32 *out_addr, HINT32 *out_port
					, HINT32 *out_len)
{
	HERROR			hErr = ERR_OK;
	struct	sockaddr_in	stServerAddr;
	HUINT32			uSize = 0;
	HINT32			nReadLen = 0;

	HxLOG_Info("%s(in_fd=%d, in_len=%d, out_buf=%p, out_len=%p) +\n", __FUNCTION__, in_fd, in_len, out_buf, out_len);
	if (out_buf == NULL || out_len == NULL)
	{
		HxLOG_Error("Invalid paramter out_buf=%p, out_len=%p\n", out_buf, out_len);
		hErr = ERR_FAIL;
		goto exit;
	}

	uSize = sizeof(struct sockaddr_in);
	bzero((char*)&stServerAddr, uSize);

	nReadLen = recvfrom(in_fd, out_buf, in_len, 0, (struct sockaddr *)&stServerAddr, &uSize);

	if (nReadLen < 0)
	{
		if (errno == EWOULDBLOCK)
		{
			HxLOG_Message("%s() in_fd=%d EWOULDBLOCK\n", __FUNCTION__, in_fd);
		}
		else
		{
			HxLOG_Error("%s() in_fd=%d, errno=%d\n", __FUNCTION__, in_fd, errno);
		}
		hErr = errno;
		goto exit;
	}

	HxLOG_Info("%s() recvfrom nReadLen=%d, ip=0x%x, port=%d\n", __FUNCTION__, nReadLen
					, stServerAddr.sin_addr.s_addr, stServerAddr.sin_port);

	*out_len = nReadLen;
	if (out_addr != NULL)
	{
		*out_addr = stServerAddr.sin_addr.s_addr; //hton or ntoh??
	}

	if (out_port != NULL)
	{
		*out_port = stServerAddr.sin_port;
	}
exit:
	HxLOG_Info("%s() -\n", __FUNCTION__);
	return hErr;
}

HERROR	remoteapp_socket_sendto(HINT32 in_fd, HINT8 *in_buf, HINT32 in_len, HINT32 in_addr, HINT32 in_port
					, HINT32 *out_len)
{
	HERROR			hErr = 0;
	struct	sockaddr_in	stServerAddr;
	HUINT32			uSize = 0;
	HINT32			nSendLen = 0;

	HxLOG_Info("%s(in_fd=%d, in_len=%d, in_buf=%p, out_len=%p) +\n", __FUNCTION__, in_fd, in_len, in_buf, out_len);
	if (in_buf == NULL || out_len == NULL)
	{
		HxLOG_Error("Invalid paramter in_buf=%p, out_len=%p\n", in_buf, out_len);
		hErr = ERR_FAIL;
		goto exit;
	}

	uSize = sizeof(struct sockaddr_in);
	bzero((char*)&stServerAddr, uSize);

	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_addr.s_addr = in_addr;
	stServerAddr.sin_port = in_port;

	nSendLen = sendto(in_fd, in_buf, in_len, 0, (struct sockaddr *)&stServerAddr, uSize);

	if (nSendLen < 0)
	{
		if (errno == EWOULDBLOCK)
		{
			HxLOG_Message("%s() in_fd=%d EWOULDBLOCK\n", __FUNCTION__, in_fd);
		}
		else
		{
			HxLOG_Error("%s() in_fd=%d, errno=%d\n", __FUNCTION__, in_fd, errno);
		}
		hErr = errno;
		goto exit;
	}

	*out_len = nSendLen;
	hErr = 0;

exit:
	return hErr;
}

