/*
*/

#include    <ondk.h>
#include	<oapi.h>
#include	<hapi.h>

#include "bml_gfxpeer.h"





#define		BML_WIDTH				1280
#define		BML_HEIGHT				720

#define		BML_LOAD_INDICATOR_X	85
#define		BML_LOAD_INDICATOR_Y	590
#define		BML_LOAD_INDICATOR_W	338	// app_browser 의 frame 값과 맞추어야 한다.
#define		BML_LOAD_INDICATOR_H	80	// app_browser 의 frame 값과 맞추어야 한다.
#if 0
static	ONDKWindowHandle	m_window;
static	ONDKWindowHandle	m_stillWindow;
#endif

void		bml_mw_init(HUINT32 nWndId, HUINT32 nStillWndId, HUINT32 nDataOSDWndId)
{
	HERROR		hErr = ERR_FAIL;
#if 0	// 실제 하는 일이 없음.
	hErr = APK_BML_InitModule(nWndId, nStillWndId,nDataOSDWndId);
#endif
	if(hErr != ERR_OK)
	{
		// TODO: bml module
	}
}


#if 0 // octo .
HERROR bml_KeyDownEvent(DFBEvent *evt)
{
	HERROR		hErr;
	HBOOL 		consumed;
	DFBInputDeviceKeySymbol symbol = evt->window.key_symbol;

	if(m_IsShow == FALSE && symbol == DIKS_CUSTOM61)
	{

		return HAPI_ReturnDFBKeyEvent(&evt->window, TRUE);
	}
	else if(m_IsShow ==FALSE)
	{
		return HAPI_ReturnDFBKeyEvent(&evt->window, FALSE);
	}

	HxLOG_Print("[%s:%d]key do Consume..[%x]\n",__FUNCTION__,__LINE__,symbol);
	hErr =HAPI_ReturnDFBKeyEvent(&evt->window, TRUE);
	switch(symbol)
	{
		case	DIKS_1:
		case	DIKS_2:
		case	DIKS_3:
		case	DIKS_4:
		case	DIKS_5:
		case	DIKS_6:
		case	DIKS_7:
		case 	DIKS_8:
		case	DIKS_9:

	    	break;
		case DIKS_CURSOR_LEFT:
		case DIKS_CURSOR_RIGHT:
		case DIKS_CURSOR_UP:
		case DIKS_CURSOR_DOWN:

			break;
		case DIKS_RED:
		case DIKS_GREEN:
		case DIKS_YELLOW:
	    case DIKS_BLUE:

	    	break;
		default :

			break;
	}

	return hErr;
}

static void KeyProcess_Task(void *arg)
{
	IDirectFBEventBuffer *peventbuffer =NULL;

	peventbuffer = MLib_GetKeyEventBuffer(m_window);
	if(peventbuffer == NULL) return;

	while(1)
	{
		peventbuffer->WaitForEvent(peventbuffer);
		while (peventbuffer->HasEvent(peventbuffer) == DFB_OK)
		{
			DFBEvent evt;
			peventbuffer->GetEvent(peventbuffer, &evt);
			if(evt.clazz == DFEC_WINDOW)
			{
				switch (evt.window.type)
				{
					case	DWET_KEYDOWN:
						bml_KeyDownEvent(&evt);
						break;
					case	DWET_KEYUP:
						break;
					default:
						break;
				}
				break;
			}
		}
	}
}
#endif

#define	BML_SURFACE_WIDTH		960
#define	BML_SURFACE_HEIGHT		540

ONDKWindowHandle	gBMLPlaneWnd, gStillPlaneWnd, gDataOSDWnd;

/*static	int	bml_status = 0;*/
HUINT8	blit_testData[255*255*4];


enum
{
	NOT_INITIALIZED = 0,
	READY_TO_DRAW,
	RECT_TEST,
	BLIT_TEST,
	STILL_BLIT_TEST,
	STILL_RECT_TEST,
	STRING_TEST,
} ;

int main( int argc, char** argv )
{
	HUINT32 	bmlWindowId;
	HUINT32 	bmlStillWindowId;
	HUINT32 	bmlDataOSDWindowId;

	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

	VK_Init();
	APK_EXE_INIT();
	ONDK_Init( argc, argv );

#if 0
	{
		HULONG	ulKeyEventThreadId;

		VK_TASK_Create(KeyProcess_Task, 40, 0x4000, "AppBmlKeyEventTask", NULL, &ulKeyEventThreadId, 0);
		VK_TASK_Start(ulKeyEventThreadId);
	}
#endif

#if	OCTO_HAS_DFB_Master	// Octo has DFB Master ...
	gBMLPlaneWnd = ONDK_WINDOW_Create(eAWINDOW_BML, 0, 0, BML_SURFACE_WIDTH, BML_SURFACE_HEIGHT, "INDEX8", TRUE);
	ONDK_WINDOW_SetResolution(gBMLPlaneWnd, BML_SURFACE_WIDTH, BML_SURFACE_HEIGHT);

	gStillPlaneWnd = ONDK_WINDOW_Create(eAWINDOW_BMLSTILL, 0, 0, BML_SURFACE_WIDTH, BML_SURFACE_HEIGHT, "ARGB8888", FALSE);
	ONDK_WINDOW_SetResolution(gStillPlaneWnd, BML_SURFACE_WIDTH, BML_SURFACE_HEIGHT);

	gDataOSDWnd = ONDK_WINDOW_Create(eAWINDOW_JAPAN_NATIVE_UI, 0, 0, BML_WIDTH, BML_HEIGHT, "ARGB8888", FALSE);

	// 늦게 show 가 불리는 window 가 위에 표시 된다.
	ONDK_WINDOW_SetShow(gStillPlaneWnd, TRUE);
	ONDK_WINDOW_SetShow(gBMLPlaneWnd, TRUE);
	ONDK_WINDOW_SetShow(gDataOSDWnd, TRUE);


	bmlWindowId = ONDK_WINDOW_GetWindowIDFromWindow(gBMLPlaneWnd);
	bmlStillWindowId = ONDK_WINDOW_GetWindowIDFromWindow(gStillPlaneWnd);
	bmlDataOSDWindowId = ONDK_WINDOW_GetWindowIDFromWindow(gDataOSDWnd);
#else		// Octo has DFB Master ...
#endif	// Octo Has DFB Master...
	bml_mw_init(bmlWindowId, bmlStillWindowId,bmlDataOSDWindowId);

	HAPI_SetWindowStyle(eAWINDOW_BML, eHWS_WindowInputAlways, FALSE);

	while (TRUE)
	{
		VK_TASK_Sleep(50);
	}

	return 0;
}


