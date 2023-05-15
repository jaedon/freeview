#include "include/logger.h"
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct _LogInfo
{
	int fd;
	struct sockaddr_in server_addr;
	char data[4][256]; //0: domain, 1:path, 2:function, 3:line
} LogInfo;




static int local_logger_stdout_open(log4c_appender_t* this)
{
	return 0;
}

static int local_logger_stdout_append(log4c_appender_t* this,
			const log4c_logging_event_t* a_event)
{
	return printf("%s", a_event->evt_rendered_msg);
}

const log4c_appender_type_t log4c_appender_type_stdout =
{
	"cvms.stdout",
	local_logger_stdout_open,
	local_logger_stdout_append,
	NULL
};

static int local_logger_dgram_init(void)
{
	struct hostent* host;
	char msg[256],ip[128],port[128];

	LogInfo.fd = open(SERVER_PATH, O_RDONLY | O_NONBLOCK);
	if(LogInfo.fd < 0 ) return 0;
	memset(msg,0,256);
	read(LogInfo.fd,msg,256);

	sscanf(msg,"%s %s",ip,port);
	close(LogInfo.fd);

	host = (struct hostent *) gethostbyname(ip);
	if ((LogInfo.fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		return 0;
	}

	LogInfo.server_addr.sin_family = AF_INET;
	LogInfo.server_addr.sin_port = htons(atoi(port));
	LogInfo.server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(LogInfo.server_addr.sin_zero),8);

	return 0;
}

static int local_logger_dgram_open(log4c_appender_t* this)
{
	return 0;
}

static int local_logger_dgram_append(
	log4c_appender_t* this,
	const log4c_logging_event_t* a_event)
{
	return sendto(LogInfo.fd, a_event->evt_rendered_msg, strlen(a_event->evt_rendered_msg), 0,
		   (struct sockaddr *)&LogInfo.server_addr, sizeof(struct sockaddr));
}

const log4c_appender_type_t log4c_appender_type_dgram =
{
	"cvms.dgram",
	local_logger_dgram_open,
	local_logger_dgram_append,
	NULL
};

int INT_LOGGER_InitAppenders(void)
{
	local_logger_dgram_init();

	log4c_appender_type_set(&log4c_appender_type_stdout);
	log4c_appender_type_set(&log4c_appender_type_dgram);
	return 0;
}
