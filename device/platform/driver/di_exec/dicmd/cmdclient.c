#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>


#define MAX_BUFFER_SIZE 512

unsigned int portno = 3131;
char hostname[] = "127.0.0.1";

int main(int argc, char *argv[])
{
	int sfd;
	struct sockaddr_in saddrin;
	struct hostent *host = gethostbyname(hostname);
	char buf[MAX_BUFFER_SIZE];

	memset(&saddrin, 0, sizeof(struct sockaddr_in));
	
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
	{
		perror("cannot open socket");
		exit(1);
	}
	
	memcpy(&saddrin.sin_addr.s_addr, host->h_addr, host->h_length);
	saddrin.sin_family = AF_INET;
	saddrin.sin_port = htons(portno);

	if (connect(sfd, (struct sockaddr *)&saddrin, sizeof(saddrin)) < 0)
	{
		perror("connecting");
		exit(1);
	}

	write(sfd, "\n", strlen("\n"));
	while(1)
	{
		fgets(buf, MAX_BUFFER_SIZE, stdin);
		buf[MAX_BUFFER_SIZE-1]='\0';
		if(strstr(buf,"quit")!=NULL) break;
		write(sfd, buf, strlen(buf));
	}
	close(sfd);
	return 0;
}

