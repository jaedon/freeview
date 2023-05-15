#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include <vkernel.h>
#include <sys/socket.h>

static int socketFd=-1;

void setSocketFd(int fd)
{
	socketFd = fd;
}

char getchar_net(void)
{
	int ret=0;
	char rdbuf=-1;

	while(1)
	{
		ret = recv(socketFd, &rdbuf,1,0);
		if(ret > 0)
			break;
	}
	return rdbuf;
}

void putchar_net(char buf)
{
	int ret=0;
	ret = send(socketFd, &buf, 1,0);
	if(ret < 0)
		VK_printf("[%d]%s send failed %d\n",__LINE__,__func__,ret);
}
