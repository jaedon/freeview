/* $Header:   //BASE/archives/STING_T/tool/cmdtool.c-arc   1.1   14 Mar 2005 16:20:52   jhkim1  $ */
/********************************************************************
* $Workfile:   cmdtool.c  $
* Project    : HEPVR
* Author     : Seungbum Chung
* Description: Main Function for Command Tool (cmd_tool)
*
*                                 Copyright (c) 2003 HUMAX Co., Ltd.
*                                               All rights reserved.
********************************************************************/

/********************************************************************
Header Files
********************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "vkernel.h"
#include "taskdef.h"

#include "cmdtool.h"
#include "di_err.h"
#include "di_uart.h"
#include "linden_monitor.h"
#include "linden_net.h"

#if defined(CONFIG_TELNET_CMD)
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#if defined(CONFIG_LINUX)
#include <termio.h>
#endif


/********************************************************************
Macro Definition
********************************************************************/
//	if you want to use old style command line tool, please uncomments below line.
//#define	CONFIG_OLD_STYLE_CMDTOOL

#if defined(CONFIG_OS_UCOS) && defined(CONFIG_DEBUG)
#define	PROMPT_PREFIX			"\033[1;33m"
#define	PROMPT_POSTFIX			" > \033[0m "
#define	HL_PREFIX				"\033[1;33m"
#define	HL_POSTFIX				"\033[0m "
#else
#define	PROMPT_PREFIX			"[01;36m"
#define	PROMPT_POSTFIX			" >[00m "
#define	HL_PREFIX				"[01;33m"
#define	HL_POSTFIX				"[00m "
#endif


/********************************************************************
Global/Static Variable Definition
********************************************************************/
static CMDGROUP_WORD	s_aCmdGroupWord[MAX_CMD_GROUP];	/* command group array */
static CMD_WORD	s_aCmdWord[MAX_CMD_WORD];	/* command word array */
static CMD_KEY	s_aCmdKey[MAX_CMD_KEY];		/* command key array  */
static CMD_RCU	s_aCmdRcu[MAX_CMD_RCU];		/* command RCU array  */

static char s_szRcuBuf[MAX_RCU_PRESS];		/* RCU buffer */
static char     *s_pPromptStr = CMD_PROMPT_DEFAULT;

#if defined(CONFIG_TELNET_CMD)
	#undef DEF_PORTNO
	#define DEF_PORTNO 3131
	static int s_nSockFD=-1, s_nClientSockFD=-1;
	static struct timeval s_stTimeOut;
	static socklen_t	s_ulClientLen;
	static char s_szReadBuffer[8], s_szWriteBuffer[8];
	static struct sockaddr_in s_stServerAddr, s_stClientAddr;
	static unsigned long s_ulCmdTelnetTaskId;
	static 	void CMD_Telnet_Init(void);
#endif

#define CMD_HISTORY
#if defined(CMD_HISTORY)
	static int s_nCmdCount = 1;
	static int s_nCmdHistoryCnt = 0;
	static int s_nHistoryMode = 0;
	extern int 	CMD_getHistory(char *cmd, int num, int* len);
	extern void CMD_printHistory(int num);
	extern int 	CMD_getHistSize(void);
	extern void CMD_pushHistory(char *cmd);
#endif

static int exit_cmd;

#undef UNUSED
#define UNUSED(x) (void)(x)

/********************************************************************
External Variable Declaration
********************************************************************/

/********************************************************************
Global Function
********************************************************************/
extern void CMD_HwTest_Parser_Init(void);
extern void	CMD_CrashTest_Init(void);

#if defined(CONFIG_TELNET_CMD)
static int CMD_Telnet_TransmitData(int fd, void *wrbuf, int len)
{
	int ret=0;
	ret = send(fd, wrbuf, len,0);
	return ret;
}
static int CMD_Telnet_ReceiveData(int fd, void *rdbuf, int len)
{
	int ret=0;
	ret = recv(fd, rdbuf, len,0);
	return ret;
}
static void CMD_Telnet_ShowPrompt(int fd)
{
	int ret=0, len=0;
	len = VK_strlen(s_pPromptStr);
	ret = CMD_Telnet_TransmitData(fd, s_pPromptStr, len);

}
static void CMD_Telnet_NegotiateMode(int fd)
{
	unsigned char buf[12];
	int ret=0;

	buf[0] = 255; /* IAC */
	buf[1] = 251; /* WILL */
	buf[2] = 1; /* ECHO */

	buf[3] = 255; /* IAC */
	buf[4] = 251; /* WILL */
	buf[5] = 3; /* SUPPRESS GO AHEAD */

	buf[6] = 255; /* IAC */
	buf[7] = 253; /* DO */
	buf[8] = 3; /* SUPPRESS GO AHEAD */

	ret=CMD_Telnet_TransmitData(fd, buf, 9);
	if(ret < 0) CMD_Printf(" Transmit failed ret=%d\n", ret);
}
static void CMD_Telnet_Init(void)
{
	s_nSockFD = socket(AF_INET, SOCK_STREAM, 0);
	if (s_nSockFD < 0)
		CMD_Printf("[%d]%s socket open failed \n",__LINE__,__func__);

	bzero((char *)&s_stServerAddr, sizeof(s_stServerAddr));

	s_stServerAddr.sin_family = AF_INET;
	s_stServerAddr.sin_addr.s_addr=INADDR_ANY;
	s_stServerAddr.sin_port = htons(DEF_PORTNO); /* Default Port Number */

	if(bind(s_nSockFD, (struct sockaddr *)&s_stServerAddr, sizeof(s_stServerAddr)) < 0)
		CMD_Printf("[%d]%s bind failed \n",__LINE__,__func__);

	listen(s_nSockFD, 5);

	s_stTimeOut.tv_sec = 2;
	s_stTimeOut.tv_usec = 0;

	if(setsockopt(s_nSockFD, SOL_SOCKET, SO_RCVTIMEO, (char *)&s_stTimeOut, sizeof(s_stTimeOut)) < 0)
	{
		CMD_Printf("[%d]%s setsockopt failed\n",__LINE__,__func__);
	}
}

/********************************************************************
Function   : CMD_TelnetKeyTask
Description: Command key handling task
Input      :
Output     :
Return     :
********************************************************************/
void CMD_Telnet_KeyTask(void *pvParam)
{
	int ret=0;
	UNUSED(pvParam);

	while(1)
	{
		VK_TASK_Sleep(10);
		if(s_nClientSockFD < 0)
		{
			s_ulClientLen = sizeof(s_stClientAddr);
			s_nClientSockFD = accept(s_nSockFD, (struct sockaddr *)&s_stClientAddr, &s_ulClientLen);
			if(s_nClientSockFD >= 0)
			{
				CMD_Telnet_NegotiateMode(s_nClientSockFD);
				CMD_Telnet_ShowPrompt(s_nClientSockFD);
			}
		}

		if(s_nClientSockFD > 0)
		{
			bzero(s_szReadBuffer,8);
			bzero(s_szWriteBuffer,8);

			ret=CMD_Telnet_ReceiveData(s_nClientSockFD, s_szReadBuffer, 1); /* Get one character */
			if(ret < 0)
			{
				continue;
			}
			else if (ret == 0) /* ret == 0 is diconnected from client */
			{
				close(s_nClientSockFD);
				s_nClientSockFD=-1;
			}
			else
			{
				if(s_szReadBuffer[0] == -1) /* Ignore IAC */
				{
					ret=CMD_Telnet_ReceiveData(s_nClientSockFD, s_szReadBuffer, 2);
					continue;
				}
				else
				{
					s_szWriteBuffer[0]=s_szReadBuffer[0];
					if(s_szWriteBuffer[0] == 0x08)
					{
						s_szWriteBuffer[0] = 0x08; /* Replace backspace */
						s_szWriteBuffer[1] = 0x20; /* Replace backspace */
						s_szWriteBuffer[2] = 0x08; /* Replace backspace */
						ret = CMD_Telnet_TransmitData(s_nClientSockFD, s_szWriteBuffer, 3);
					}
					else
					{
						ret = CMD_Telnet_TransmitData(s_nClientSockFD, s_szWriteBuffer, 1);
					}
				}

				if(s_szReadBuffer[0] == 0x0d) /* Check CR and Add LF */
				{
					ret=CMD_Telnet_ReceiveData(s_nClientSockFD, s_szReadBuffer, 1);
					if(s_szReadBuffer[0] == 0x00)
					{
						s_szWriteBuffer[0] = s_szReadBuffer[0] = 0x0a;
					}
					else
					{
						s_szWriteBuffer[0] = s_szReadBuffer[0];
					}
					ret = CMD_Telnet_TransmitData(s_nClientSockFD, s_szWriteBuffer, 1);

				}
			}
			CMD_OnKeyPress((char)s_szReadBuffer[0]);
			if(s_szReadBuffer[0] == 0xa || s_szReadBuffer[0] == 0xd)
				CMD_Telnet_ShowPrompt(s_nClientSockFD);
		}

	}
}
#endif
/********************************************************************
Function   : CMD_KeyTask
Description: Command key handling task
Input      :
Output     :
Return     :
********************************************************************/
unsigned char CMD_FunctionKeyProcess(unsigned char ucBuffer)
{
	unsigned char	ucSecondBuffer;

	if ((ucBuffer & 0xff) != 0x1b)	// Function Key Input
	{
		return ucBuffer;
	}

	// Get second dummy byte
	DI_UART_ReadByte((HUINT8 *)&ucBuffer);

	// Get third byte
	DI_UART_ReadByte((HUINT8 *)&ucBuffer);
	if ((ucBuffer >= 0x41) && (ucBuffer <= 0x44))
	{
		return (UART_UP + (ucBuffer - 0x41));
	}

	// Get 4th byte
	DI_UART_ReadByte((HUINT8 *)&ucSecondBuffer);
	if (((ucSecondBuffer & 0xff) == 0x7e)
		&& ((ucBuffer >= 0x31) && (ucBuffer <= 0x36)))
	{
		return (UART_INS + (ucBuffer - 0x31));
	}

	if (((ucBuffer & 0xff) == 0x31)
		&& ((ucSecondBuffer >= 0x31) && (ucSecondBuffer <= 0x39)))
	{
		// Get 5th dummy byte
		DI_UART_ReadByte((HUINT8 *)&ucBuffer);
		return (ucBuffer = UART_F1 + (ucSecondBuffer - 0x31));
	}

	if (((ucBuffer & 0xff) == 0x32)
		&& ((ucSecondBuffer >= 0x30) && (ucSecondBuffer <= 0x34)))
	{
		// Get 5th dummy byte
		DI_UART_ReadByte((HUINT8 *)&ucBuffer);
		return (ucBuffer = UART_F9 + (ucSecondBuffer - 0x30));
	}

	return ucBuffer;
}

