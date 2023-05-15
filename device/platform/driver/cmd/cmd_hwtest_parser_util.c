#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "cmdtool.h"
#include <netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include <netdb.h>
#include <arpa/inet.h>	/* inet(3) functions */

/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {CMD_Printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}


int CMD_HwTest_Parser_Util(void *szArgs)
{
	GET_ARGS;

	if (szParam1 == NULL ){};
	if (szParam2 == NULL ) {};

	if CMD_IS("sleep")
	{
		int iMs;
		if (READABLE_IN_DEC(HWTEST_PARAM, iMs))
		{
			VK_TASK_Sleep(iMs);
		}
		iResult=CMD_OK;
	}
	else
	{
		iResult=CMD_OK;
	}

	return iResult;
}

