/*
 * (c) 2011-2012 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/****************************************************************************
	Main source for Loader DVT Application.
****************************************************************************/
#include <signal.h>
#include <hlib.h>

#include <autoconf.h>

//#include <oapi.h>
#include <hapi.h>


#include <ldvt.h>
#include <directfb.h>

#if defined(CONFIG_ENHANCED_SECURITY)
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
//#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif

#include <irldvt_ir_api.h>
#include <irldvt_output.h>
#include <environment.h>

#include <ondk.h>

/****************************************************************************
	Definitions.
****************************************************************************/

#define	LDVT_OSD_HORIZONTAL 1280
#define	LDVT_OSD_VERTICAL	720
#define DVT_PALETTE_MAX_NUM			256
#define	IRDVT_FONT_PATH 			"/usr/fonts/DejaVuSansCondensed.ttf"



//	debug messages
#ifdef CONFIG_DEBUG
#define	PrintDebug					printf
#else
#define	PrintDebug
#endif

#define	CHECK_VALUE(a)				{if (err!=ERR_OK) printf("error : [%s][%d]\n", __FUNCTION__, __LINE__);}

STATIC HINT32	show_ldvt_port=0;
#ifdef	CONFIG_DEBUG
#define	WRAPPORT_PRT(x, y)	( (x == DBG_ASSERT)?(HxLOG_Error y):(((x) & show_ldvt_port)?(WRAP_Print("[VLDVTPort] "),WRAP_Print y ) : 0 ))
#define	WRAPEMPTY_PRT(x, y)		( (x == DBG_ASSERT)?(HxLOG_Error y):(((x) & show_ldvt_port)?(WRAP_Print("\t"), WRAP_Print y ) : 0 ))
#else
#define	WRAPPORT_PRT(x, y)
#define	WRAPEMPTY_PRT(x, y)
#endif

#ifdef	CONFIG_DEBUG
#define 	WRAP_Print						HxLOG_Print
#else
#define 	WRAP_Print
#endif

