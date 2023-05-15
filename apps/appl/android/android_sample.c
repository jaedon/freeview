/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


#include    <ondk.h>
#include	<oapi.h>
#include	<hapi.h>
#include	<hxmsg.h>

#define		WINDOW_WIDTH				1280
#define		WINDOW_HEIGHT				720

// ONDKWindowHandle 는 IDirectFBWindow * 와 동일함
static	ONDKWindowHandle	m_windowHandle;

static	void	signalNotifier(HSIGNAL_e eSignal, HSIGNALTYPE_e eSignalType, HINT32 size, void *data)
{
#if	0
	switch (eSignal)
	{
		case eHSIG_RequestOperation:

			break;

		case eHSIG_RequestStandby:

			break;

		case eHSIG_RequestReboot:

			break;

		default:
			break;
	}
#endif
}

static ONDK_Result_t ONDK_KEY_KeyNotifyCallback(HUINT32 ucKeyCode, ONDK_KEY_PressMode_t pressMode, ONDK_KEY_Device_t device, void *keyTypeData)
{
	HInputRetType_e		consumed = ONDK_MESSAGE_CONSUMED; // 다른 window 로 전달 하지  않음

	switch(ucKeyCode)
	{
		case KEY_BLUE:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (DIKS_BLUE KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (DIKS_BLUE KeyUp)\n");
			}
			break;

		case KEY_RED:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (DIKS_RED KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (DIKS_RED KeyUp)\n");
			}
			break;

		case KEY_GREEN:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (DIKS_GREEN KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (DIKS_GREEN KeyUp)\n");
			}
			break;

		case KEY_YELLOW:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (DIKS_YELLOW KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (DIKS_YELLOW KeyUp)\n");
			}
			break;

		case KEY_OK:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (KEY_OK KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (KEY_OK KeyUp)\n");
			}
			break;

		case KEY_BACK:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (KEY_BACK KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (KEY_BACK KeyUp)\n");
			}
			break;

		case KEY_ARROWUP:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (DIKS_CURSOR_UP KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (DIKS_CURSOR_UP KeyUp)\n");
			}
			break;

		case KEY_ARROWDOWN:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (KEY_ARROWDOWN KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (KEY_ARROWDOWN KeyUp)\n");
			}
			break;

		case KEY_ARROWLEFT:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (DIKS_CURSOR_LEFT KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (DIKS_CURSOR_LEFT KeyUp)\n");
			}
			break;

		case KEY_ARROWRIGHT:
			if (pressMode == eKeyPress_DOWN)
			{
				printf("++++++++++++++++++++++ key press (DIKS_CURSOR_RIGHT KeyDown)\n");
			}
			else if (pressMode == eKeyPress_UP)
			{
				printf("++++++++++++++++++++++ key press (DIKS_CURSOR_RIGHT KeyUp)\n");
			}
			break;

		default:
			printf("++++++++++++++++++++++ key press (0x%X %s)\n", ucKeyCode, (pressMode == eKeyPress_DOWN ? "KeyDown" : "KeyUp"));
			consumed = ONDK_MESSAGE_NONCONSUMED; // 다른 window 로 전달
			break;
	}

	return consumed;
}

static	void	runAndroid(void)
{
	HBOOL			bRunning = TRUE;

	while (bRunning)
	{
		HLIB_STD_TaskSleep(100);
	}
}

int		main(int argc, char **argv)
{
	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

	//	initialize vkernel and oapi
	VK_Init();
	ONDK_Init( argc, argv );
	OAPI_Init();

	APK_EXE_INIT();

	m_windowHandle = ONDK_WINDOW_Create(eAWINDOW_ANDROID, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, "ARGB8888", TRUE);
	ONDK_KEY_RegisterKeyCallback(m_windowHandle, ONDK_KEY_KeyNotifyCallback);
	ONDK_WINDOW_SetShow(m_windowHandle, TRUE);
	ONDK_WINDOW_Update(m_windowHandle, NULL);

	HAPI_Register("androidHapi",40,0x4000,signalNotifier);

	HAPI_SetWindowStyle(eAWINDOW_ANDROID, eHWS_WindowInputAlways, FALSE);


	runAndroid();

	ONDK_Destroy();

	return 0;
}

