
/**
	@file     rfvod.c
	@brief   rfvod process management code

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/** @brief global header file  */
#include <ondk.h>
#include <oapi.h>
#include <hapi.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	RFVOD_WIDTH			960
#define	RFVOD_HEIGHT			540


#ifdef CONFIG_DEBUG
#define DBG(...)	\
	do	\
	{	\
		if (s_bDebugFlag)	\
			HxLOG_Print(__VA_ARGS__);	\
	} while(0)
#undef	COMMAND_NAME
#define COMMAND_NAME		"RFVOD"
#else
#define DBG(...)
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

ONDKWindowHandle	g_hRfVodWnd;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#ifdef CONFIG_DEBUG
static HBOOL	s_bDebugFlag = FALSE;
#endif
STATIC HUINT32	s_unRfvodWindowId = 0;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

static void	local_rfvod_MakeWindow( int argc, char** argv )
{
#if	OCTO_HAS_DFB_Master	// Octo has DFB Master ...
	g_hRfVodWnd 		= ONDK_WINDOW_Create(eAWINDOW_RFVOD, 0, 0, RFVOD_WIDTH, RFVOD_HEIGHT, "ARGB8888", TRUE);
	s_unRfvodWindowId	= ONDK_WINDOW_GetWindowIDFromWindow(g_hRfVodWnd);
	ONDK_WINDOW_SetResolution(g_hRfVodWnd, RFVOD_WIDTH, RFVOD_HEIGHT);
	ONDK_WINDOW_SetShow(g_hRfVodWnd, TRUE);
	ONDK_WINDOW_Update(g_hRfVodWnd, NULL );
#else		// Octo has DFB Master ...
#endif	// Octo has DFB Master ...
}

#ifdef CONFIG_DEBUG
static int	local_rfvod_SetDebug(void *szArgList)
{
	HUINT8		*aucArg_1=NULL;
	HUINT8		aucStr1[20];
	unsigned long	arg1 = 0;
	char 		*szCmd = (char *)szArgList;

	HxLOG_Trace();

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
		{
			return HxCMD_ERR;
		}

		arg1 = atoi(aucStr1);
	}
	else
	{
		return HxCMD_ERR;
	}

	s_bDebugFlag = arg1;

	return HxCMD_OK;
}

static void local_rfvod_SetCommandRegister(HUINT8 *cmdname)
{
#define	hCmdHandle		"RFVOD_CMD"
	HLIB_CMD_RegisterWord(hCmdHandle,
			local_rfvod_SetDebug,
			"setdbg",						/* keyword */
			"set rfvod module's log",		/*   help  */
			"setdbg 0 or 1");				/*  usage  */
}
#endif

static void		local_rfvod_Init(HUINT32 nWndId)
{
	HERROR		hErr = ERR_FAIL;
#if 0 // no need
	hErr = APK_RFVOD_InitModule(nWndId);
	if(hErr != ERR_OK)
	{
		// TODO:
	}
#endif
}

int main( int argc, char** argv )
{
	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

	VK_Init();
	//	initialize vkernel and oapi
	APK_EXE_INIT();
//	APK_Init(eModule_RFVOD | eSubModule_MASK);

	ONDK_Init( argc, argv );


#ifdef CONFIG_DEBUG
{
	local_rfvod_SetCommandRegister(COMMAND_NAME);
}
#endif

	//	initialize Window
	local_rfvod_MakeWindow(argc, argv);

#if 0
	//	Register key
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_CUSTOM45);		// VOD key
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_ENTER);			// enter
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_BACK);			// back
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_CURSOR_UP);		// up
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_CURSOR_DOWN);	// down
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_CURSOR_LEFT);	// left
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_CURSOR_RIGHT);	// right
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_1);			// 1
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_2);			// 2
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_3);			// 3
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_4);			// 4
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_5);			// 5
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_6);			// 6
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_7);			// 7
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_8);			// 8
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_9);			// 9
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_0);			// 10
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_CUSTOM10);		// 11
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_CUSTOM11);		// 12
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_BLUE);			// blue
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_RED);			// red
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_GREEN);		// green
	HAPI_RegisterAcceptableKey(eAWINDOW_RFVOD, (HUINT32)DIKS_YELLOW);		// yellow

	// TODO: play, stop, pause, FF, REW key 처리 필요한지 확인
#endif

	HAPI_SetWindowStyle(eAWINDOW_RFVOD, eHWS_WindowInputAlways, FALSE);

	local_rfvod_Init(s_unRfvodWindowId); /* octo mw & rfvod engine start */

	while (TRUE)
	{
#if !defined(USE_GROUPMSG)
		OAPI_DispatchMessage("rfvodOapiTask",40,0x4000);
#endif
		VK_TASK_Sleep(50);
	}

	return 0;
}


