/**************************************************************
 *	@file		app_bsautomsg.c
 *	System Keyboard Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/14
 *	@author			humax
 *	@brief			System bsautomsg Application
 **************************************************************/
#include   <ondk.h>
#include	<oapi.h>
#include	<hapi.h>

#if defined(CONFIG_ENHANCED_SECURITY)
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif




/**************************************************************
	internal structures and variables
 **************************************************************/
#ifdef	CONFIG_DEBUG
	#define		DBG(...)		do { if (s_bBsAutoMsgDBG) HxLOG_Print(__VA_ARGS__); } while(0)
#else
	#define		DBG(...)
#endif

#undef		COMMAND_NAME
#define		COMMAND_NAME					"APP_BSAUTOMSG"
#define		HxCMD_PORT_APP_SUB			HxCMD_PORT_BSAUTOMSG// telnet port!!!

#define		BSAUTOMSG_WIDTH				1280
#define		BSAUTOMSG_HEIGHT				720
#define 		BSAUTODISP_MSG_W				(24*18 + 8)
#define 		BSAUTODISP_MSG_H				(24*8 + 3 * 8)

#define 		BSAUTODISP_MSG_LEFT_X			(140 - 40)
#define 		BSAUTODISP_MSG_RIGHT_X		(BSAUTOMSG_WIDTH - (BSAUTODISP_MSG_W + BSAUTODISP_MSG_LEFT_X))
#define 		BSAUTODISP_MSG_CENTER_X		((BSAUTOMSG_WIDTH- BSAUTODISP_MSG_W) / 2)

#define 		BSAUTODISP_MSG_TOP_Y			(110+12) 	/* 아래로 부터 offset */
#define 		BSAUTODISP_MSG_BOTTOM_Y		(BSAUTOMSG_HEIGHT - BSAUTODISP_MSG_TOP_Y)
#define 		BSAUTODISP_MSG_CENTER_Y		(BSAUTOMSG_HEIGHT / 2)

#define 		JAPANESE_FONT_PATH 	"/usr/fonts/JP-ARIB-JGTR01B.ttf"


typedef struct	_BSAUTOMSGData
{
	HBOOL					show;
	APKS_CASBC_BSAutoMsgInfo_t	stBsAutoMsg;
} BSAUTOMSGData_t;

static	APK_HANDLE		hCasbcMain;

static	BSAUTOMSGData_t	s_stBsAutoMsgInfo;

static 	ONDKWindowHandle	hWindow;
static 	ONDKSurfaceHandle 	hSurface;
static 	ONDKFontHandle      	s_bsAutoMsgFont;
//#define TEST_CASE
#if defined(TEST_CASE)
static HUINT32 timerid = 0;
#endif


/**************************************************************
	private functions
 **************************************************************/
#if defined(CONFIG_ENHANCED_SECURITY)
static	void dropUserPrevilegeAndChroot(void)
{
	int i;
	struct passwd *u_info;
	struct group  *g_info;
	struct rlimit rlim;

#if (0)
	/* 1. Close open file descriptors */
	if (getrlimit(RLIMIT_NOFILE, &rlim))
	{
		printf("getrlimit() for RLIMIT_NOFILE failed with error %s\n",
				strerror(errno));
		exit(-1);
	}

	for (i = 0; i < rlim.rlim_max; i++)
	{
		while (close(i) != 0 && errno == EINTR);
	}


	/* 2. Chroot */
	if (chroot("/hmx/octo") == -1)
	{
		printf("\n\t\t =============> APP_ChrootAndDropUserPrevilege : error chroot.....\n");
		exit(-1);
	}
	else
	{
		printf("\n\t\t =============> APP_ChrootAndDropUserPrevilege : succ chroot.....\n");
	}

	if (chdir("/"))
	{
		printf("chdir() failed...\n");
		exit(-1);
	}
#endif


	/* 3. Drop root privileges	to a non-privileged uesr */
	/* get GID and UID */
	u_info = getpwnam("settop");
	g_info = getgrnam("settop");

	if ( (u_info != NULL) && (g_info != NULL) )
	{
		uid_t real, eff, saved;
		gid_t gid = (gid_t)g_info->gr_gid;
		uid_t uid = (uid_t)u_info->pw_uid;

		if (setresgid(gid, gid, gid) != 0)
		{
			HxLOG_Print("Failed changing GID to %d with error %s\n",
						gid, strerror(errno));
			exit(-1);
		}

		if (setresuid(uid, uid, uid) != 0)
		{
			HxLOG_Print("Failed changing UID to %d with error %s\n",
						uid, strerror(errno));
			exit(-1);
		}

		if (getresuid(&real, &eff, &saved) != 0)
		{
			HxLOG_Print("Failed reading UID with error %s\n", strerror(errno));
			exit(-1);
		}

		if (real != uid || eff != uid || saved != uid)
		{
			HxLOG_Print("UID sanity check failed\n");
			exit(-1);
		}

		if (getresgid(&real, &eff, &saved) != 0)
		{
			HxLOG_Print("Failed reading GID with error %s\n", strerror(errno));
			exit(-1);
		}

		if (real != gid || eff != gid || saved != gid)
		{
			HxLOG_Print("GID sanity check failed\n");
			exit(-1);
		}

		HxLOG_Print("Dropped root priviliges. Now running as UID %u GID %u\n", uid, gid);
	}
	else
	{
		exit(-1);
	}
}
#endif