int CMD_SetKeyPressBootWait(int bSet, int bIsPress)
{
	static int s_bIsPress = 0;
	if(bSet)
	{
		s_bIsPress = bIsPress;
	}
	return s_bIsPress;
}

unsigned char CMD_CustomKeyProcess(unsigned char ucBuffer)
{
	switch(ucBuffer)
	{
#if defined (CONFIG_OS_UCOS)
	case 0x0d: /* return */
		return 0x0a;
#endif

	default:
		break;
	}

	return ucBuffer;
}

void CMD_KeyTask(void *pvParam)
{
	unsigned char	ucBuffer;
	int err;

#if defined(CONFIG_LINUX)
	struct termios saveTermiOS;
	tcgetattr(0, &saveTermiOS);
#endif

	UNUSED(pvParam);
	while(1)
	{
		err = DI_UART_ReadByte((HUINT8 *)&ucBuffer);
		if( err )
		{
			DI_UART_Print("[CMD_KeyTask] Error(0x%x) in DI_UART_ReadByte\n\r", err);
			continue;
		}

		ucBuffer = CMD_FunctionKeyProcess(ucBuffer);
		ucBuffer = CMD_CustomKeyProcess(ucBuffer);

		CMD_OnKeyPress((char)ucBuffer);

		if (exit_cmd)
		{
			//	DDI_SCSI_MON_Stop();
			break;
		}
	}

	#if defined(CONFIG_LINUX)
	tcsetattr(0, TCSANOW, &saveTermiOS);/*Apply backup attribute*/
	#endif

}	// End of CMD_KeyTask()


HUINT8 *	CMD_StrToUpper(HUINT8 *str)
{
	HUINT8 i, ucLength=0;

	ucLength = VK_strlen( (char *)str );

	for( i = 0; i < ucLength; i++ )
		str[i] = VK_toupper(str[i]);

	return str;
}

HUINT8 *	CMD_StringToUpper(HUINT8 *str, HUINT8 *pucTarget)
{
	HUINT8 i, ucLength=0;

	if( str  == NULL || pucTarget == NULL )
		return NULL;

	ucLength = VK_strlen( (char *)str );

	for( i = 0; i < ucLength; i++ )
		pucTarget[i] = VK_toupper(str[i]);

	return str;
}

void CMD_RegisterModelName(void *szName)
{
	UNUSED(szName);
}

void CMD_RegisterPromptString(void *szPrompt)
{
	s_pPromptStr = szPrompt;
}

void CMD_ShowPrompt(void)
{
	CMD_Printf(s_pPromptStr);
	VK_fflush(NULL);
}


int CMD_DITrace(void *szArgs)
{
	UNUSED(szArgs);
#if defined(CONFIG_TELNET_CMD)
	setSocketFd(s_nClientSockFD);
#endif
	lmStart_Monitor();
	lmEnd_Monitor();

	return 0;
}

/********************************************************************
Function   : CMD_Init
Description: Init Command-Tool environment and register commands
Input      :
Output     :
Return     :
********************************************************************/
void CMD_Init(void)
{
	unsigned short i;

	/* initialize RCU buffer */
	for (i = 0; i < MAX_RCU_PRESS - 1; i++)
		s_szRcuBuf[i] = 0x7F;				/* non-zero value */

	/* initialize command group and word */
	VK_memset(s_aCmdWord, 0x00, sizeof(CMD_WORD) * MAX_CMD_WORD);
	VK_memset(s_aCmdGroupWord, 0x00, sizeof(CMDGROUP_WORD) * MAX_CMD_GROUP);

	/* make initial command line root group */
	s_aCmdGroupWord[0].nGroupId = 0;
	s_aCmdGroupWord[0].pstParentGroup = &(s_aCmdGroupWord[0]);
	VK_snprintf(s_aCmdGroupWord[0].szPromptName, 64, "%s%s%s", PROMPT_PREFIX, "ROOT", PROMPT_POSTFIX);
	VK_snprintf(s_aCmdGroupWord[0].szGroupName, MAX_WORD_LEN, "ROOT");
	VK_snprintf(s_aCmdGroupWord[0].szHelp, 23,"not categorized command");

	CMD_SetCurrentGroup("ROOT");
}


void CMD_DI_Init(void)
{

	/* print logo */
	CMD_Printf("\n\r");
	CMD_Printf("============================================================\n\r");
	CMD_Printf("             Command Tool                \n\r");
	CMD_Printf("                                       Ver : NONE\n\r");
	CMD_Printf("                                             (C) 2008 HUMAX \n\r");
	CMD_Printf("\n\r");
	CMD_Printf("============================================================\n\r");

	/* print prompt */
	CMD_ShowPrompt();

	CMD_RegisterGroup("DI", "DI", "Driver commands");
	CMD_RegisterWord(CMD_DITrace,
		(char *)"di_trace",
		(char *)"di trace debug control",
		(char *)"di_trace");

#if defined(CONFIG_OS_UCOS)
#else
	CMD_HwTest_Parser_Init();
	CMD_CrashTest_Init();
#endif

	CMD_ReleaseRegisterGroup();

#if defined(CONFIG_TELNET_CMD)
	CMD_Telnet_Init();
	VK_TASK_Create(CMD_Telnet_KeyTask, BASE_PRIORITY, BASIC_TASK_STACK_SIZE, "CMD_Telnet_KeyTask", (void *)0, &s_ulCmdTelnetTaskId, FALSE);
	VK_TASK_Start(s_ulCmdTelnetTaskId);
#endif
}

static	CMDGROUP_WORD	*_pstCmdCurrentGroup = &s_aCmdGroupWord[0];
static	CMDGROUP_WORD	*_pstCmdRegisterGroup = &s_aCmdGroupWord[0];

CMDGROUP_WORD	*FindCMDGroupWithGroupName(char *szGroupName)
{
	int		i;

	for (i = 0; i < MAX_CMD_GROUP; i++)
	{
		if (s_aCmdGroupWord[i].pstParentGroup)
		{
			if (VK_strcasecmp(s_aCmdGroupWord[i].szGroupName, szGroupName) == 0)
				return &(s_aCmdGroupWord[i]);
		}
	}

	return NULL;
}

