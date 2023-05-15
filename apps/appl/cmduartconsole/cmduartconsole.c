/**************************************************************
 *	@file		cmduartconsole.c
 *
 *	http://www.humaxdigital.com
 **************************************************************/
#include	<arpa/inet.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<termios.h>
#include <pthread.h>
#include	<time.h>
#include	<hlib.h>

void	tasksleep(unsigned long ulTime)
{
	struct timespec	delay;
	struct timespec	rem;
	int	rc;

	delay.tv_sec = ulTime / 1000;
	delay.tv_nsec = (ulTime % 1000) * 1000000;

	for (;;)
	{

		rc = nanosleep(&delay, &rem);
		if (0 != rc)
		{
			if (EINTR == errno)
			{
				delay = rem;
				continue;
			}

			return;
		}
		break;
	}
}

int getch(void)
{
	int ch;
	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;
	buf.c_lflag &= ~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &buf);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}


static void	*key_receiver(void *arg)
{
	char	ch;
	int		sfd = (int)arg;

	while (1)
	{
		ch = (char)getch();
		write(sfd, &ch, 1);
	}
	return NULL;
}

#define	CMDMGR_MAX_INPUT_BUFFER		2048
#define	CMDMGR_INET_ADDR		60001

int		main(int argc, char **argv)
{
	int		sfd;
	struct sockaddr_in	saddrin;
	char	buf[CMDMGR_MAX_INPUT_BUFFER];
	const char*	pszIpAddr;
	int		nPort;
	int		len;
	pthread_t	tid;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
	{
		printf("<--- fail to connect cmddaemon...\n");
		return 0;
	}

	pszIpAddr = (const char *)HLIB_EXTRA_GetIpAddr("cmdconsole");
	nPort     = (int)HLIB_EXTRA_GetPortNo("cmdconsole");

	memset(&saddrin, 0, sizeof(saddrin));
	inet_pton(AF_INET, pszIpAddr, &saddrin.sin_addr);
	saddrin.sin_family = AF_INET;
	saddrin.sin_port   = htons(nPort);
	if (connect(sfd, (struct sockaddr *)&saddrin, sizeof(saddrin)) < 0)
	{
		printf("error to connect server\n");
		exit(1);
	}

	pthread_create(&tid, NULL, key_receiver, (void*)sfd);

	while (1)
	{
		{
			len = recv(sfd, buf, CMDMGR_MAX_INPUT_BUFFER, 0);
			if (len > 0)
			{
				int	i;
				for (i = 0; i < len; i++)
					putchar(buf[i]);

				if(buf[0] == 0x08 || buf[0] == 0x7F)
				{
					putchar(' ');
					putchar(0x08);
				}
			}
			if (len < 0)
				printf("error\n");
		}
	}

	close(sfd);

	return 0;
}