#define DFBCHECK(x...)                                                     \
               err = x;                                                    \
               if (err != DFB_OK) {                                        \
                    fprintf( stdout, "<'))))><\n<'))))><  GLIB DFB assert !! %s <%d>:\n<'))))><\n", __FILE__, __LINE__ ); \
                    printf("GLIB DFB assert !! \n"); \
                    DirectFBErrorFatal( #x, err );                         \
               }


/****************************************************************************
	Static Variables.
****************************************************************************/
static	HUINT32		_ulDvtAppMsgQueueId, _ulDvtAppTaskId;
static	HINT32		s_nLDvtKeyTaskID = 0;
static  HINT32		s_nLDvtRepeatKey = 0;	/* keytask는 계속 존재하기 때문에 함수 내가 아닌 외부에서 초기화가 필요해서 옮김. */
static 	HINT32		s_KeyID;
static 	ONDKWindowHandle	wHandle;

static	ONDKWindowHandle 		s_hApirldvtWnd = NULL;
static  ONDKSurfaceHandle		s_stdvtScr = NULL;

static  HUINT32				s_unLdvt_WindowId;

static ONDKFontHandle		s_hIrldvtFont;



/****************************************************************************
	Static Functions.
****************************************************************************/
static	void	_runTestAppl(void);
static	void	_ldvtApplMainTask(void *arg1);
static  void 	_ldvtKeyTask(void *args);

void	LDVT_InitAppl(void);

static ONDK_Result_t 	keyTest(HUINT32 ucKeyCode, ONDK_KEY_PressMode_t pressMode, ONDK_KEY_Device_t device, void *keyTypeData);
static void	LDVT_PostMessage(HUINT32 ulMsgType, HUINT32 arg1, HUINT32 arg2, HUINT32 arg3);


//static	void	_pfnChannelCallback(HUINT32 ulTunerGroupId, HUINT32 ulRequestId, PalCh_Event_e event);

/****************************************************************************
	Init OSD and AV environments.
****************************************************************************/
//int	LDVT_InitAppl(int argc, char *argv[])
/* 현재는 obama process에서 호출해서 obama process 기생해서 돈다. task로 돈다. */

void main(HINT32 argc, char ** argv)
{
	signal(SIGPIPE, SIG_IGN);
#if 1
	VK_Init();
#endif

#if 1
	//ONDK_Init(argc, argv);/* windown style 을 결정 한다. */


	ONDK_Init(argc, argv);

	DI_Init();

	//HAPI_Init();

	//HAPI_SetWindowStyle(eAWINDOW_TESTAPP,eHWS_WindowInputAlways,FALSE);

	LDVT_WINDOW_Create();


	ONDK_GWM_Init();




	ONDK_KEY_Active(TRUE);


#endif
	PrintDebug("\n=============================== \n");
	PrintDebug("@@@@ Loader DVT Application @@@@ \n");
	PrintDebug("================================= \n");

	_initEnvironment();
	irldvt_cas_IrLoaderInit();
	//HAPI_SetWindowStyle(eAWINDOW_TESTAPP, eHWS_WindowOwnControl | eHWS_WindowInputAlways, FALSE);
	_runTestAppl();

//	INT_ONDK_GWM_RECV_RegisterKeyCallback(s_hApirldvtWnd,keyTest);

	while(1)
	{
		// to avoid ONDK_Destroy();
		VK_TASK_Sleep(100000);
	}

	ONDK_GWM_Destroy();
	ONDK_Destroy();
}

/****************************************************************************
	System Function : Post Message.
****************************************************************************/

static void	LDVT_PostMessage(HUINT32 ulMsgType, HUINT32 arg1, HUINT32 arg2, HUINT32 arg3)
{
	LMSG	msg;

	msg.ulMsgId = ulMsgType;
	msg.ulArg1  = arg1;
	msg.ulArg2  = arg2;
	msg.ulArg3  = arg3;
	VK_MSG_SendTimeout(_ulDvtAppMsgQueueId, &msg, sizeof(LMSG), 0);
}

/****************************************************************************
	System Function : Get Message QueueId.
****************************************************************************/
HUINT32	LDVT_GetMessageQueueId(void)
{
	return _ulDvtAppMsgQueueId;
}

#if 0
static void	LDVT_SignalNotifier(HSIGNAL_e eSignal, HSIGNALTYPE_e eSignalType, HINT32 size, void *data)
{

/* Nothing to do */
	switch (eSignal)
	{
		case eHSIG_KeyboardCanceled:
			break;

		case eHSIG_KeyboardResult:
			break;
		default:
			break;
	}
}
#endif


/****************************************************************************
	Make Task for Main Application
****************************************************************************/
static	void	_runTestAppl(void)
{
	printf("in runTestAppl\n");
	HINT32		err=1;

	err = (HUINT32)VK_MSG_Create(LDVT_MSGQ_SIZE, sizeof(LMSG), LDVT_MSGQ_NAME, (unsigned long *)&_ulDvtAppMsgQueueId, VK_SUSPENDTYPE_FIFO);
	if(err != ERR_OK)
	{
		printf("==== VK_MSG_Create ERROR ==== \n");
		CHECK_VALUE(err);
	}

	printf("VK_MSG_CREATE DONE\n");

	err = (HUINT32)VK_TASK_Create(_ldvtApplMainTask, LDVT_TASK_PRIORITY, LDVT_TASK_STACK_SIZE, LDVT_TASK_NAME, NULL, (unsigned long *)&_ulDvtAppTaskId, 0);
	if(err !=ERR_OK)
	{
		printf("==== _ldvtApplMainTask Error ==== \n");
		CHECK_VALUE(err);
	}

	err = VK_TASK_Start(_ulDvtAppTaskId);
	if(err !=ERR_OK)
	{
		printf("==== VK_TASK_Start _ulDvtAppTaskId Error ==== \n");
		CHECK_VALUE(err);
	}


	err = (HUINT32)VK_TASK_Create(_ldvtKeyTask, LDVT_TASK_PRIORITY, SIZE_8K, LDVT_KEYTASK_NAME, NULL, (unsigned long *)&s_nLDvtKeyTaskID, 0);
	if(err !=ERR_OK)
	{
		printf("==== Key TASK ERROR ==== \n");
		CHECK_VALUE(err);
	}

	err = VK_TASK_Start(s_nLDvtKeyTaskID);
	if(err !=ERR_OK)
	{
		printf("==== VK_TASK_Start Key task Error ==== \n");
		CHECK_VALUE(err);
	}


}

/****************************************************************************
	Application Main Task
****************************************************************************/
extern HUINT32 TIR_FLASH_LOADER_Init(void);

static	void	_ldvtApplMainTask(void *arg1)
{
	printf("in ldvtAppMainTask ============\n");
	LMSG		msg;

	TIR_FLASH_LOADER_Init();
	printf("==== Flash_LOADER_INIT ==== \n");
	DVTAPP_InitAppl();
	printf("==== Flash_LOADER_INIT finished==== \n");
	while (1)
	{
		//printf("[%s.%d] ------------------- TASK --------------------\n",__FUNCTION__,__LINE__);
		if (VK_MSG_ReceiveTimeout(_ulDvtAppMsgQueueId, &msg, sizeof(LMSG), 10) == VK_OK)
		{
			printf("[%s.%d] ------------------- Get MSG --------------------\n",__FUNCTION__,__LINE__);
			//printf(" convert to systemkey %08x -> %08x, msgtype:0x%08x \n", key, SystemKey, message);
			DVTAPP_ProcessMessage(msg.ulMsgId, msg.ulArg1, msg.ulArg2, msg.ulArg3);
		}
		else
		{
			DVTAPP_RedrawScreen();
		}
		//sleep(2);
	}
}

static  void 	LDVT_SendSignalToHama(HInputEvent_t *pHEvent, HBOOL bconsume)
{
//	EMPTY_PRT(4, ("%s\n", (bconsume == TRUE)?"Consumed":"Pass"));

	if(bconsume)
		HAPI_ReturnKeyEvent(eAWINDOW_TESTAPP, eHINPUTRET_CONSUMED, pHEvent);
	else
		HAPI_ReturnKeyEvent(eAWINDOW_TESTAPP, eHINPUTRET_PASS, pHEvent);
}

static void	LDVT_ConverDfbKeytoSystemKey(HINT32 Inkey , HINT32 *Outkey)
{
	HINT32 nkey=0;

	switch(Inkey)
	{
	case DIKS_NULL: nkey = KEY_UNKNOWN; break;
	case DIKS_BACKSPACE: nkey = KEY_UNKNOWN; break;
	case DIKS_TAB: nkey = KEY_UNKNOWN; break;
	case DIKS_RETURN: nkey = LDVT_KEY_OK; break;
	case DIKS_CANCEL: nkey = KEY_UNKNOWN; break;
	case DIKS_ESCAPE: nkey = KEY_UNKNOWN; break;
	case DIKS_SPACE: nkey = KEY_UNKNOWN; break;
	case DIKS_EXCLAMATION_MARK: nkey = KEY_UNKNOWN; break;
	case DIKS_QUOTATION: nkey = KEY_UNKNOWN; break;
	case DIKS_NUMBER_SIGN: nkey = KEY_UNKNOWN; break;
	case DIKS_DOLLAR_SIGN: nkey = KEY_UNKNOWN; break;
	case DIKS_PERCENT_SIGN: nkey = KEY_UNKNOWN; break;
	case DIKS_AMPERSAND: nkey = KEY_UNKNOWN; break;
	case DIKS_APOSTROPHE: nkey = KEY_UNKNOWN; break;
	case DIKS_PARENTHESIS_LEFT: nkey = KEY_UNKNOWN; break;
	case DIKS_PARENTHESIS_RIGHT: nkey = KEY_UNKNOWN; break;
	case DIKS_ASTERISK: nkey = KEY_UNKNOWN; break;
	case DIKS_PLUS_SIGN: nkey = KEY_UNKNOWN; break;
	case DIKS_COMMA: nkey = KEY_UNKNOWN; break;
	case DIKS_MINUS_SIGN: nkey = KEY_UNKNOWN; break;
	case DIKS_PERIOD: nkey = KEY_UNKNOWN; break;
	case DIKS_SLASH: nkey = KEY_UNKNOWN; break;
	case DIKS_0: nkey = LDVT_KEY_0 ; break;
	case DIKS_1: nkey = LDVT_KEY_1; break;
	case DIKS_2: nkey = LDVT_KEY_2; break;
	case DIKS_3: nkey = LDVT_KEY_3; break;
	case DIKS_4: nkey = LDVT_KEY_4; break;
	case DIKS_5: nkey = LDVT_KEY_5; break;
	case DIKS_6: nkey = LDVT_KEY_6; break;
	case DIKS_7: nkey = LDVT_KEY_7; break;
	case DIKS_8: nkey = LDVT_KEY_8; break;
	case DIKS_9: nkey = LDVT_KEY_9; break;
	case DIKS_COLON: nkey = KEY_UNKNOWN; break;
	case DIKS_SEMICOLON: nkey = KEY_UNKNOWN; break;
	case DIKS_LESS_THAN_SIGN: nkey = KEY_UNKNOWN; break;
	case DIKS_EQUALS_SIGN: nkey = KEY_UNKNOWN; break;
	case DIKS_GREATER_THAN_SIGN: nkey = KEY_UNKNOWN; break;
	case DIKS_QUESTION_MARK: nkey = KEY_UNKNOWN; break;
	case DIKS_AT: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_A: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_B: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_C: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_D: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_E: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_F: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_G: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_H: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_I: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_J: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_K: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_L: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_M: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_N: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_O: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_P: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_Q: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_R: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_S: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_T: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_U: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_V: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_W: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_X: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_Y: nkey = KEY_UNKNOWN; break;
	case DIKS_CAPITAL_Z: nkey = KEY_UNKNOWN; break;
	case DIKS_SQUARE_BRACKET_LEFT: nkey = KEY_UNKNOWN; break;
	case DIKS_BACKSLASH: nkey = LDVT_KEY_BACK; break;
	case DIKS_SQUARE_BRACKET_RIGHT: nkey = KEY_UNKNOWN; break;
	case DIKS_CIRCUMFLEX_ACCENT: nkey = KEY_UNKNOWN; break;
	case DIKS_UNDERSCORE: nkey = KEY_UNKNOWN; break;
	case DIKS_GRAVE_ACCENT: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_A: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_B: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_C: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_D: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_E: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_F: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_G: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_H: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_I: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_J: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_K: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_L: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_M: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_N: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_O: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_P: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_Q: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_R: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_S: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_T: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_U: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_V: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_W: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_X: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_Y: nkey = KEY_UNKNOWN; break;
	case DIKS_SMALL_Z: nkey = KEY_UNKNOWN; break;
	case DIKS_CURLY_BRACKET_LEFT: nkey = KEY_UNKNOWN; break;
	case DIKS_VERTICAL_BAR: nkey = KEY_UNKNOWN; break;
	case DIKS_CURLY_BRACKET_RIGHT: nkey = KEY_UNKNOWN; break;
	case DIKS_TILDE: nkey = KEY_UNKNOWN; break;
	case DIKS_DELETE: nkey = KEY_UNKNOWN; break;

	case DIKS_CURSOR_LEFT: 		nkey = LDVT_ARROWLEFT; break;
	case DIKS_CURSOR_RIGHT: 	nkey = LDVT_ARROWRIGHT; break;
	case DIKS_CURSOR_UP: 		nkey = LDVT_ARROWUP; break;
	case DIKS_CURSOR_DOWN: 		nkey = LDVT_ARROWDOWN; break;
	//case DIKS_CURSOR_UP: 		nkey = KEY_ARROWUP; break;
	//case DIKS_CURSOR_DOWN: 		nkey = KEY_ARROWDOWN; break;
	case DIKS_INSERT: nkey = KEY_UNKNOWN; break;
	case DIKS_END: nkey = KEY_UNKNOWN; break;
	case DIKS_PAGE_UP: nkey = KEY_PAGEUP ; break;
	case DIKS_PAGE_DOWN: nkey = KEY_PAGEDOWN; break;
	case DIKS_PRINT: nkey = KEY_UNKNOWN; break;
	case DIKS_PAUSE: nkey = KEY_PAUSE; break;
	case DIKS_OK: nkey = LDVT_KEY_OK; break;
	case DIKS_SELECT: nkey = KEY_UNKNOWN; break;
	case DIKS_GOTO: nkey = KEY_UNKNOWN; break;
	case DIKS_POWER: nkey = KEY_STANDBY; break;
	case DIKS_POWER2: nkey = KEY_UNKNOWN; break;
	case DIKS_OPTION: nkey = KEY_OPT; break;
	case DIKS_MENU: nkey = KEY_MENU; break;
	//case DIKS_HELP: nkey = KEY_HELP; break;
	case DIKS_INFO: nkey = KEY_INFO ; break;
	case DIKS_TIME: nkey = KEY_UNKNOWN; break;
	case DIKS_VENDOR: nkey = KEY_UNKNOWN; break;

	case DIKS_ARCHIVE: nkey = KEY_UNKNOWN; break;
	case DIKS_PROGRAM: nkey = KEY_UNKNOWN; break;
	case DIKS_CHANNEL: nkey = KEY_UNKNOWN; break;
	case DIKS_FAVORITES: nkey = KEY_UNKNOWN; break;
	case DIKS_EPG: nkey = KEY_UNKNOWN; break;
	case DIKS_PVR: nkey = KEY_UNKNOWN; break;
	case DIKS_MHP: nkey = KEY_UNKNOWN; break;
//case DIKS_LANGUAGE: nkey = KEY_LANGUAGE; break;
	case DIKS_TITLE: nkey = KEY_UNKNOWN; break;
	case DIKS_SUBTITLE: nkey = KEY_UNKNOWN; break;
	case DIKS_ANGLE: nkey = KEY_UNKNOWN; break;
	case DIKS_ZOOM: nkey = KEY_UNKNOWN; break;
	case DIKS_MODE: nkey = KEY_UNKNOWN; break;
	case DIKS_KEYBOARD: nkey = KEY_UNKNOWN; break;
	case DIKS_PC: nkey = KEY_UNKNOWN; break;
	case DIKS_SCREEN: nkey = KEY_UNKNOWN; break;

	case DIKS_TV: nkey = KEY_UNKNOWN; break;
	case DIKS_TV2: nkey = KEY_UNKNOWN; break;
	case DIKS_VCR: nkey = KEY_UNKNOWN; break;
	case DIKS_VCR2: nkey = KEY_UNKNOWN; break;
	case DIKS_SAT: nkey = KEY_UNKNOWN; break;
	case DIKS_SAT2: nkey = KEY_UNKNOWN; break;
	case DIKS_CD: nkey = KEY_UNKNOWN; break;
	case DIKS_TAPE: nkey = KEY_UNKNOWN; break;
	case DIKS_RADIO: nkey = KEY_UNKNOWN; break;
	case DIKS_TUNER: nkey = KEY_UNKNOWN; break;
	case DIKS_PLAYER: nkey = KEY_UNKNOWN; break;
	case DIKS_TEXT: nkey = KEY_UNKNOWN; break;
	case DIKS_DVD: nkey = KEY_UNKNOWN; break;
	case DIKS_AUX: nkey = KEY_UNKNOWN; break;
	case DIKS_MP3: nkey = KEY_UNKNOWN; break;
	case DIKS_PHONE: nkey = KEY_UNKNOWN; break;
	case DIKS_AUDIO: nkey = KEY_UNKNOWN; break;
	case DIKS_VIDEO: nkey = KEY_UNKNOWN; break;

	case DIKS_INTERNET: nkey = KEY_UNKNOWN; break;
	case DIKS_MAIL: nkey = KEY_UNKNOWN; break;
	case DIKS_NEWS: nkey = KEY_UNKNOWN; break;
	case DIKS_DIRECTORY: nkey = KEY_UNKNOWN; break;
	case DIKS_LIST: nkey = KEY_UNKNOWN; break;
	case DIKS_CALCULATOR: nkey = KEY_UNKNOWN; break;
	case DIKS_MEMO: nkey = KEY_UNKNOWN; break;
	case DIKS_CALENDAR: nkey = KEY_UNKNOWN; break;
	case DIKS_EDITOR: nkey = KEY_UNKNOWN; break;

	case DIKS_RED: nkey = LDVT_KEY_RED ; break;
	case DIKS_GREEN: nkey = LDVT_KEY_GREEN; break;
	case DIKS_YELLOW: nkey = LDVT_KEY_YELLOW; break;
	case DIKS_BLUE: nkey = LDVT_KEY_BLUE; break;

	case DIKS_CHANNEL_UP: nkey = KEY_CH_PLUS; break;
	case DIKS_CHANNEL_DOWN: nkey = KEY_CH_MINUS; break;
	case DIKS_BACK: nkey = LDVT_KEY_BACK; break;
	case DIKS_FORWARD: nkey = KEY_FORWARD; break;
	case DIKS_FIRST: nkey = KEY_UNKNOWN; break;
	case DIKS_LAST: nkey = LDVT_KEY_BACK; break;
	case DIKS_VOLUME_UP: nkey = KEY_VOLUMEUP ; break;
	case DIKS_VOLUME_DOWN: nkey = KEY_VOLUMEDOWN; break;
	case DIKS_MUTE: nkey = KEY_MUTE; break;
	case DIKS_AB: nkey = KEY_UNKNOWN; break;
	case DIKS_PLAYPAUSE: nkey = KEY_UNKNOWN; break;
	case DIKS_PLAY: nkey = KEY_UNKNOWN; break;
	case DIKS_STOP: nkey = KEY_UNKNOWN; break;
	case DIKS_RESTART: nkey = KEY_UNKNOWN; break;
	case DIKS_SLOW: nkey = KEY_SLOW; break;
	case DIKS_FAST: nkey = KEY_UNKNOWN; break;
	case DIKS_RECORD: nkey = KEY_UNKNOWN; break;
	case DIKS_EJECT: nkey = KEY_UNKNOWN; break;
	case DIKS_SHUFFLE: nkey = KEY_UNKNOWN; break;
	case DIKS_REWIND: nkey = KEY_UNKNOWN; break;
	case DIKS_FASTFORWARD: nkey = KEY_UNKNOWN; break;
	case DIKS_PREVIOUS: nkey = KEY_UNKNOWN; break;
	case DIKS_NEXT: nkey = KEY_NEXT; break;
	case DIKS_BEGIN: nkey = KEY_UNKNOWN; break;

	case DIKS_DIGITS: nkey = KEY_UNKNOWN; break;
	case DIKS_TEEN: nkey = KEY_UNKNOWN; break;
	case DIKS_TWEN: nkey = KEY_UNKNOWN; break;

	case DIKS_BREAK: nkey = KEY_UNKNOWN; break;
	case DIKS_EXIT: nkey = LDVT_KEY_EXIT; break;
	case DIKS_SETUP: nkey = KEY_UNKNOWN; break;

	case DIKS_CURSOR_LEFT_UP: 		 nkey = KEY_ARROWLEFT; break;
	case DIKS_CURSOR_LEFT_DOWN: 	 nkey = KEY_ARROWRIGHT; break;
	case DIKS_CURSOR_UP_RIGHT: 		 nkey = LDVT_ARROWUP; break;
	case DIKS_CURSOR_DOWN_RIGHT: 	 nkey = LDVT_ARROWDOWN; break;

	case DIKS_PIP: nkey = KEY_UNKNOWN; break;
	case DIKS_SWAP: nkey = KEY_UNKNOWN; break;
	case DIKS_MOVE: nkey = KEY_UNKNOWN; break;
	case DIKS_FREEZE: nkey = KEY_UNKNOWN; break;

	case DIKS_F1: nkey = KEY_UNKNOWN; break;
	case DIKS_F2: nkey = KEY_UNKNOWN; break;
	case DIKS_F3: nkey = KEY_UNKNOWN; break;
	case DIKS_F4: nkey = KEY_UNKNOWN; break;
	case DIKS_F5: nkey = KEY_UNKNOWN; break;
	case DIKS_F6: nkey = KEY_UNKNOWN; break;
	case DIKS_F7: nkey = KEY_UNKNOWN; break;
	case DIKS_F8: nkey = KEY_UNKNOWN; break;
	case DIKS_F9: nkey = KEY_UNKNOWN; break;
	case DIKS_F10: nkey = KEY_UNKNOWN; break;
	case DIKS_F11: nkey = KEY_UNKNOWN; break;
	case DIKS_F12: nkey = KEY_UNKNOWN; break;

	case DIKS_SHIFT: nkey = KEY_UNKNOWN; break;
	case DIKS_CONTROL: nkey = KEY_UNKNOWN; break;
	case DIKS_ALT: nkey = KEY_UNKNOWN; break;
	case DIKS_ALTGR: nkey = KEY_UNKNOWN; break;
	case DIKS_META: nkey = KEY_UNKNOWN; break;
	case DIKS_SUPER: nkey = KEY_UNKNOWN; break;
	case DIKS_HYPER: nkey = KEY_UNKNOWN; break;


	case DIKS_CAPS_LOCK: nkey = KEY_UNKNOWN; break;
	case DIKS_NUM_LOCK: nkey = KEY_UNKNOWN; break;
	case DIKS_SCROLL_LOCK: nkey = KEY_UNKNOWN; break;

	case DIKS_DEAD_ABOVEDOT: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_ABOVERING: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_ACUTE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_BREVE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_CARON: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_CEDILLA: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_CIRCUMFLEX: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_DIAERESIS: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_DOUBLEACUTE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_GRAVE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_IOTA: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_MACRON: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_OGONEK: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_SEMIVOICED_SOUND: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_TILDE: nkey = KEY_UNKNOWN; break;
	case DIKS_DEAD_VOICED_SOUND: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM0: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM1: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM2: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM3: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM4: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM5: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM6: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM7: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM8: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM9: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM10: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM11: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM12: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM13: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM14: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM15: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM16: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM17: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM18: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM19: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM20: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM21: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM22: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM23: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM24: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM25: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM26: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM27: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM28: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM29: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM30: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM31: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM32: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM33: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM34: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM35: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM36: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM37: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM38: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM39: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM40: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM41: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM42: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM43: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM44: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM45: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM46: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM47: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM48: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM49: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM50: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM51: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM52: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM53: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM54: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM55: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM56: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM57: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM58: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM59: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM60: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM61: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM62: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM63: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM64: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM65: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM66: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM67: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM68: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM69: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM70: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM71: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM72: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM73: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM74: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM75: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM76: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM77: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM78: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM79: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM80: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM81: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM82: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM83: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM84: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM85: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM86: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM87: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM88: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM89: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM90: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM91: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM92: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM93: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM94: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM95: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM96: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM97: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM98: nkey = KEY_UNKNOWN; break;
	case DIKS_CUSTOM99: nkey = KEY_UNKNOWN; break;
	//case DIKS_ENTER : nkey = LDVT_KEY_OK; break;

	default:
		printf("-----------it think it's unknown key\n\n\n");
		nkey = KEY_UNKNOWN;
		break;
	}

	*Outkey=(HINT32)nkey;

}

static void	LDVT_UsedKey(HInputEvent_t *pHEvent)
{
	//printf("\n\n\nLDVT_UsedKey called\n\n\n\n");
	HInputEvent_t	*pKeyEvent;
	HINT32			message;
	HINT32			key, SystemKey;
	static HINT16 	keyBackup = 0;

	pKeyEvent = pHEvent;

//	PrintDebug("%s() %s compare 0x%08x,down:up(0x%08x:0x%08x) \n", __FUNCTION__, (pKeyEvent->type == DIET_KEYPRESS)?"KEYPRESS":"KEYRELEASE", pKeyEvent->key_symbol,LDVT_VKEY_DOWN,LDVT_VKEY_UP);
	message = (pKeyEvent->type == DIET_KEYRELEASE) ? LDVT_MSG_KEYUP : LDVT_MSG_KEYDOWN;
	key = pKeyEvent->key_symbol;

	if(message == LDVT_MSG_KEYDOWN)
	{
		printf("===========MSG_KEYDOWN===========\n");
		s_nLDvtRepeatKey += 1;
		keyBackup = key;
	}
	else
	{
		s_nLDvtRepeatKey -= 1;
	}

	if ((s_nLDvtRepeatKey > 1) || (s_nLDvtRepeatKey < 0) ||
		((message == LDVT_MSG_KEYUP) && (keyBackup != key)))
	{
		LDVT_SendSignalToHama(pHEvent, TRUE);
		s_nLDvtRepeatKey = 0;
		return;
	}

	switch (key)
	{
		case LDVT_VKEY_POWER:
			LDVT_SendSignalToHama(pHEvent, FALSE);
			break;

		case LDVT_VKEY_BLUE:
		case LDVT_VKEY_RED :
		case LDVT_VKEY_YELLOW:
		case LDVT_VKEY_GREEN:

		case LDVT_VKEY_0:
		case LDVT_VKEY_1:
		case LDVT_VKEY_2:
		case LDVT_VKEY_3:
		case LDVT_VKEY_4:
		case LDVT_VKEY_5:
		case LDVT_VKEY_6:
		case LDVT_VKEY_7:
		case LDVT_VKEY_8:
		case LDVT_VKEY_9:
		case LDVT_VKEY_11:
		case LDVT_VKEY_12:

		case LDVT_VKEY_UP:
		case LDVT_VKEY_DOWN:
		case LDVT_VKEY_LEFT:
		case LDVT_VKEY_RIGHT:

		case LDVT_VKEY_OK:
		case LDVT_VKEY_EXIT:
		case LDVT_VKEY_BACK:
			LDVT_ConverDfbKeytoSystemKey(key,&SystemKey);
			printf(" convert to systemkey %08x -> %08x, msgtype:0x%08x \n", key, SystemKey, message);
			LDVT_PostMessage((HUINT32)message, (HUINT32)SystemKey,0,0);
			LDVT_SendSignalToHama(pHEvent, TRUE);
			break;

		default:
			LDVT_SendSignalToHama(pHEvent, TRUE);
			break;
	}
}


/****************************************************************************
	Key Task
****************************************************************************/

static void _ldvtKeyTask(void *args)
{
	//printf("\n\n\n in _ldvtKeyTask \n\n\n");
	//printf("[%s.%d] ------------------- TASK --------------------\n",__FUNCTION__,__LINE__);
	//INT_ONDK_GWM_RECV_RegisterKeyCallback(s_hApirldvtWnd,keyTest);

	HAPI_Init();
	HAPI_SetWindowStyle(eAWINDOW_TESTAPP,eHWS_WindowInputAlways,FALSE);
	/*******************************************************
	리모콘에서 사용할 키들을 등록 한다.
	remoteapp.c 에서 날라오는 RCU key 값을 사용 할 수 잇다.
	********************************************************/
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_MENU,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_LAST,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_EXIT,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_0,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_1,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_2,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_3,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_4,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_5,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_6,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_7,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_8,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_9,TRUE);

	/***********************************************
	MW 나 WEP에서 받아야하는 시그날을 연결 해논다.
	관련 module 에서 data를 얻을시에 사용 한다.
	************************************************/
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_CURSOR_LEFT,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_CURSOR_RIGHT,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_CURSOR_UP,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_CURSOR_DOWN,TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP,DIKS_RETURN,TRUE);

	static HInputEvent_t hEvent;
		while (1)
			{
				if (HAPI_GetInputEvent(eAWINDOW_TESTAPP, &hEvent, -1) == TRUE)
				{
					//printf("\n\n\nKey Receieved!!!!!!!!!!!\n\n\n\n");
					LDVT_UsedKey(&hEvent);
					//HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_PASS, &hEvent);
				}
			}