void CMD_SetCurrentGroup(char *szGroupName)
{
	CMDGROUP_WORD *pstCmdGroup;

	pstCmdGroup = FindCMDGroupWithGroupName(szGroupName);
	if (pstCmdGroup == NULL)
		return;

	_pstCmdCurrentGroup = pstCmdGroup;
	CMD_RegisterPromptString(pstCmdGroup->szPromptName);
}

void CMD_RegisterGroup(char *szGroupName, char *szPrompt, char *szHelp)
{
	int		i;

	CMDGROUP_WORD	*pstCmdGroup;

#ifndef	CONFIG_OLD_STYLE_CMDTOOL
	for (i = 0; i < MAX_CMD_GROUP; i++)
	{
		pstCmdGroup = &s_aCmdGroupWord[i];
		if (pstCmdGroup->pstParentGroup == NULL)
			break;
		if (VK_strcmp(pstCmdGroup->szGroupName, szGroupName) == 0)
		{
			_pstCmdRegisterGroup = pstCmdGroup;
			return;
		}
	}

	if(szHelp != NULL && szPrompt != NULL)
	{
		if (i < MAX_CMD_GROUP)
		{
			pstCmdGroup->pstParentGroup = _pstCmdCurrentGroup;
			VK_strncpy(pstCmdGroup->szGroupName, szGroupName, MAX_WORD_LEN - 1);
			VK_snprintf(pstCmdGroup->szPromptName, 8+VK_strlen(szPrompt)+8, "%s%s%s", PROMPT_PREFIX, szPrompt, PROMPT_POSTFIX);
			VK_strncpy(pstCmdGroup->szHelp, szHelp, MAX_HELP_LEN);
			pstCmdGroup->nGroupId = 0;

			_pstCmdRegisterGroup = pstCmdGroup;
		}
	}
#endif

}

void CMD_ReleaseRegisterGroup(void)
{
	_pstCmdRegisterGroup = &s_aCmdGroupWord[0];
}

/********************************************************************
Function   : CMD_RegisterWord
Description:
Input      :
Output     :
Return     :
********************************************************************/\
void CMD_RegisterWord(int (*fnToDo)(void *arg), char *szWord, char *szHelp, char *szUsage)
{
	unsigned long 	i;

	for (i = 0; i < MAX_CMD_WORD; i++)
	{
		/* skip commands in static array which has been defined already */
		if (VK_strcasecmp((const void *)s_aCmdWord[i].szWord, (const void *)szWord) == 0)
		{
			CMD_Printf("Already defined word - %s\n\r", szWord);
			return;
		}
		if (s_aCmdWord[i].fnToDo == NULL)
		{
			break;
		}
	}

	/* array is full */
	if (i == MAX_CMD_WORD)
		return;

	/* add command word */
	s_aCmdWord[i].fnToDo 	= fnToDo;

	// Copy data to buffer
	if (szWord)
		VK_strncpy((void *)s_aCmdWord[i].szWord, (const void *)szWord, (MAX_WORD_LEN-1));
	if (szHelp)
		VK_strncpy((void *)s_aCmdWord[i].szHelp, (const void *)szHelp, (MAX_HELP_LEN-1));
	if (szUsage)
		VK_strncpy((void *)s_aCmdWord[i].szUsage, (const void *)szUsage, (MAX_USAGE_LEN-1));

	s_aCmdWord[i].pstGroupPtr = _pstCmdRegisterGroup;

	return;
}

/********************************************************************
Function   : CMD_ListGroup
Description:
Input      :
Output     :
Return     :
********************************************************************/
void CMD_ListGroup(void)
{
	int		i, j , k;
	char	word[MAX_WORD_LEN + 1];

#ifndef	CONFIG_OLD_STYLE_CMDTOOL
	CMD_Printf("\n\r");
	CMD_Printf("======== List of Register Group ==========\n");
	for (i = 0; i < MAX_CMD_GROUP; i++)
	{
		if (s_aCmdGroupWord[i].pstParentGroup != NULL)
		{
			VK_memset(word, ' ', MAX_WORD_LEN);
			word[MAX_WORD_LEN] = '\0';

			for (j = MAX_WORD_LEN - VK_strlen(s_aCmdGroupWord[i].szGroupName), k = 0; j < MAX_WORD_LEN; j++, k++)
				word[j] = s_aCmdGroupWord[i].szGroupName[k];
			CMD_Printf(HL_PREFIX "%s" HL_POSTFIX ": %s\n\r", word, s_aCmdGroupWord[i].szHelp);
		}
	}
#endif
}

/********************************************************************
Function   : CMD_ListAllCommandWithFullPath
Description:
Input      :
Output     :
Return     :
********************************************************************/
void CMD_ListAllCommandWithFullPath(void)
{
	unsigned short i, j, k;

	char word[MAX_WORD_LEN + 1];

#ifndef	CONFIG_OLD_STYLE_CMDTOOL
	CMD_Printf("\n\r");
	CMD_Printf("======== List of Registered Keyword (in Group ALL)========================\n\r");

	/* print all registered keywords and help string */
	for (i = 0; (s_aCmdWord[i].fnToDo != NULL) && (i < MAX_CMD_WORD); i++)
	{
		VK_memset(word, ' ', MAX_WORD_LEN);
		word[MAX_WORD_LEN] = '\0';

		if (s_aCmdWord[i].pstGroupPtr == NULL)
			continue;

		for (j = MAX_WORD_LEN - VK_strlen(s_aCmdWord[i].pstGroupPtr->szGroupName), k = 0; j < MAX_WORD_LEN; j++, k++)
			word[j] = s_aCmdWord[i].pstGroupPtr->szGroupName[k];
		CMD_Printf(HL_PREFIX "%s" HL_POSTFIX " ", word);

		for (j = MAX_WORD_LEN - VK_strlen(s_aCmdWord[i].szWord), k = 0; j < MAX_WORD_LEN; j++, k++)
			word[j] = s_aCmdWord[i].szWord[k];

		CMD_Printf("%s: %s\n\r", word, s_aCmdWord[i].szHelp);
	}

	CMD_Printf("================================= total %03d Keyword ========\n\r", i);
#endif
}

/********************************************************************
Function   : CMD_ListWord
Description:
Input      :
Output     :
Return     :
********************************************************************/
void CMD_ListWord(void)
{
	unsigned short i, j, k, nCnt;

	char word[MAX_WORD_LEN + 1];

	CMD_Printf("\n\r");
	CMD_Printf("======== List of Registered Keyword (in Group %s%s%s)========================\n\r", HL_PREFIX, _pstCmdCurrentGroup->szGroupName, HL_POSTFIX);

	nCnt = 0;
	/* print all registered keywords and help string */
	for (i = 0; (s_aCmdWord[i].fnToDo != NULL) && (i < MAX_CMD_WORD); i++)
	{
		if (s_aCmdWord[i].fnToDo && s_aCmdWord[i].pstGroupPtr == _pstCmdCurrentGroup)
		{
			nCnt++;
			VK_memset(word, ' ', MAX_WORD_LEN);
			word[MAX_WORD_LEN] = '\0';

			for (j = MAX_WORD_LEN - VK_strlen(s_aCmdWord[i].szWord), k = 0; j < MAX_WORD_LEN; j++, k++)
				word[j] = s_aCmdWord[i].szWord[k];

			CMD_Printf("%s: %s\n\r", word, s_aCmdWord[i].szHelp);
		}
	}

	CMD_Printf("================================= total %03d Keyword ========\n\r", nCnt);
}

/********************************************************************
Function   : CMD_RegisterKey
Description:
Input      :
Output     :
Return     :
********************************************************************/
void CMD_RegisterKey(void (*fnToDo)(void), unsigned char ucKeyCode, char *szHelp, char *szUsage)
{
	unsigned short i;

	/* skip commands in static array which has been defined already */
	for (i = 0; s_aCmdKey[i].fnToDo && (i < MAX_CMD_KEY); i++)
		;

	/* array is full */
	if (i == MAX_CMD_KEY)
		return;

	/* add command key */
	s_aCmdKey[i].fnToDo 	= fnToDo;
	s_aCmdKey[i].ucKeyCode	= ucKeyCode;

	// Added by iwnam, 2002/10/22, for fitting buffer
	// Initialize buffer
	VK_memset((void *)s_aCmdKey[i].szHelp, 0, (unsigned int)MAX_HELP_LEN);
	VK_memset((void *)s_aCmdKey[i].szUsage, 0, (unsigned int)MAX_USAGE_LEN);

	// Copy data to buffer
	VK_memcpy((void *)s_aCmdKey[i].szHelp, (const void *)szHelp, (unsigned int)(MAX_HELP_LEN-1));
	VK_memcpy((void *)s_aCmdKey[i].szUsage, (const void *)szUsage, (unsigned int)(MAX_USAGE_LEN-1));

	return;
}