#ifdef	CONFIG_DEBUG

static HBOOL s_bBsAutoMsgDBG = FALSE;
#if 0
static int	CMD_BSAUTOMSG_DBGOn(void *szArgList)
{
	s_bBsAutoMsgDBG = TRUE;

	return HxCMD_OK;
}

static int	CMD_BSAUTOMSG_DBGOff(void *szArgList)
{
	s_bBsAutoMsgDBG = FALSE;

	return HxCMD_OK;
}
#endif
void APBsAutoMsg_CMD_Register(HUINT8 *cmdname)
{
#define	hCmdHandle		"BSAUTOMSG"
//	HxCMD_RegisterGroup(hCmdHandle, "BSAUTOMSG_CMD", "BSAUTOMSG_CMD", "Command for bs auto display message");
}

#endif

/**************************************************************
	Static Function
 **************************************************************/

static	void	makeBsAutoMsgWindow(void)
{
	hWindow = ONDK_WINDOW_Create(eAWINDOW_BSAUTOMSG, 0, 0, BSAUTOMSG_WIDTH, BSAUTOMSG_HEIGHT, "ARGB8888", FALSE);

	ONDK_WINDOW_SetShow(hWindow, FALSE);
	s_stBsAutoMsgInfo.show = TRUE;
	s_bsAutoMsgFont = ONDK_FONT_CreateGreyFont( JAPANESE_FONT_PATH, 24, 24 );

	// TODO: 추후 multiline이 구현되면 windwo의 hSurface를 사용한다.
    #if 1 	// Use WindowSurface
	hSurface = ONDK_SURFACE_GetFromWindow(hWindow);
    #else
	hSurface = ONDK_SURFACE_Create(BSAUTOMSG_WIDTH, BSAUTOMSG_HEIGHT, "ARGB8888");
    #endif
	ONDK_SURFACE_Clear(hSurface);
	ONDK_WINDOW_Update(hWindow, NULL);
}

static void displayAutoDispMsg(APKE_CasBcCtrl_Event_e eLastEvent)
{
	HINT32 	x, y, h;
	HUINT32	strWidth = 0, strHeight = 0, strLine = 0, strVspace = 0;
	static	ONDK_Rect_t		stRect = {0, 0, 0, 0};

	switch(eLastEvent)
	{
		case eCasBcEvent_ShowBsAutoMsg:
			stRect = ONDK_Rect(0, 0, BSAUTOMSG_WIDTH, BSAUTOMSG_HEIGHT);

			h = BSAUTODISP_MSG_H;
			switch (s_stBsAutoMsgInfo.stBsAutoMsg.positionX)
			{
				case CAS_BC_AUTODISPLAY_X_LEFT:
					x = BSAUTODISP_MSG_LEFT_X;
					break;

				case CAS_BC_AUTODISPLAY_X_CENTER:
					x = BSAUTODISP_MSG_CENTER_X;
					break;

				case CAS_BC_AUTODISPLAY_X_RIGHT:
					x = BSAUTODISP_MSG_RIGHT_X;
					break;

				default:
					return;
			}

			switch (s_stBsAutoMsgInfo.stBsAutoMsg.positionY)
			{
				case CAS_BC_AUTODISPLAY_Y_HIGH:
					y = BSAUTODISP_MSG_TOP_Y;
					break;

				case CAS_BC_AUTODISPLAY_Y_CENTER:
					y = BSAUTODISP_MSG_CENTER_Y - h / 2;
					break;

				case CAS_BC_AUTODISPLAY_Y_LOW:
					y = BSAUTODISP_MSG_BOTTOM_Y - h;
					break;

				default:
					return;
			}

			stRect.x = x;
			stRect.y = y;
			stRect.h = h;
			stRect.w = BSAUTODISP_MSG_W;
			strVspace = 0;
			strLine = (HUINT32)ONDK_FONT_GetWidthMultiLine(s_bsAutoMsgFont, s_stBsAutoMsgInfo.stBsAutoMsg.szBsAutoMsg, BSAUTODISP_MSG_W, 0, (HINT32*)&strWidth, (HINT32*)&strHeight);
			if(strLine != 0)
			{
				stRect.w = (HINT32)strWidth;
				stRect.y = stRect.y + stRect.h - (strHeight + (strVspace*strLine));
				stRect.h = strHeight + (strVspace*strLine);
			}
			ONDK_DRAW_FillRect(hSurface, stRect, ONDK_Color(0x7F, 0x00, 0x00, 0x00));
    #if 1 	// Use WindowSurface
			ONDK_FONT_DrawStringForMultiLine(hSurface, s_bsAutoMsgFont, stRect.x,stRect.y,stRect.w,stRect.h, s_stBsAutoMsgInfo.stBsAutoMsg.szBsAutoMsg, strVspace, 0x00FFFFFF);
    #else
			// TODO: multiline이 완성되면 이 작업은 필요 없을 것 같다.
			ONDK_SURFACE_StretchBlit(ONDK_SURFACE_GetFromWindow(hWindow),hSurface,stRect,stRect);
    #endif
			ONDK_WINDOW_Update(hWindow, &stRect);

			/*hide videomessage를 받은 경우에 data는 update하고 그리지는 않는다.*/
			if(s_stBsAutoMsgInfo.show != TRUE)
				return;

			ONDK_WINDOW_SetShow(hWindow, TRUE);
			break;
		case eCasBcEvent_HideBsAutoMsg:
		case eCasBcEvent_ClearBsAutoMsg:
			ONDK_SURFACE_Clear(hSurface);
			ONDK_WINDOW_Update(hWindow, NULL);
			ONDK_WINDOW_SetShow(hWindow, FALSE);
			break;
		default:
			break;

	}

}



