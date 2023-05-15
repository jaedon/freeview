/**
	@file     port_itk_console.c
	@brief    ITK Host Console Module.

	Description: ITK Host Console Interface. Debug, monitoring?\n
	Module: mw/mheg/itk												\n

	Copyright (c) 2008 HUMAX Co., Ltd.									\n
	All rights reserved.													\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include "port_itk_main.h"
#include "port_itk_console.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#ifdef	CONFIG_DEBUG
#define ITK_CONSOLE_DEBUG
#endif

#define NullPrint	if (0) ((int (*)(char *, ...)) 0)

#ifdef CONFIG_DEBUG
#define ItkPrint							HxLOG_Print
#else
#define ItkPrint							NullPrint
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define	ITK_CONSOLE_SIZE_MAX	256

#define	ITK_CONSOLE_CMD_LEN		16
#define	ITK_CONSOLE_HELP_LEN	512


/** @brief itk console command type */
typedef struct tag_ITK_CONSOLE_CMD_T
{
	char  	szCmd[ITK_CONSOLE_CMD_LEN];
	void		(*fnToDo)(void *context, int argc, char **argv);
	void		*pvContext;
	char	szHelp[ITK_CONSOLE_HELP_LEN];
} ITK_CONSOLE_CMD_T;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef	ITK_CONSOLE_DEBUG
HBOOL	g_bItkConsoleDebug = FALSE;
#endif

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#ifdef	ITK_CONSOLE_DEBUG
ITK_CONSOLE_CMD_T s_itkConsoleCmds[ITK_CONSOLE_SIZE_MAX];
#endif

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#ifdef	ITK_CONSOLE_DEBUG
static void local_itk_console_remove(const char *cmd)
{
	unsigned short	i;
	int	found = 0;

	/* find commands in static array which has been matched */
	for (i = 0; i < ITK_CONSOLE_SIZE_MAX; i++)
	{
		if (s_itkConsoleCmds[i].fnToDo != NULL)
		{
			if (strcasecmp((const void *)s_itkConsoleCmds[i].szCmd, (const void *)cmd) == 0)
			{
				ItkPrint("Found Command - %s\n\r", cmd);
				found = 1;
				break;
			}
		}
	}

	/* commands array is full */
	if (i == ITK_CONSOLE_SIZE_MAX || found == 0)
	{
		ItkPrint("Not Found Command - %s\n\r", cmd);
		return;
	}

	/* remove command word */
	s_itkConsoleCmds[i].fnToDo	= NULL;
	s_itkConsoleCmds[i].pvContext	= NULL;

	// initialize buffer
	memset((void *)s_itkConsoleCmds[i].szCmd, 0, (size_t)ITK_CONSOLE_CMD_LEN);
	memset((void *)s_itkConsoleCmds[i].szHelp, 0, (size_t)ITK_CONSOLE_HELP_LEN);
}
#endif

#ifdef	ITK_CONSOLE_DEBUG
static int local_itk_console_add(const char *cmd, itk_simon_callback_t callback, void *context, const char *help)
{

	unsigned short	i;
	size_t	len;

	/* skip commands in static array which has been defined already */
	for (i = 0; (i < ITK_CONSOLE_SIZE_MAX) && (s_itkConsoleCmds[i].fnToDo != NULL); i++)
	{
		if (strcasecmp((const void *)s_itkConsoleCmds[i].szCmd, (const void *)cmd) == 0)
		{
			ItkPrint("Already Defined Command - %s\n\r", cmd);
			return 0;
		}
	}

	/* commands array is full */
	if (i == ITK_CONSOLE_SIZE_MAX)
	{
		ItkPrint("Command Array is Full - %s\n\r", cmd);
		return 0;
	}

	/* add command word */
	s_itkConsoleCmds[i].fnToDo	= callback;
	s_itkConsoleCmds[i].pvContext	= context;

	// initialize buffer
	memset((void *)s_itkConsoleCmds[i].szCmd, 0, (size_t)ITK_CONSOLE_CMD_LEN);
	memset((void *)s_itkConsoleCmds[i].szHelp, 0, (size_t)ITK_CONSOLE_HELP_LEN);

	// copy data to buffer
	len = strlen(cmd);
	if (len < ITK_CONSOLE_CMD_LEN)
	{
		memcpy((void *)s_itkConsoleCmds[i].szCmd, (const void *)cmd, (size_t)len);
	}
	else
	{
		memcpy((void *)s_itkConsoleCmds[i].szCmd, (const void *)cmd, (size_t)(ITK_CONSOLE_CMD_LEN-1));
	}

	len = strlen(help);

	if (len < ITK_CONSOLE_HELP_LEN)
	{
		memcpy((void *)s_itkConsoleCmds[i].szHelp, (const void *)help, (size_t)len);
	}
	else
	{
		memcpy((void *)s_itkConsoleCmds[i].szHelp, (const void *)help, (size_t)(ITK_CONSOLE_HELP_LEN-1));
	}

	// invalid parameter
	//CMD_RegisterWord(s_itkConsoleCmds[i].fnToDo, s_itkConsoleCmds[i].szCmd, help, help);
	return 1;

}
#endif

ITK_EXPORT void ITK_consoleMessage(const char *fmt, ...)
{
#ifdef	ITK_CONSOLE_DEBUG
	if(g_bItkConsoleDebug)
	{
		va_list args;

		va_start(args, fmt);
		printf("[Redkey] Console Start: \n");
		vprintf(fmt, args);
		printf("[Redkey] Stop\n ");
		va_end(args);
	}
#endif
}

ITK_EXPORT void ITK_fatalError(const char *fmt, ...)
{
#ifdef	ITK_CONSOLE_DEBUG
	if(g_bItkConsoleDebug)
	{
		va_list args;

		va_start(args, fmt);
		printf("[Redkey] Fatal Start: \n");
		vprintf(fmt, args);
		printf("[Redkey] Stop\n ");
		va_end(args);

		*(volatile unsigned char *)0;
		// TODO:  reboot or other recovery
	}
#endif
}

ITK_EXPORT itk_errcode_t ITK_addSimonCmd(const char *cmd,
                                         itk_simon_callback_t callback,
                                         void *callbackContext,
                                         const char *helpStr)
{
#ifdef	ITK_CONSOLE_DEBUG
	local_itk_console_add(cmd, callback, callbackContext, helpStr);
#endif
	return ITKE_OK;
}

ITK_EXPORT void ITK_removeSimonCmd(const char *cmd)
{
#ifdef	ITK_CONSOLE_DEBUG
	local_itk_console_remove(cmd);
#endif
}

ITK_EXPORT void ITK_sendSimon(const char *fmt, ...)
{
#ifdef	ITK_CONSOLE_DEBUG
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
#endif
}

#define _____PUBLIC_APIs________________________________________________________________

void PORT_ITK_Console_Init(void)
{

}

void PORT_ITK_Console_Debug(HBOOL bValue)
{
#ifdef	ITK_CONSOLE_DEBUG
	g_bItkConsoleDebug = bValue;
#endif
}