/********************************************************************
Function   : CMD_ListKey
Description:
Input      :
Output     :
Return     :
********************************************************************/
void CMD_ListKey(void)
{
	unsigned short i;

	CMD_Printf("\n\r");
	CMD_Printf("======== List of Registered Key Press ======================\n\r");

	/* print all registered keywords and help string */
	for (i = 0; s_aCmdKey[i].fnToDo && (i < MAX_CMD_KEY); i++)
	{
		CMD_Printf("  keycode 0x%02x (%s) - %s\n\r",
			s_aCmdKey[i].ucKeyCode,
			s_aCmdKey[i].szUsage,
			s_aCmdKey[i].szHelp);
	}

	CMD_Printf("===================================== total %03d Key ========\n\r", i);
}

/********************************************************************
Function   : CMD_RegisterKey
Description:
Input      :
Output     :
Return     :
********************************************************************/
void CMD_RegisterRcu(void (*fnToDo)(void), char *szRcuCode, char *szHelp, char *szUsage)
{
	unsigned short i;

	/* skip commands in static array which has been defined already */
	for (i = 0; s_aCmdRcu[i].fnToDo && (i < MAX_CMD_RCU); i++)
		;

	/* array is full */
	if (i == MAX_CMD_RCU)
		return;

	/* add command RCU */
	s_aCmdRcu[i].fnToDo 	= fnToDo;

	// Added by iwnam, 2002/5/20, for fitting buffer
	// Initialize buffer
	VK_memset((void *)s_aCmdRcu[i].szRcuCode, 0, (unsigned int)MAX_RCU_PRESS);
	VK_memset((void *)s_aCmdRcu[i].szHelp, 0, (unsigned int)MAX_HELP_LEN);
	VK_memset((void *)s_aCmdRcu[i].szUsage, 0, (unsigned int)MAX_USAGE_LEN);

	VK_memcpy((void *)s_aCmdRcu[i].szRcuCode, (const void *)szRcuCode, (unsigned int)(MAX_RCU_PRESS-1));
	VK_memcpy((void *)s_aCmdRcu[i].szHelp, (const void *)szHelp, (unsigned int)(MAX_HELP_LEN-1));
	VK_memcpy((void *)s_aCmdRcu[i].szUsage, (const void *)szUsage, (unsigned int)(MAX_USAGE_LEN-1));

	return;
}

/********************************************************************
Function   : CMD_ListKey
Description:
Input      :
Output     :
Return     :
********************************************************************/
void CMD_ListRcu(void)
{
	unsigned short i, j;

	CMD_Printf("\n\r");
	CMD_Printf("======== List of Registered RCU Sequence ===================\n\r");

	/* print all registered RCU sequence and help string */
	for (i = 0; s_aCmdRcu[i].fnToDo && (i < MAX_CMD_RCU); i++)
	{
		CMD_Printf("  RCU sequence [");

		for (j = 0; j < VK_strlen(s_aCmdRcu[i].szRcuCode); j++)
			CMD_Printf("0x%x ", s_aCmdRcu[i].szRcuCode[j]);

		CMD_Printf("] (%s) - %s\n\r",
			s_aCmdRcu[i].szUsage,
			s_aCmdRcu[i].szHelp);
	}

	CMD_Printf("===================================== total %03d RCU ========\n\r", i);
}

/********************************************************************
Function   : CMD_Parse
Description:
Input      :
Output     :
Return     :
********************************************************************/
void CMD_Parse(char *pszLine)
{
	unsigned int 	i, j;
	char 	szCmd[MAX_CMD_LINE] = {0, };
	char 	szCmdLine[MAX_CMD_LINE] = {0, };
	char 	*pszArg = NULL;
	unsigned int ulLength;
	CMDGROUP_WORD *pstTmpGroup;

	static int  _bExitConfirm = 0;


	if (NULL == pszLine || 0 == pszLine[0])
	{
		return;
	}

	VK_strncpy(szCmdLine, pszLine, (MAX_CMD_LINE-1));
	pszArg = szCmdLine;
	for(j=0; j < (MAX_CMD_LINE-1); j++)
	{
		if(0 == pszArg[j] || ' ' == pszArg[j])
		{
			break;
		}
		szCmd[j] = pszArg[j];
	}
	szCmd[j] = 0;

	pszArg = szCmdLine + VK_strlen(szCmd) + 1;

	if (VK_strcmp((const char *)"exit", (const char *)szCmd) == 0)
	{
		if (_pstCmdCurrentGroup == _pstCmdCurrentGroup->pstParentGroup)
		{
			if (_bExitConfirm == 0)
			{
				CMD_Printf("Current Command mode is root\nIf you want to quit debug mode, please enter 'exit' again\n");
				_bExitConfirm = 1;
			}
			else
			{
				#if defined(CONFIG_TELNET_CMD)
				#else
					exit_cmd = 1;
				#endif
			}
		}
		else
		{
			CMD_SetCurrentGroup(_pstCmdCurrentGroup->pstParentGroup->szGroupName);
			CMD_Printf("Command mode changes to " HL_PREFIX "%s" HL_POSTFIX " group\n", _pstCmdCurrentGroup->szGroupName);
		}
		return;
	}
	_bExitConfirm = 0;

	if (VK_strcmp((const char *)"humaxtv", (const char *)szCmd) == 0)
	{
		// to skip worng command
		return;
	}

	/* if keyword is "help" */
	if (VK_strcmp((const char *)"help", (const char *)szCmd) == 0)
	{
		if (VK_strcmp((const char *)"all", (const char *)pszArg) == 0)
		{
			CMD_ListAllCommandWithFullPath();

			return;
		} else
		{
			CMD_ListGroup();	/* show list of registerd group */
			CMD_ListWord(); 	/* show list of registered keyword */
			CMD_ListKey();		/* show list of registered keycode */
			CMD_ListRcu();		/* show list of registered RCU seq */

			return;
		}
	}

	/* search command in array of group list */
	pstTmpGroup = _pstCmdCurrentGroup;
	for (i = 0; (i < MAX_CMD_GROUP) && (s_aCmdGroupWord[i].pstParentGroup != NULL); i++)
	{
		ulLength = VK_strlen(szCmd);
		if (ulLength >= MAX_WORD_LEN)
		{
			ulLength = MAX_WORD_LEN - 1;
		}
 		if (VK_strcasecmp(s_aCmdGroupWord[i].szGroupName, szCmd) == 0)
 		{
			if (VK_strlen(pszArg) == 0)
			{
				CMD_SetCurrentGroup(s_aCmdGroupWord[i].szGroupName);
				CMD_Printf("Command mode changes to " HL_PREFIX "%s" HL_POSTFIX " group\n", _pstCmdCurrentGroup->szGroupName);
				return;
			}
			else
			{
				for(j=0; j < (MAX_CMD_LINE-1); j++)
				{
					if(0 == pszArg[j] || ' ' == pszArg[j])
					{
						break;
					}
					szCmd[j] = pszArg[j];
				}
				szCmd[j] = 0;

				pszArg = pszArg + VK_strlen(szCmd) + 1;
				pstTmpGroup = &(s_aCmdGroupWord[i]);
			}
		}
	}

	/* search string in array of registered keywords in group*/
	for (i = 0; (i < MAX_CMD_WORD) && (s_aCmdWord[i].fnToDo != NULL); i++)
	{
		//if (s_aCmdWord[i].pstGroupPtr == pstTmpGroup)
		{
			ulLength = VK_strlen(szCmd);
			if (ulLength >= MAX_WORD_LEN)
				ulLength = MAX_WORD_LEN-1;
 			if (VK_strcasecmp(s_aCmdWord[i].szWord, szCmd) == 0)
 			{
				if ( (s_aCmdWord[i].fnToDo)(pszArg) == CMD_ERR)
					CMD_Printf("  usage: %s\n\r", s_aCmdWord[i].szUsage);

				return;
			}
		}
	}

	/* unknown command */
	//CMD_Printf("  unknown command: [%s], len %d\n\r", szCmd, VK_strlen(szCmd));
	//system(pszLine);
	VK_SYSTEM_Command( pszLine );
}