/**************************************************************
  Event Handler
 **************************************************************/

static	void bsAutoMsgNotifier(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HxJSON_t					jsParam = (HxJSON_t)ulParam1;
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulSessionHandle = 0;
	APKE_MediaCtrl_Destination_e	eDestination;
	APKE_CasBcCtrl_Event_e		eLastEvent;
	APKS_CASBC_BSAutoMsgInfo_t stBsAutoMsg;

	HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
	hErr = APK_CAS_BC_ParseBsAutoMsgArgs(jsParam, &eDestination, &ulSessionHandle, &stBsAutoMsg);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_CAS_BC_ParseBsAutoMsgArgs returned err\n");
		return;
	}
	eLastEvent = stBsAutoMsg.eLastEvent;

	HxLOG_Print("eDestination %d [%s:%d]\n",eDestination, __FUNCTION__, __LINE__);
	if (eDestination == eMctrlDest_Av0)
	{
		switch (eLastEvent)
		{
			case eCasBcEvent_ShowBsAutoMsg:
			case eCasBcEvent_HideBsAutoMsg:
			case eCasBcEvent_ClearBsAutoMsg:
				HxSTD_MemCopy(&(s_stBsAutoMsgInfo.stBsAutoMsg), &stBsAutoMsg, sizeof(APKS_CASBC_BSAutoMsgInfo_t));
				HxLOG_Print("s_stBsAutoMsgInfo.stBsAutoMsg.msgLength %d\n",s_stBsAutoMsgInfo.stBsAutoMsg.msgLength);
				HxLOG_Print("s_stBsAutoMsgInfo.stBsAutoMsg.szBsAutoMsg %s\n",s_stBsAutoMsgInfo.stBsAutoMsg.szBsAutoMsg);
				displayAutoDispMsg(eLastEvent);
				break;

			default:
				break;
		}

	}

	return;
}

static HERROR	onShowBsAutoMsg(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	if (HxOBJECT_INT(apArgv[0]))
	{
		ONDK_WINDOW_SetShow(hWindow, TRUE);
		s_stBsAutoMsgInfo.show = TRUE;
	} else
	{
		ONDK_WINDOW_SetShow(hWindow, FALSE);
		s_stBsAutoMsgInfo.show = FALSE;
	}

	return ERR_OK;
}

/**************************************************************
	public functions
 **************************************************************/
int		main(int argc, char **argv)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	dropUserPrevilegeAndChroot();
#endif
	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

	VK_Init();
	//	Initialize OAPI and APPKIT
	//APK_CAS_BC_Init();

	ONDK_Init(argc, argv);

	//	Initialize Internal Structure
	HxSTD_MemSet(&s_stBsAutoMsgInfo, 0, sizeof(BSAUTOMSGData_t));


	hCasbcMain = APK_CAS_BC_GetHandle(eCasBcCtrlType_BsAutoMsg, eMctrlDest_Av0);
	APK_CAS_BC_RegisterNotifier(hCasbcMain, bsAutoMsgNotifier);

	HAPI_ConnectSignal("signal:onShowBsAutoMsg", NULL, onShowBsAutoMsg);
//	HAPI_ConnectSignal(eHSIG_GoShutDown);

	makeBsAutoMsgWindow();

#ifdef	CONFIG_DEBUG
	APBsAutoMsg_CMD_Register(COMMAND_NAME);
#endif
	while (1)
	{
		VK_TASK_Sleep(10);
	}
	return 0;
}