#if 0 // TODO: 다른 방식으로 구현 필요.
	HAPI_Register("ldvtHapi", 40, 0x4000, LDVT_SignalNotifier);
	HAPI_HookKeyEvent(eAWINDOW_TESTAPP);

//	HAPI_RegisterAcceptableKey(eAWINDOW_TESTAPP, (HUINT32)DIKS_POWER);	/* 다른 키를 받지 않도록 하기 위함. */

	while (1)
	{
		if (HAPI_GetInputEvent(eAWINDOW_TESTAPP, &hEvent, 0) == TRUE)
		{
			LDVT_UsedKey(&hEvent);
		}
		sleep(100);
	}

	HAPI_UnhookKeyEvent(eAWINDOW_TESTAPP);
	HAPI_UnRegisterNotifier(LDVT_SignalNotifier);
#endif

}

#if 0
static	void	_pfnKeyCallback(HUINT32 ucKeyCode, KEY_PressMode_t pressMode, KEY_Device_t device)
{
	if (pressMode == eKeyPress_DOWN)
	{
#if defined(CONFIG_APUI_DIGITURK)
		if(ucKeyCode == KEY_BACK)
		{
			ucKeyCode = KEY_EXIT;
		}
#endif
		LDVT_PostMessage(LDVT_MSG_KEYDOWN, ucKeyCode, device, VK_TIMER_GetSystemTick());
	}
}
#endif