/********************************************************************
Function   : CMD_NextArg
Description: obtain next argument from argument line (if any)
Input      :
Output     :
Return     :
********************************************************************/
char *CMD_NextArg(char **szLine)
{
	char *pszArg = NULL ;

	/* find start of argument */
	while (**szLine == ' ' || **szLine == '\t')
		*szLine += 1 ;

	if (**szLine != '\0')
	{
		pszArg = *szLine ;

		/* find end of argument */
		while (**szLine != '\0' && **szLine != ' ' && **szLine != '\t')
			*szLine += 1 ;

		/* terminate this argument and prepare for next (if any) */
		if (**szLine != '\0')
		{
			**szLine = '\0' ;
			*szLine += 1 ;
		}
	} /* else no more arguments */

	return pszArg;
}

/********************************************************************
Function   : CMD_ReadAsHex
Description: Convert string to Hex
Input      :
Output     :
Return     :
********************************************************************/
int CMD_ReadAsHex(char *pString, unsigned int *puiHex_value )
{
	unsigned int string_index = 0;
	unsigned int string_length = 0;
	unsigned int return_value = 0;
	unsigned int temp_value = 0;

	if ( ( pString == NULL ) || (puiHex_value == NULL ))
	{
		return 0;
	}

	while(pString[string_index] != '\0' )
	{
		string_length++;
		string_index++;
	}


	if ( ( string_length == 0 ) || (string_length > 16 ) )
	{
		return 0;
	}

	for ( string_index = 0 ; string_index < string_length ; string_index++ )
	{
		switch(pString[string_index])
		{
		case '0':
			temp_value = 0 ;
			break;
		case '1':
			temp_value = 1 ;
			break;
		case '2':
			temp_value = 2 ;
			break;
		case '3':
			temp_value = 3 ;
			break;
		case '4':
			temp_value = 4 ;
			break;
		case '5':
			temp_value = 5 ;
			break;
		case '6':
			temp_value = 6 ;
			break;
		case '7':
			temp_value = 7 ;
			break;
		case '8':
			temp_value = 8 ;
			break;
		case '9':
			temp_value = 9 ;
			break;
		case 'a':
		case 'A':
			temp_value = 10 ;
			break;
		case 'b':
		case 'B':
			temp_value = 11 ;
			break;
		case 'c':
		case 'C':
			temp_value = 12 ;
			break;
		case 'd':
		case 'D':
			temp_value = 13 ;
			break;
		case 'e':
		case 'E':
			temp_value = 14 ;
			break;
		case 'f':
		case 'F':
			temp_value = 15 ;
			break;
		default:
			return 0;
		}
		return_value += temp_value << ((string_length - string_index-1)*4);
	}
	*puiHex_value = return_value;
	return 1;
}

/********************************************************************
Function   : CMD_ReadAsDec
Description: Convert string to Hex
Input      :
Output     :
Return     :
********************************************************************/
int CMD_ReadAsDec(char *pString, int *puiDec_value )
{
	unsigned int string_index = 0;
	unsigned int string_length = 0;
	unsigned int return_value = 0;
	unsigned int temp_value = 0;
	unsigned int temp_value2 = 0;
	int signofvalue = 1;
	int skip_calc = 0;
	if ( ( pString == NULL ) || (puiDec_value == NULL ))
	{
		return 0;
	}


	while(pString[string_index] != '\0' )
	{
		string_length++;
		string_index++;
	}
	if ( ( string_length == 0 ) || (string_length > 10 ) )
	{
		return 0;
	}

	for ( string_index = 0 ; string_index < string_length ; string_index++ )
	{
		switch(pString[string_index])
		{
		case '-':
			signofvalue = -1;
			skip_calc = 1;
			break;
		case '0':
			temp_value = 0 ;
			break;
		case '1':
			temp_value = 1 ;
			break;
		case '2':
			temp_value = 2 ;
			break;
		case '3':
			temp_value = 3 ;
			break;
		case '4':
			temp_value = 4 ;
			break;
		case '5':
			temp_value = 5 ;
			break;
		case '6':
			temp_value = 6 ;
			break;
		case '7':
			temp_value = 7 ;
			break;
		case '8':
			temp_value = 8 ;
			break;
		case '9':
			temp_value = 9 ;
			break;
		default:
			return 0;
		}
		if(skip_calc == 0)
		{
			for ( temp_value2 = 0 ; temp_value2 < (string_length - string_index-1) ; temp_value2++ )
			{
				temp_value = temp_value * 10;
			}
			return_value += temp_value ;
		}
		else
		{
			skip_calc = 0;
		}
	}
	*puiDec_value = return_value;
	*puiDec_value =  *puiDec_value*signofvalue;
	return 1;
}


void CMD_FindWordByStartChar(char *pszCmdLine, unsigned char *pucCmdMaxLineIndex, unsigned char *pucCmdLineIndex)
{
	unsigned char ucCmdLineIndex = *pucCmdLineIndex;
	unsigned char ucCmdMaxLineIndex = *pucCmdMaxLineIndex;
	int cnt = 0;
	int sameLength = 0, saveLength = 255 ;
	int findIdx[MAX_CMD_WORD] = {0, };
	int i, j;

	ucCmdLineIndex = ucCmdMaxLineIndex;
	CMD_Printf("\r\n");
	pszCmdLine[ucCmdMaxLineIndex] = 0;

#if 0
	{
		char szCmdTempLine[MAX_CMD_LINE];

		CMD_Printf("[01;36m === file === [00m\r\n");
		snprintf(szCmdTempLine, MAX_CMD_LINE, "ls -d %s*", pszCmdLine);
		VK_SYSTEM_Command(szCmdTempLine);
		CMD_Printf("\r\n\r\n");
	}
#endif

	CMD_Printf("[01;36m === cmd list === [00m\r\n");

	for (i = 0; (s_aCmdWord[i].fnToDo != NULL) && (i < MAX_CMD_WORD); i++)
	{
		if (s_aCmdWord[i].fnToDo && VK_strncmp(s_aCmdWord[i].szWord, pszCmdLine, VK_strlen(pszCmdLine)) == 0)
		{
			CMD_Printf("%-30s", s_aCmdWord[i].szWord);
			findIdx[cnt++] = i;
			if(cnt % 7 == 0)
			{
				CMD_Printf("\r\n");
			}
		}
	}
	if(0 == cnt)
	{
		goto EXIT_FUNC;
	}

	if(1 == cnt)
	{
		VK_strcpy(pszCmdLine, s_aCmdWord[findIdx[0]].szWord);
		ucCmdMaxLineIndex = VK_strlen(s_aCmdWord[findIdx[0]].szWord);
		ucCmdLineIndex = ucCmdMaxLineIndex;
		goto EXIT_FUNC;
	}

	if(0 == ucCmdMaxLineIndex)
	{
		goto EXIT_FUNC;
	}

	for(i=1; i < cnt; i++)
	{
		sameLength = 0;
		for(j=0; j < MAX_WORD_LEN; j++)
		{
			if(s_aCmdWord[findIdx[i]].szWord[j] != s_aCmdWord[findIdx[0]].szWord[j])
			{
				break;
			}
			sameLength++;
		}
		if(sameLength < saveLength)
		{
			saveLength = sameLength;
		}
	}
	VK_strncpy(pszCmdLine, s_aCmdWord[findIdx[0]].szWord, saveLength);
	ucCmdMaxLineIndex = saveLength;
	ucCmdLineIndex = ucCmdMaxLineIndex;


EXIT_FUNC:
	CMD_Printf("\r\n");
	CMD_ShowPrompt();
	DI_UART_Write(pszCmdLine,ucCmdMaxLineIndex);

	*pucCmdLineIndex = ucCmdLineIndex;
	*pucCmdMaxLineIndex = ucCmdMaxLineIndex;
}

/********************************************************************
Function   : CMD_OnKeyPress
Description: Handler for key from input device
Input      :
Output     :
Return     :
********************************************************************/

