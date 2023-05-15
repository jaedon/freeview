/**************************************************************
 *	@file		remoteapp_main.c
 *	RemoteApp Client Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/25
 *	@author			humax
 *	@brief			Remote App Client
 ***************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include 	<sys/ioctl.h>
#include	<errno.h>

int socket_open(int *out_fd)
{
	int sock;
	HxLOG_Print("%s() +\n", __FUNCTION__);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	//sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		HxLOG_Print("%s():%d \n", __FUNCTION__, __LINE__);
	}

	*out_fd= sock;

	HxLOG_Print("%s() -\n", __FUNCTION__);

	return 0;
}

int socket_close(int in_fd)
{
	int e;

	HxLOG_Print("%s() +\n", __FUNCTION__);
	e = close(in_fd);
	if (e!=0)
	{
		HxLOG_Print("%s():%d errno=%d\n", __FUNCTION__, __LINE__, errno);
	}

	HxLOG_Print("%s() -\n", __FUNCTION__);
	return 0;
}

int socket_sendto(int in_fd, int in_addr, int in_port)
{
	struct sockaddr_in 	sin;
	int			in_buf=0;
	int			sLen = 0;

	HxLOG_Print("%s() +\n", __FUNCTION__);

	bzero((char*)&sin, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = in_addr;
	sin.sin_port = in_port;

	sLen = sendto(in_fd, (char*)&in_buf, sizeof(in_buf), 0, (struct sockaddr*)&sin, sizeof(sin));

	if (sLen < 0)
	{
		HxLOG_Print("%s():%d errno=%d\n", __FUNCTION__, __LINE__, errno);
	}

	HxLOG_Print("%s() -\n", __FUNCTION__);
	return 0;
}

int	main(int argc, char **argv)
{
	int fd = 0;
	HxLOG_Print("%s() serverip=%s port=%d+\n", __FUNCTION__, argv[1], atoi(argv[2]));
	socket_open(&fd);

	socket_sendto(fd, inet_addr(argv[1]), htons(atoi(argv[2])));

	socket_close(fd);

	HxLOG_Print("%s() -\n", __FUNCTION__);
	return 0;
}