/****************************************************************************
	Channel Callback Functions
****************************************************************************/
/*
void _pfnChannelCallback(HUINT32 ulTunerGroupId, HUINT32 ulRequestId, PalCh_Event_e event)
{
	LDVT_PostMessage(LDVT_MSG_CHANNEL, ulTunerGroupId, ulRequestId, event);
}
*/

#define _______LDVT_WINDOW_FUNCTION____________

void	LDVT_WINDOW_Create()
{
	printf("\n============ LDVT_WINDOW_Create =============== \n");

	//ONDK_Init(0,0);



	s_hApirldvtWnd = ONDK_WINDOW_Create(eAWINDOW_TESTAPP, 0, 0, LDVT_OSD_HORIZONTAL, LDVT_OSD_VERTICAL, "ARGB8888", TRUE);

	ONDK_WINDOW_SetShow(s_hApirldvtWnd, FALSE);

	ONDK_WINDOW_SetResolution(s_hApirldvtWnd , LDVT_OSD_HORIZONTAL, LDVT_OSD_VERTICAL);

	ONDK_WINDOW_Update(s_hApirldvtWnd, NULL );

	ONDK_WINDOW_Clear(s_hApirldvtWnd);

	s_stdvtScr = ONDK_SURFACE_GetFromWindow(s_hApirldvtWnd);


	ONDK_SURFACE_Clear(s_stdvtScr);

}
void	LDVT_WINDOW_GetFullSize(ONDK_Rect_t *pRect)
{
	ONDKSurfaceHandle	screen = s_stdvtScr;
	HINT32			w, h;
//	RFVOD_Rect_t	stRect;

	WRAPPORT_PRT(DBG_FUNC, ("%s() +\n", __FUNCTION__));

	if(screen == NULL)
	{
		WRAPEMPTY_PRT(DBG_ASSERT, ("Surface is NULL.\n"));
		goto exit;
	}

	((IDirectFBSurface *)screen)->GetSize ((IDirectFBSurface *)screen, &w, &h);

	pRect->x = 0;
	pRect->y = 0;
	pRect->w = w;
	pRect->h = h;

exit:
	WRAPPORT_PRT(DBG_FUNC, ("%s() -\n", __FUNCTION__));

}