void CMD_OnKeyPress(const char key)
{
	static char s_szCmdLine[MAX_CMD_LINE];
	static int s_nCmdLineIndex = 0;
	static int s_nCmdMaxLineIndex = 0;

	unsigned int i;

	/* search key in array of registered keycode */
	for (i = 0; (s_aCmdKey[i].fnToDo != NULL) && (i < MAX_CMD_KEY); i++)
	{
		if (s_aCmdKey[i].ucKeyCode == (unsigned char)key)
		{
			(s_aCmdKey[i].fnToDo)();
			return;
		}
	}

	if ((key == 0x20) && (s_nCmdMaxLineIndex == 0))
		return;

	/* usual key */
#if defined(CMD_HISTORY)
	if ( s_nHistoryMode == 0 && VK_isprint(key) )
#else
	if ( VK_isprint(key) )
#endif
	{
		if(s_nCmdLineIndex == s_nCmdMaxLineIndex)
		{
			CMD_Printf("%c", key);		/* local echo */
			s_szCmdLine[s_nCmdLineIndex++] = key;
			s_nCmdMaxLineIndex++;
		}
		else
		{
			CMD_Printf("[M\r");
			for(i=s_nCmdMaxLineIndex; i > s_nCmdLineIndex; i--)
			{
				s_szCmdLine[i] = s_szCmdLine[i-1];
			}
			s_szCmdLine[s_nCmdLineIndex]=key;
			s_szCmdLine[s_nCmdMaxLineIndex+1] = 0;
			CMD_ShowPrompt();
			CMD_Printf("%s", s_szCmdLine);
			s_nCmdMaxLineIndex++;
			s_nCmdLineIndex++;

			for(i=0; i < (unsigned int)(s_nCmdMaxLineIndex-s_nCmdLineIndex); i++)
			{
				CMD_Printf("[D");
			}
		}
	}

#if defined (CONFIG_OS_UCOS) && defined(CONFIG_DEBUG)
	switch((key&0xFF))
	{
	case 0x81: case 0x82: case 0x83: case 0x84: /* arrow key */
	case 0xa1: case 0xa2: /* shift key (left/right) */
	case 0xa3: case 0xa4: /* control key (left/right) */
		CMD_SetKeyPressBootWait(TRUE, TRUE);
		break;

	default:
		break;
	}
#endif

#if defined(CMD_HISTORY)
	switch((key&0xFF))
	{
	case 0x81: /* up */
		{
			int ret;
			ret = CMD_getHistory((char*)s_szCmdLine, s_nCmdHistoryCnt-1, (int *)&s_nCmdMaxLineIndex);
			if( ret == 1 )
			{
				CMD_Printf("[M\r");
				CMD_ShowPrompt();
				DI_UART_Write(s_szCmdLine,s_nCmdMaxLineIndex);
				s_nCmdLineIndex = s_nCmdMaxLineIndex;
				s_nCmdHistoryCnt--;
			}
		}
		break;

	case 0x82: /* down */
		{
			int ret;
			ret = CMD_getHistory((char*)s_szCmdLine, s_nCmdHistoryCnt+1, (int *)&s_nCmdMaxLineIndex);
			if( ret == 1 )
			{
				CMD_Printf("[M\r");
				CMD_ShowPrompt();
				DI_UART_Write(s_szCmdLine,s_nCmdMaxLineIndex);
				s_nCmdLineIndex = s_nCmdMaxLineIndex;
				s_nCmdHistoryCnt++;
			}
		}
		break;

	case 0xA8: /* f8 */
		{
			int ret;
			int histNum;
			CMD_Printf("\n");
			CMD_Printf("0 : Ignore\n");
			CMD_printHistory(CMD_getHistSize());
			CMD_Printf("Num : ");
			VK_scanf("%d", &histNum);

			s_nHistoryMode = 1;

			if( histNum > 0 )
			{
				ret = CMD_getHistory((char*)s_szCmdLine, histNum, (int *)&s_nCmdMaxLineIndex);
				if( ret == 1 )
				{
					CMD_ShowPrompt();
					DI_UART_Write(s_szCmdLine,s_nCmdMaxLineIndex);
					s_nCmdLineIndex = s_nCmdMaxLineIndex;
				}
			}
		}
		break;


	default:
		//CMD_Printf("0x%x", key);
		break;
	}
#endif

	/* carrige return */
	switch ((key & 0xFF))
	{
	case 0x09: /* tab */
		{
			CMD_FindWordByStartChar(s_szCmdLine, &s_nCmdMaxLineIndex, &s_nCmdLineIndex);
		}
		break;

	case 0x83: /* right */
		{
			if(s_nCmdLineIndex < s_nCmdMaxLineIndex)
			{
				s_nCmdLineIndex++;
				CMD_Printf("[1C");
			}
		}
		break;

	case 0x84: /* left */
		{
			if(s_nCmdLineIndex > 0)
			{
				s_nCmdLineIndex--;
				CMD_Printf("[1D");
			}
		}
		break;

	case 0x08: /* back-space */
	case 0x7f: /* back-space */
		if(s_nCmdLineIndex > 0)
		{
			if(s_nCmdLineIndex == s_nCmdMaxLineIndex)
			{
				CMD_Printf("%c %c", 0x08, 0x08);
				s_nCmdMaxLineIndex--;
				s_nCmdLineIndex--;
			}
			else
			{
				CMD_Printf("[M\r");
				for(i=s_nCmdLineIndex; i < s_nCmdMaxLineIndex; i++)
				{
					s_szCmdLine[i-1] = s_szCmdLine[i];
				}
				s_szCmdLine[i-1]=0;
				CMD_ShowPrompt();
				CMD_Printf("%s", s_szCmdLine);
				s_nCmdMaxLineIndex--;
				s_nCmdLineIndex--;

				for(i=0; i < (unsigned int)(s_nCmdMaxLineIndex-s_nCmdLineIndex); i++)
				{
					CMD_Printf("[D");
				}
			}
		}
		break;

	case 0x0a: /* return */
		CMD_Printf("\n");

		s_szCmdLine[s_nCmdMaxLineIndex]	= 0;	/* NULL-terminated  */
#if defined(CMD_HISTORY)
		s_nHistoryMode = 0;

		if( s_nCmdMaxLineIndex > 0 )
		{
			CMD_pushHistory(&s_szCmdLine[0]);
		}
		s_nCmdHistoryCnt = s_nCmdCount;
#endif
		CMD_Parse(s_szCmdLine);					/* parse this line  */

		if (exit_cmd)
			return;
		//CMD_Printf("Uart> String: %s\n\r", s_szCmdLine);

		s_nCmdMaxLineIndex = 0;					/* reset index to 0 */
		s_nCmdLineIndex = 0;
		VK_memset(s_szCmdLine, 0, MAX_CMD_LINE);				/* reset str buffer */

		CMD_ShowPrompt();
		break;

	default:
		break;
	}
}

/********************************************************************
Function   : CMD_OnRcuPress
Description: Handler for RCU from input device
Input      :
Output     :
Return     :
********************************************************************/
void CMD_OnRcuPress(const char key)
{
	unsigned short	i;
	HINT32 	nLength;

	for (i = 0; i < MAX_RCU_PRESS - 2; i++)
		s_szRcuBuf[i] = s_szRcuBuf[i + 1];		/* shift to left */

	/* store current key to tail */
	s_szRcuBuf[i] = key;

#if 0
	CMD_Printf("\t\t RCU Buffer: ");
	for (i = 0; i < MAX_RCU_PRESS; i++)
		CMD_Printf("0x%x ", s_szRcuBuf[i]);
	CMD_Printf("\n\r");
#endif


	/* search string in array of registered RCU key string */
	for (i = 0; (s_aCmdRcu[i].fnToDo != NULL) && (i < MAX_CMD_RCU); i++)
	{
		nLength = VK_strlen(s_aCmdRcu[i].szRcuCode);
		if (VK_strcmp(s_aCmdRcu[i].szRcuCode, s_szRcuBuf + (MAX_RCU_PRESS - nLength - 1)) == 0)
		{
			(s_aCmdRcu[i].fnToDo)();
			return;
		}
	}
}

HUINT8 PositionOfNextSpaceInBuffer(HUINT8* buffer, HUINT8* bufIndex)
{
	HUINT8		position;
	HUINT8	result = TRUE;
	HUINT8	spaceReached;
	HUINT8	endReached;
	HUINT8	currentChar;

	position = *bufIndex;
	spaceReached = FALSE;
	endReached = FALSE;

	while ((!endReached) && (!spaceReached))
	{
		currentChar = buffer[position];
		if (currentChar == '\0')
		{
			endReached = TRUE;
		}
		else if (currentChar == ' ')
		{
			spaceReached = TRUE;
		}
		else
		{
			position++;
		}
	}

	/* returns FALSE if no more space chars left */
	if (endReached)
	{
		result = FALSE;
	}

	/* return the position value in bufIndex */
	*bufIndex = position;

	return result;
}


