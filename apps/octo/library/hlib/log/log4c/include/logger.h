#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <log4c.h>
#include <_hlib_log.h>

#define SERVER_PATH "./log4c.txt"

#define 		LOG_CAT_BASIC  0
#define 		LOG_CAT_CVMEDIA 1
#define 		LOG_CAT_CVMGR 2
#define 		LOG_CAT_MEDIA 3
#define 		LOG_CAT_LINEAR 4
#define 		LOG_CAT_SYSTEM 5
#define 		LOG_CAT_OUTPUT 6
#define 		LOG_CAT_NETWORK 7
int 				LOGGER_Init(void);
int 				INT_LOGGER_InitCategory(void);
log4c_category_t* 	INT_LOGGER_GetCategory(int nCategory);

int 				INT_LOGGER_InitAppenders(void);
int 				INT_LOGGER_InitFormatters(void);

void 				LOGGER_NetLog(const HCHAR *domain, HxLOG_Level_e lv, const HCHAR *msg, void *userdata,const char *format,...);
void 				LOGGER_STDPrint(const char *format,...);

#endif /* _LOGGER_H_ */
