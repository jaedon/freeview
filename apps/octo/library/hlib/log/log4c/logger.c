#include "include/logger.h"
#include <_hlib_log.h>
#include <netdb.h>
#include <string.h>
extern struct _LogInfo
{
	int fd;
	struct sockaddr_in server_addr;
	char data[4][256]; //0: domain, 1:path, 2:function, 3:line
} LogInfo;


/*  start of static function */
static HCHAR * _hxlog_LogLvStr (HxLOG_Level_e lv, HCHAR *str)
{
	HCHAR *p = str;

	if (lv & HxLOG_PRINT)    *p++ = 'P';
	if (lv & HxLOG_MESSAGE)  *p++ = 'M';
	if (lv & HxLOG_INFO)     *p++ = 'I';
	if (lv & HxLOG_DEBUG)    *p++ = 'D';
	if (lv & HxLOG_TRACE)    *p++ = 'T';
	if (lv & HxLOG_WARNING)  *p++ = 'W';
	if (lv & HxLOG_ERROR)    *p++ = 'E';
	if (lv & HxLOG_CRITICAL) *p++ = 'C';
	if (lv & HxLOG_ASSERT)   *p++ = 'A';
	*p++ = '\0';

	return str;
}


static int ConvertHxLogLevelToLog4C(int lv)
{
	switch(lv)
	{
	case HxLOG_DEBUG : 		return LOG4C_PRIORITY_DEBUG;
	case HxLOG_INFO : 		return LOG4C_PRIORITY_INFO;
	case HxLOG_TRACE :		return LOG4C_PRIORITY_TRACE;
	case HxLOG_WARNING:		return LOG4C_PRIORITY_WARN;
	case HxLOG_ERROR : 		return LOG4C_PRIORITY_ERROR;
	case HxLOG_CRITICAL :	return LOG4C_PRIORITY_CRIT;
	case HxLOG_ASSERT : 	return LOG4C_PRIORITY_ALERT;
	case HxLOG_FATAL : 		return LOG4C_PRIORITY_FATAL;
	default: 				return LOG4C_PRIORITY_DEBUG;
	}
}
/*  start of static function */

/*  start of public function */
int LOGGER_Init(void)
{
	INT_LOGGER_InitFormatters();
	INT_LOGGER_InitAppenders();


	if (log4c_init())
	{
		printf("log4c_init() failed");
		/* TODO: set default config */
		return 1;
	}
	INT_LOGGER_InitCategory();
	printf("\n*************************************************\n");
	printf("****************** Log4c Enable *****************\n");
	printf("*************************************************\n");
	return 0;
}

//HLIB_LOG_SetHandler 에 등록되어 사용
void	LOGGER_NetLog(const HCHAR *domain, HxLOG_Level_e lv, const HCHAR *msg, void *userdata,const HCHAR *format,...)
{
	HCHAR *buf=NULL;
	int i =0;
	HCHAR	lvStr[12];
	va_list va;

	strncpy(LogInfo.data[0],domain, sizeof(LogInfo.data[0]));
	va_start(va,format);
	while(1){
		buf = va_arg(va,char*);
		if(!buf || (++i > 3)) break;
		strncpy(LogInfo.data[i],buf,sizeof(LogInfo.data[i]));
	}
	va_end(va);

	if(LogInfo.fd > 0)
		log4c_category_log(INT_LOGGER_GetCategory(LOG_CAT_NETWORK), ConvertHxLogLevelToLog4C(lv), msg);

	 //Log4c 를 사용해도 시리얼에 로그를 찍는다.
	#if defined(HXLOG_PRINT_LINE_FUNC)
	printf("%10s<%s><%s><%s>::%s", domain, _hxlog_LogLvStr(lv, lvStr), msg, LogInfo.data[2], LogInfo.data[3]);
	#else
	printf("%10s<%s>::%s", domain, _hxlog_LogLvStr(lv, lvStr), msg);
	#endif
}

//stdout 전용
void LOGGER_STDPrint(const char *format,...)
{
	va_list args;

	va_start(args, format);
	log4c_category_vlog(INT_LOGGER_GetCategory(LOG_CAT_BASIC), LOG4C_PRIORITY_INFO, format, args);
	va_end(args);
}
/*  end of public function */

//End file