HUINT8 PositionOfNextNonSpaceInBuffer(HUINT8* buffer, HUINT8* bufIndex)
{
	HUINT8		position;
	HUINT8	result;
	HUINT8	nonSpaceReached;
	HUINT8	endReached;
	HUINT8	currentChar;

	result = TRUE;
	position = *bufIndex;
	nonSpaceReached = FALSE;
	endReached = FALSE;

	/* go to the first non-space character */
	while ((!endReached) && (!nonSpaceReached))
	{
		currentChar = buffer[position];
		if (currentChar == '\0')
		{
			endReached = TRUE;
		}
		else if (currentChar != ' ')
		{
			nonSpaceReached = TRUE;
		}
		else
		{
			position++;
		}
	}

	/* returns FALSE if no more non-whitespace chars left */
	if (endReached)	result = FALSE;

	/* return the position value in bufIndex */
	else			*bufIndex = position;

	return result;
}


HUINT8 GetNumberOfArgs(HUINT8* argBuffer)
{
	HUINT8	position;
	HUINT8		numberOfArgs;
	HUINT8	stillMoreText;

	numberOfArgs = 0;
	position = 0;

	stillMoreText = PositionOfNextNonSpaceInBuffer(argBuffer, &position);
	while (stillMoreText)
	{
		numberOfArgs++;
		stillMoreText = PositionOfNextSpaceInBuffer(argBuffer, &position);
		if (stillMoreText)
		{
			stillMoreText = PositionOfNextNonSpaceInBuffer(argBuffer, &position);
		}
	}

	return numberOfArgs;
}

HUINT8 NextArgString(HUINT8** startOfLine, HUINT8* resultBuf)
{
	HUINT8		startOfArg;
	HUINT8		endOfArg;
	HUINT8		lengthOfArg;
	HUINT8	argExists;

	startOfArg = 0;
	lengthOfArg = 0;

	argExists = PositionOfNextNonSpaceInBuffer(*startOfLine, &startOfArg);
	if (argExists)
	{
		/* this will set endOfArg to the first char AFTER the end of the argument */
		endOfArg = startOfArg;
		argExists = PositionOfNextSpaceInBuffer(*startOfLine, &endOfArg);

		/* this does NOT copy the null character into the resultBuffer */
		lengthOfArg = endOfArg-startOfArg;
		VK_memcpy((char*)resultBuf, (char*)((*startOfLine)+startOfArg), lengthOfArg * sizeof(HUINT8));
		resultBuf[lengthOfArg] = '\0';

		/* change startOfLine to point to after the argument */
		*startOfLine = (*startOfLine) + endOfArg;
	}
	else /* if argument does not exist, set resultBuf to null string */
	{
		resultBuf[0] = '\0';
	}

	return lengthOfArg;
}
char *CMD_AddString(char *pA, char *pB, int lengthB, int mode)
{
	char *pReturn = NULL;
	if ( pB == NULL || lengthB == 0 )
	{
		return NULL;
	}
	if ( pA == NULL )
	{
		pReturn = DD_MEM_Alloc(lengthB+1);
		pReturn[lengthB] = 0;
		VK_MEM_Memcpy((void *)&pReturn[0], (void *)&pB[0], lengthB);
		return pReturn;
	}
	else if ( mode == 1 && pA != NULL )
	{
		DD_MEM_Free(pA);
		pReturn = DD_MEM_Alloc(lengthB+1);
		pReturn[lengthB] = 0;
		VK_MEM_Memcpy((void *)&pReturn[0], (void *)&pB[0], lengthB);
		return pReturn;
	}
	else
	{
		unsigned int lengthA = 0;
		lengthA = VK_strlen(pA);
		pReturn = DD_MEM_Alloc(lengthA+lengthB+1);
		pReturn[lengthA+lengthB] = 0;
		VK_MEM_Memcpy((void *)&pReturn[0], (void *)&pA[0], lengthA);
		VK_MEM_Memcpy((void *)&pReturn[lengthA], (void *)&pB[0], lengthB);
		DD_MEM_Free(pA);
		return pReturn;
	}
}

static pfn_cmdhwtest_ProgressbarUI sProgressBarUIFunc = NULL;
static pfn_cmdhwtest_InfoUI sInfoUIFunc = NULL;
static pfn_cmdhwtest_MsgUI sMsgUIFunc = NULL;
static pfn_cmdhwtest_PVRGlobalCallback sPVRReadyCallback = NULL;
static pfn_cmdhwtest_demuxCallback sDemuxCallback = NULL;
static pfn_cmdhwtest_TimeoutInfoUI sTimeoutInfoUIFunc = NULL;

pfn_cmdhwtest_ProgressbarUI CMD_GetProgressbarUI(void)
{
	return sProgressBarUIFunc;
}
pfn_cmdhwtest_InfoUI CMD_GetInfoUI(void)
{
	return sInfoUIFunc;
}
pfn_cmdhwtest_MsgUI CMD_GetMsgUI(void)
{
	return sMsgUIFunc;
}
pfn_cmdhwtest_PVRGlobalCallback CMD_GetPVRTestReadyCallback(void)
{
	return sPVRReadyCallback;
}
pfn_cmdhwtest_demuxCallback CMD_GetDemuxCallback(void)
{
	return sDemuxCallback;
}
pfn_cmdhwtest_TimeoutInfoUI CMD_GetTimeoutInfoUI(void)
{
	return sTimeoutInfoUIFunc;
}



void CMD_RegisterUIFunctions(pfn_cmdhwtest_ProgressbarUI pfnProgress,
							 pfn_cmdhwtest_InfoUI pfnInfo,
							 pfn_cmdhwtest_MsgUI pfnMsg,
							 pfn_cmdhwtest_TimeoutInfoUI pTimeoutInfo)
{
	if ( pfnProgress != NULL )
	{
		sProgressBarUIFunc = pfnProgress;
	}
	if ( pfnInfo != NULL )
	{
		sInfoUIFunc = pfnInfo;
	}
	if ( pfnMsg != NULL )
	{
		sMsgUIFunc = pfnMsg;
	}
	if ( pTimeoutInfo != NULL )
	{
		sTimeoutInfoUIFunc = pTimeoutInfo;
	}
}
void CMD_RegisterPVRTestReadyCallback(pfn_cmdhwtest_PVRGlobalCallback callback)
{
	sPVRReadyCallback = callback;
}
void CMD_RegisterDemuxTestCallback(pfn_cmdhwtest_demuxCallback callback)
{
	sDemuxCallback = callback;
}
void CMD_2HexChTo1Char(char *pGet, char upper, char lower)
{
	unsigned char aTemp = 0x00;
	unsigned char bTemp = 0x00;
	if ( pGet == NULL )
	{
		return;
	}

	switch(upper)
	{
	case '0':
		aTemp = 0x00;
		break;
	case '1':
		aTemp = 0x10;
		break;
	case '2':
		aTemp = 0x20;
		break;
	case '3':
		aTemp = 0x30;
		break;
	case '4':
		aTemp = 0x40;
		break;
	case '5':
		aTemp = 0x50;
		break;
	case '6':
		aTemp = 0x60;
		break;
	case '7':
		aTemp = 0x70;
		break;
	case '8':
		aTemp = 0x80;
		break;
	case '9':
		aTemp = 0x90;
		break;
	case 'a':
	case 'A':
		aTemp = 0xA0;
		break;
	case 'b':
	case 'B':
		aTemp = 0xB0;
		break;
	case 'c':
	case 'C':
		aTemp = 0xC0;
		break;
	case 'd':
	case 'D':
		aTemp = 0xD0;
		break;
	case 'e':
	case 'E':
		aTemp = 0xE0;
		break;
	case 'f':
	case 'F':
		aTemp = 0xF0;
		break;

	}
	switch(lower)
	{
	case '0':
		bTemp = 0x00;
		break;
	case '1':
		bTemp = 0x01;
		break;
	case '2':
		bTemp = 0x02;
		break;
	case '3':
		bTemp = 0x03;
		break;
	case '4':
		bTemp = 0x04;
		break;
	case '5':
		bTemp = 0x05;
		break;
	case '6':
		bTemp = 0x06;
		break;
	case '7':
		bTemp = 0x07;
		break;
	case '8':
		bTemp = 0x08;
		break;
	case '9':
		bTemp = 0x09;
		break;
	case 'a':
	case 'A':
		bTemp = 0x0A;
		break;
	case 'b':
	case 'B':
		bTemp = 0x0B;
		break;
	case 'c':
	case 'C':
		bTemp = 0x0C;
		break;
	case 'd':
	case 'D':
		bTemp = 0x0D;
		break;
	case 'e':
	case 'E':
		bTemp = 0x0E;
		break;
	case 'f':
	case 'F':
		bTemp = 0x0F;
		break;

	}
	*pGet = 0x00;
	*pGet = aTemp | bTemp;
}

