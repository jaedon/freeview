#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <oapi.h>


#if 0
#define	CMDMGR_MAX_INPUT_BUFFER		2048
#define	CMDMGR_INET_ADDR		60001

static void	*receiver(void *arg)
{
	int		sfd = (int)arg;
	char	buf[CMDMGR_MAX_INPUT_BUFFER];
	int		len;
	char ch;

	ch = 'h'; write(sfd, &ch, 1); 
	ch = 'e'; write(sfd, &ch, 1); 
	ch = 'l'; write(sfd, &ch, 1); 
	ch = 'p'; write(sfd, &ch, 1); 
	ch = 0x0d;	write(sfd, &ch, 1);
	ch = 0x00;	write(sfd, &ch, 1);
/*
	ch = 'o'; write(sfd, &ch, 1); 
	ch = 'b'; write(sfd, &ch, 1); 
	ch = 'a'; write(sfd, &ch, 1); 
	ch = 'm'; write(sfd, &ch, 1); 
	ch = 'a'; write(sfd, &ch, 1); 
	ch = 0x0d;	write(sfd, &ch, 1);
	ch = 0x00;	write(sfd, &ch, 1);
	*/
	ch = 'e'; write(sfd, &ch, 1);
	ch = 'x'; write(sfd, &ch, 1);
	ch = 'i'; write(sfd, &ch, 1);
	ch = 't'; write(sfd, &ch, 1);
	ch = 0x0a; write(sfd, &ch, 1);
	return ;
}

int	cmd(void)
{
	int		sfd;
	struct sockaddr_in	saddrin;
	const char*	pszIpAddr;
	int		nPort;
	pthread_t	tid;
	char	buf[CMDMGR_MAX_INPUT_BUFFER];
	int		len;
	int		nExit = 0;
	
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
	{
		printf("<--- fail to connect cmddaemon...\n");
		return 0;
	}

	pszIpAddr = HLIB_EXTRA_GetIpAddr("cmdconsole");
	nPort     = HLIB_EXTRA_GetPortNo("cmdconsole");

	memset(&saddrin, 0, sizeof(saddrin));
	inet_pton(AF_INET, pszIpAddr, &saddrin.sin_addr);
	saddrin.sin_family = AF_INET;
	saddrin.sin_port   = htons(nPort);
	if (connect(sfd, (struct sockaddr *)&saddrin, sizeof(saddrin)) < 0)
	{
		printf("error to connect server\n");
		exit(1);
	}
printf("<p>");
	pthread_create(&tid, NULL, receiver, (void*)sfd);
	
	while (nExit<4)
	{
		len = recv(sfd, buf, CMDMGR_MAX_INPUT_BUFFER, 0);
		if (len > 0)
		{
			int i;
			
			for (i = 0; i < len; i++)
			{
				if (((nExit == 0) && (buf[i] == 'e')) ||
					((nExit == 1) && (buf[i] == 'x')) ||
					((nExit == 2) && (buf[i] == 'i')) ||
					((nExit == 3) && (buf[i] == 't')) )
				{
					nExit ++;
				}
								
				putchar(buf[i]);
			}
			fflush(NULL);
		}
		if (len < 0)
			break;
	}
printf("</p>");

	close(sfd);
	return 0;
}
#endif

void print_memory(void)
{
	FILE	  *fp;
	char 	*str;
	int		 buff_size;
			   
	buff_size	= 1024; 		   

	if ( fp = fopen( "/proc/meminfo", "r"))
	{
	  str = malloc( buff_size+5);	  

	  while( fgets( str, buff_size, fp))
		 printf( "%s<br/>", str);

	  fclose( fp);
	  free( str);
	}

	return ;
}

void print_pvr_state(void)
{
	FILE	  *fp;
	char 	*str;
	int		 buff_size;
			   
	buff_size	= 1024; 		   

	if ( fp = fopen( "/var/.lr_obama_recorder_state.log", "r"))
	{
	  str = malloc( buff_size+5);	  

	  while( fgets( str, buff_size, fp))
		 printf( "%s<br/>", str);

	  fclose( fp);
	  free( str);
	}

	return ;
}

void test (void)
{
	OAPI_MEDIA_PLAY_Stop (0);
	sleep(1);
}

void obama_live_stop (void)
{

	VK_Init();
	OAPI_MEDIA_PLAY_Init (test);
	sleep(1);
}

int main(int argc, char *argv[])
{
	/* initialize Lua */
	lua_State* L = luaL_newstate(); 

	/* load Lua base libraries */
	luaL_openlibs(L);

	/* register our function */
	lua_register(L, "print_pvr_state", print_pvr_state);
	lua_register(L, "obama_live_stop", obama_live_stop);

	/* run the script */
	luaL_dofile(L, argv[1]);
	/* cleanup Lua */
	lua_close(L);

	return 0;
}