void	LDVT_WINDOW_GetScreenSize(HUINT32 *pulWidth, HUINT32 *pulHeight)
{
	ONDK_Rect_t strect;

	LDVT_WINDOW_GetFullSize(&strect);
	*pulWidth = strect.w;
	*pulHeight = strect.h;
}
void	LDVT_WINDOW_FillRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT32 col)
{
	ONDK_Rect_t rect;

	rect.x=x;
	rect.y=y;
	rect.w=w;
	rect.h=h;

	ONDK_DRAW_FillRect(s_stdvtScr, rect, col);
}

void	LDVT_WINDOW_DrawString(HINT32 x, HINT32 y, HINT32 w, HINT32 h, HINT32 fs, HUINT8 *pszString, HUINT32 col, HINT32 eAlign)
{
	HINT32		nWidth = 0, nHeight=0;
	HINT32		nStartX, nStartY;
	ONDK_Rect_t		stRect;

	s_hIrldvtFont = ONDK_FONT_Create(IRDVT_FONT_PATH,fs,fs);

	switch (eAlign)
	{
		case 0:			//	left
			nStartX = x;
			break;
		case 1:			//	center
			nStartX = x + (w - nWidth) / 2;
			break;
		case 2:
			nStartX = x + w - nWidth;
	}


	nWidth = ONDK_FONT_GetWidth(s_hIrldvtFont);
	nHeight = ONDK_FONT_GetHeight(s_hIrldvtFont);
	nStartY = y + (h - nHeight)/2;
	stRect.x=nStartX;
	stRect.y=nStartY;
	stRect.w=w;
	stRect.h=h;


	ONDK_FONT_DrawString(s_stdvtScr,s_hIrldvtFont,pszString,stRect.x,stRect.y,col,eAlign);

	ONDK_FONT_Release(s_hIrldvtFont);

}


void	LDVT_WINDOW_UpdateScreen(HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2)
{
	ONDK_Rect_t	stMRect = {0, };

	stMRect.x = x1;
	stMRect.y = y1;
	stMRect.w = x2;
	stMRect.h = y2;

//	GLIB_ShowWindow(s_hApirldvtWnd, TRUE);
	ONDK_WINDOW_SetShow(s_hApirldvtWnd,TRUE);

	//GLIB_UpdateWindow(s_hApirldvtWnd, &stMRect);
	ONDK_WINDOW_Update(s_hApirldvtWnd, &stMRect);

//	WRAP_AV_SetFullWindow();
}

