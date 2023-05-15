#include "include/logger.h"
#include <netdb.h>

#define MAX_LOGGER_BUFFER		1024

struct _LogInfo
{
	int fd;
	struct sockaddr_in server_addr;
	char data[4][256]; //0: domain, 1:path, 2:function, 3:line
} LogInfo;


static const char* local_logger_raw_format(
		const log4c_layout_t* a_layout,
		const log4c_logging_event_t* a_event)
{
	int line;
 	sscanf(LogInfo.data[3],"%d",&line);

	snprintf(a_event->evt_buffer.buf_data, a_event->evt_buffer.buf_size,
		 "<log4j:event logger=\"%s\" level=\"%s\" "
		 "thread=\"%s\" timestamp=\"%u%03u\"> "
		 "<log4j:message>%s</log4j:message> "
		 "<log4j:locationInfo class=\"%s\" method=\"%s\" "
		 "file=\"%s\" line=\"%d\"></log4j:locationInfo>"
		 "</log4j:event>\n",
		  LogInfo.data[0],
		  log4c_priority_to_string(a_event->evt_priority),
		  "thread", // not used
		  (unsigned int)a_event->evt_timestamp.tv_sec,
		  (unsigned int)a_event->evt_timestamp.tv_usec / 1000,
		  a_event->evt_msg,
		  "class", // not used
		  LogInfo.data[2], //funciton
		  LogInfo.data[1], //path
		  line); //line
	return a_event->evt_buffer.buf_data;
}

const log4c_layout_type_t log4c_layout_type_raw = {
	"cvms.raw",
	local_logger_raw_format
};

static const char* local_logger_basic_format(
		const log4c_layout_t* a_layout,
		const log4c_logging_event_t* a_event)
{
	static char buffer[MAX_LOGGER_BUFFER];
	snprintf(buffer, MAX_LOGGER_BUFFER, "%-8s %s - %s",
			 log4c_priority_to_string(a_event->evt_priority),
			 a_event->evt_category, a_event->evt_msg);
		return buffer;

}

const log4c_layout_type_t log4c_layout_type_basic = {
	"cvms.basic",
	local_logger_basic_format
};

int INT_LOGGER_InitFormatters(void)
{
	log4c_layout_type_set(&log4c_layout_type_raw);
	log4c_layout_type_set(&log4c_layout_type_basic);
	return 0;
}