#if 0
unsigned int NextArgU32(HUINT8** argBuffer, unsigned int *pul)
{
	HUINT8	argAsString[MAX_INPUT_LENGTH];
	HUINT8		numChars;
	unsigned int		argAsU32;

	numChars = NextArgString(argBuffer, argAsString);

	/* add null to string for benefit of str-to-num routine */
	argAsString[numChars] = '\0';
	argAsU32 = (unsigned int)strtoul((char*)argAsString, (char**)NULL, 0);

	*pul = argAsU32;
	return numChars;
}
#endif

#if defined(CMD_HISTORY)
/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define DEF_HISTORY_SIZE 25
#ifndef DEF_CMDLINE_SIZE
#define DEF_CMDLINE_SIZE 256
#endif

typedef struct {
	int num;
	char cmd[DEF_CMDLINE_SIZE];
} CMD_History_t;

/******************************************************************************
* Global variables and structures
******************************************************************************/


/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */



/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */
static CMD_History_t s_szCmdHistory[DEF_HISTORY_SIZE];
static int s_nFirstHistIndex = 0;
static int s_nLastHistIndex = 0;

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */
static void CMD_memCopy(char *src, char *dest, int size);
static int CMD_getHistIndex(int num, int *index);
static int CMD_getHistIndexWithString(char *cptr, int *index);


/******************************************************************************
* function : CMD_interpreteCommand
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
******************************************************************************/
int CMD_interpreteCommand(char *cmd)
{
	char *cptr;
	int ret = 0;
	int histNum;
	int foundIndex;
	int nIndex = s_nLastHistIndex;

	if(nIndex == 0 && s_nFirstHistIndex == 0){
		if(!VK_strcmp(cmd,"!!")){
			CMD_Printf("Warning: No history!\n");
			return 0;
		}
		return 1;
	}


	if(!VK_strcmp(cmd,"!!")){
		if(nIndex == 0)
			nIndex = DEF_HISTORY_SIZE -1;
		else
			nIndex --;
		CMD_memCopy(s_szCmdHistory[nIndex].cmd, cmd, VK_strlen(s_szCmdHistory[nIndex].cmd));
		CMD_Printf("%s\n", cmd);
	}
	else if(cmd[0] == '!' && cmd[1] == '-'){
		cptr = (char *)&cmd[2];

		VK_sscanf(cptr, "%d", &histNum);
		if(ret != 1) {
			CMD_Printf("Warning: should be number \"%s\"\n", cptr);
			return 0;
		}

		histNum = s_nCmdCount - histNum;

		ret = CMD_getHistIndex(histNum, &foundIndex);

		if(ret == 0){
			CMD_Printf("Warning: out of bound\n");
			return 0;
		}

		CMD_memCopy(s_szCmdHistory[foundIndex].cmd, cmd, VK_strlen(s_szCmdHistory[foundIndex].cmd));
		CMD_Printf("%s\n", cmd);
	}
	else if(cmd[0] == '!'){
		cptr = (char *)&cmd[1];

		VK_sscanf(cptr, "%d", &histNum);
		if(ret == 1){
			ret = CMD_getHistIndex(histNum, &foundIndex);

			if(ret == 0){
				CMD_Printf("Warning: out of bound\n");
				return 0;
			}

			CMD_memCopy(s_szCmdHistory[foundIndex].cmd, cmd, VK_strlen(s_szCmdHistory[foundIndex].cmd));
			CMD_Printf("%s\n", cmd);
		}
		else {
			ret = CMD_getHistIndexWithString(cptr, &foundIndex);
			if(ret == 0){
				CMD_Printf("Warning: cannot find \"%s\" in history\n", cptr);
				return 0;
			}
			CMD_memCopy(s_szCmdHistory[foundIndex].cmd, cmd, VK_strlen(s_szCmdHistory[foundIndex].cmd));
			CMD_Printf("%s\n", cmd);
		}
	}

	return 1;
}

/******************************************************************************
* function : CMD_pushHistory
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
******************************************************************************/
void CMD_pushHistory(char *cmd)
{

	s_szCmdHistory[s_nLastHistIndex].num = s_nCmdCount;
	CMD_memCopy(cmd, s_szCmdHistory[s_nLastHistIndex].cmd, VK_strlen(cmd));

	s_nLastHistIndex ++;
	s_nLastHistIndex %= DEF_HISTORY_SIZE;
	if(s_nLastHistIndex == s_nFirstHistIndex) {
		s_nFirstHistIndex ++;
		s_nFirstHistIndex %= DEF_HISTORY_SIZE;
	}
	s_nCmdCount ++; // increase command count
	s_szCmdHistory[s_nLastHistIndex].num = s_nCmdCount;
}

/******************************************************************************
* function : CMD_getHistory
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
******************************************************************************/
int CMD_getHistory(char *cmd, int num, int* len)
{
	int nIndex;
	int ret;

	ret = CMD_getHistIndex(num, &nIndex);
	if(ret == 0) {
		return 0;
	}

	*len = VK_strlen(s_szCmdHistory[nIndex].cmd);

	CMD_memCopy(s_szCmdHistory[nIndex].cmd, cmd, *len);

	return 1;
}

/******************************************************************************
* function : CMD_getHistSize
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
******************************************************************************/
int CMD_getHistSize(void)
{
	return DEF_HISTORY_SIZE;
}

/******************************************************************************
* function : CMD_printHistory
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
******************************************************************************/
void CMD_printHistory(int num)
{
	int nIndex;
	int done = 0;

	nIndex = s_nFirstHistIndex;

	if(nIndex == 0 && s_nLastHistIndex == 0)
		return;

	if(num >= DEF_HISTORY_SIZE)
		num = DEF_HISTORY_SIZE -1;

	if(num >= s_nCmdCount)
		num = s_nCmdCount;

	nIndex = s_nCmdCount - num;
	nIndex %= DEF_HISTORY_SIZE;


	while(!done) {
		CMD_Printf("%d : %s\n", s_szCmdHistory[nIndex].num, s_szCmdHistory[nIndex].cmd);
		nIndex ++;
		nIndex %= DEF_HISTORY_SIZE;
		if(nIndex == s_nLastHistIndex)
			done = 1;
	}
}

/******************************************************************************
* function : CMD_memCopy
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
******************************************************************************/
static void CMD_memCopy(char *src, char *dest, int size)
{
	int i;
	for(i=0;i<size;i++){
		dest[i] = src[i];
	}
	dest[size] = '\0';
}


/******************************************************************************
* function : CMD_getHistIndex
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
******************************************************************************/
static int CMD_getHistIndex(int num, int *nIndex)
{
	int done = 0;
	int i;

	if(num >= s_szCmdHistory[s_nFirstHistIndex].num && num < s_szCmdHistory[s_nLastHistIndex].num) {
	}
	else {
		return 0;
	}

	i = s_nFirstHistIndex;
	while(!done){
		if(s_szCmdHistory[i].num == num)
			done = 1;
		else {
			i++;
			i %= DEF_HISTORY_SIZE;

			if(i == s_nLastHistIndex)
				return 0;
		}
	}

	*nIndex = i;

	return 1;
}

/******************************************************************************
* function : CMD_getHistIndexWithString
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
******************************************************************************/
static int CMD_getHistIndexWithString(char *cptr, int *nIndex)
{
	int done = 0;
	int found;
	int i;
	unsigned int j;
	int endIndex;

	if(s_nLastHistIndex == 0)
		i = DEF_HISTORY_SIZE -1;
	else
		i = s_nLastHistIndex - 1;

	if(s_nFirstHistIndex == 0)
		endIndex = DEF_HISTORY_SIZE -1;
	else
		endIndex = s_nFirstHistIndex - 1;

	if(i == endIndex)
		return 0;

	while(!done) {
		found = 1;
		for(j=0;j< VK_strlen(cptr); j++){
			if(cptr[j] != s_szCmdHistory[i].cmd[j]) {
				found = 0;
				break;
			}
		}
		if(found)
			done = 1;
		else {
			if(i == 0)
				i = DEF_HISTORY_SIZE -1;
			else
				i --;

			if(i == endIndex)
				done = 1;
		}
	}

	if(found) {
		*nIndex = i;
		return 1;
	}

	return 0;
}


#endif

