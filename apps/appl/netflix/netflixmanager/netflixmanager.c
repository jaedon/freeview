/**************************************************************
 *	@file		NetflixManager.c
 *	Power Manager Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/07
 *	@author			humax
 *	@brief			Power Manager Application
 **************************************************************/

/*
	마지막 POWER관련 관리, 즉 Shutdown관리 대상이 OCTO라는 가정하에
	해당 코드를 재 구성한다. (2012/07/08)
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include 	<sys/signal.h>
#include 	<sys/socket.h>
#include 	<sys/un.h>

#include	<unistd.h>
#include 	<wait.h>
#include 	<fcntl.h>

#include	<directfb.h>
#include	<sawman.h>

#include	<vkernel.h>
#include	<hlib.h>
#include	<oapi.h>
#include	<hapi.h>
#include	<papi.h>
#include	<apk.h>
#if defined(CONFIG_HUMAX_CRASHREPORT)
#include  	<hcrlib.h>
#endif
//#include	<pcdapi.h>

#ifndef WIN32
#include <curl/curl.h>
#endif

#if defined(CONFIG_ENHANCED_SECURITY)
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/resource.h>
#endif

#include <di_flash.h>

#include <netflixmanager.h>
#include "netflixmanager_int.h"

#define		__SUPPORT_USB_LOGGING__				0

#if	__SUPPORT_USB_LOGGING__

static 		HCHAR			s_szUsbLogBuf[8200];
static		HxFILE_t		s_usbLogfile;// = 0;

#undef	HxLOG_Print
#define	HxLOG_Print(fmt,args...)														\
							HxSTD_snprintf(s_szUsbLogBuf, 8200, fmt, ##args);		\
							printf("%s", s_szUsbLogBuf);\
							s_usbLogfile = HLIB_FILE_Open("/media/drive1/nflixman.log", "a");\
							if(s_usbLogfile)							\
							{												\
								HLIB_FILE_Write(s_usbLogfile, s_szUsbLogBuf, HxSTD_StrLen(s_szUsbLogBuf), 1);\
								HLIB_FILE_Close(s_usbLogfile);\
							}

#endif

//#undef	HxLOG_Print
//#define	HxLOG_Print     printf


#define		UDB_SYSTEM_FIRSTTIME_BOOT				"UDB/SYSTEM_bFirstTimeBoot"


#define		NFLIX_APP				"./netflix"
#define		NFLIX_CREDENTIALS 		"/var/lib/humaxtv_user/netflix/manuf_ss.bin"
#define		NFLIX_CREDENTIALS_PATH 	"/var/lib/humaxtv_user/netflix/"

#define		MAX_NFLIX_UI_QUERY_STR		(8 * 1024)	// 8KB
#define		MAX_MFLIX_PAYLOAD_STR		(5 * 1024)	// 5KB

#define		NFLIX_CREDENTIALS_STR 	"--idfile=/var/lib/humaxtv_user/netflix/manuf_ss.bin"

#define		NFLIX_UI_QUERY_STR_NORMAL						"--ui-query-string=source_type=%d&additionalDataUrl=%s"
#define		NFLIX_UI_QUERY_STR_DIAL							"--ui-query-string=source_type=%d&additionalDataUrl=%s&dial=%s"
#define		NFLIX_UI_QUERY_STR_SRC_PAYLOAD					"--ui-query-string=source_type=%d&source_type_payload=%s&additionalDataUrl=%s"
#define		NFLIX_UI_QUERY_STR_DEEP_LINK					"--ui-query-string=source_type=%d&m=%s&additionalDataUrl=%s"

#define		NFLIX_NX_STR_SRC							"source_type=%d&additionalDataUrl=%s"
#define		NFLIX_NX_STR_DIAL							"source_type=%d&additionalDataUrl=%s&dial=%s"
#define		NFLIX_NX_STR_SRC_PAYLOAD					"source_type=%d&source_type_payload=%s&additionalDataUrl=%s"
#define		NFLIX_BARKER_CH_PAYLOAD						"chID=%d&prevChID=%d&nextChID=%d&category=%s"
#define		NFLIX_NX_STR_DEEP_LINK_PAYLOAD					"source_type=%d&m=%s&additionalDataUrl=%s"

#define		NFLIX_PERSISTENET_STORAGE_PATH	"/var/lib/humaxtv_user/netflix"
#define		NFLIX_DISK_CACHE_PATH			"/var/lib/humaxtv_user/netflix/var/nrd/gibbon/diskcache"
#define		NFLIX_DISK_STORE_PATH			"/mnt/hd3"
#define		NFLIX_DISK_STORE_TOUCH_FILE		"/mnt/hd3/nx_cache.touch"

#define		NFLIX_USER_PARTITION_TOUCH_FILE		"/var/lib/humaxtv_user/nx_usr_part.touch"



//#define		NFLIX_MANUFSS_APP					"./manufss"
#define		NFLIX_MANUFSS_APP						"./humax_manufss"

#define		NFLIX_MAX_STR_LEN			1024
#define		NFLIX_MAC_ADDR_LEN			20
#define		NFLIX_SYSINFO_STR_LEN			32

#if defined(CONFIG_PROD_FVP4000T)
#define		__USE_CALLBACK_FOR_DIAL_CHECKING__          (1)
#else
#define		__USE_CALLBACK_FOR_DIAL_CHECKING__          (0)
#endif

#define		DIAL_ADDITIONALDATAURL_DEFAULT				"http%3A%2F%2Flocalhost%3A56789%2FdialData%3Fapp%3DNetflix"

#if !__USE_CALLBACK_FOR_DIAL_CHECKING__
#define		NFLIX_DIAL_CHECKING_PERIOD		10000	// 10 seconds
#define		NFLIX_DIAL_UPDATING_PERIOD		600000	// 10 minutes

#if defined(CONFIG_OP_TDC)
#define		NFLIX_DIAL_CHECK_OVERCOUNT		12		// 2 minutes // too long time for dial init(booting)
#else
#define		NFLIX_DIAL_CHECK_OVERCOUNT		6			// 60 seconds
#endif
#endif

#define		__RECOVER_USER_DB__					1

#define		__CLEAR_DUPLICATE_PROCESS__				1

#if			__CLEAR_DUPLICATE_PROCESS__

#define		NFLIX_DUPLICATE_CHECKING_PERIOD			60000	// 10 minutes

static		HUINT32		s_ulDuplicateCheckCount;// = 0;

#endif


#define		__SUPPORT_PROVISIONING__			1
#if __SUPPORT_PROVISIONING__

#define		__FIX_PROVISIOING_BUG__				0
#define		__USB_PROVISIONING__				0
#define		__DO_IN_FIELD_PROVISIONING__		1

#else //__SUPPORT_PROVISIONING__

#define		__FIX_PROVISIOING_BUG__				0
#define		__USB_PROVISIONING__				0
#define		__DO_IN_FIELD_PROVISIONING__		0

#endif //__SUPPORT_PROVISIONING__

#define		__SUPPORT_SPLUNK_REPORT__			0

#define		__PRIVACY_POLICY_AUTH__				1

typedef struct _NFlix_SysInfo_ {
	HBOOL		bValid;
	HCHAR 		szMac[NFLIX_MAC_ADDR_LEN];
	HCHAR		szVersion[NFLIX_SYSINFO_STR_LEN];
 	HCHAR		szSystem_id[NFLIX_SYSINFO_STR_LEN];
	HCHAR		szDevice_id[NFLIX_SYSINFO_STR_LEN];
	HUINT32		ulTimezone;
	HCHAR		*pszAuthTicket;
	HUINT8		*pszProvisioningResult;
}NFlix_SysInfo_t;

NFlix_SysInfo_t		s_stSysInfo;

typedef struct _HTTP_Response_ {
	char 		szBuff[NFLIX_MAX_STR_LEN];
	size_t 		size;
}_HTTP_Response_t;

static _HTTP_Response_t 		s_stHttpResponse;

static NFLIX_NotifyState_t		s_stExitState;


#define		NFLIX_TVPORTAL_ROOT_CA		"/usr/browser/opera_dir/certs/root/humax_tvportal_rootcert_2k_20101106_pub.pem"
#define		NFLIX_TVPORTAL_CERT		"/usr/browser/opera_dir/certs/client/htvdev_portal_ccert_2k_20101106.pem"

#if __SUPPORT_PROVISIONING__

#define		NFLIX_CLEAR_CREDENTIALS 	"/var/lib/humaxtv_user/netflix/ESN.idfile"
#define		MXT_STR_PROVISIONING_RESULT	256

#if __DO_IN_FIELD_PROVISIONING__

#define		NFLIX_REQ_CREDENTIAL_URL	"https://www.humaxtvportal.com/api/GetNetflixKey?deviceID=%s&macaddress=%s&systemid=%s&swver=%s&serialnumber=%s&timezone=%d"
//#define		NFLIX_REQ_CREDENTIAL_URL	"https://10.0.12.122/Humax.Tvportal/api/GetNetflixKey?deviceID=%s&macaddress=%s&systemid=%s&swver=%s&serialnumber=%s&timezone=%d"

#endif //__DO_IN_FIELD_PROVISIONING__


#if __FIX_PROVISIOING_BUG__

#define		NFLIX_FIX_BUG_LOG 	"/var/lib/humaxtv_user/netflix/fbESN.0"

#endif //__FIX_PROVISIOING_BUG__

#if __USB_PROVISIONING__

#define		NFLIX_USB_CREDENTIALS 	"/media/drive1/ESN.idfile"

#endif //__USB_PROVISIONING__

#endif //__SUPPORT_PROVISIONING__

#define		NFLIX_ACTIVATED_INFO_PATH					"/usr/bin/netflix/data/var/nrd/activated"

#define		_NFLIX_SUSPEND_WITH_LAST_SUSPEND_GAP	(10000) /* ... */
#define		_NFLIX_ACTION_WITH_LAST_SUSPEND_GAP		(5000) /* ... */
#define		_NFLIX_RESUME_WITH_LAST_SUSPEND_GAP		(3000) /* ... */
#define		_NFLIX_OVERLAY_WITH_LAST_ACTION_GAP		(2000) /* Increased gap with 1.5 (sec) from 1 (sec) */
#define		_NFLIX_OVERLAY_WITH_LAST_POPUP_GAP		(1000) /* Increased gap with 1.5 (sec) from 1 (sec) */
#define		_NFLIX_TASK_PRIORITY					(40)
#define		_NFLIX_TASK_SIZE						(0x4000)

#define		_NFLIX_ACK_TIMEOUT_						(20)	// 20 sec

#define		_STB_SHUTDOWN_WAIT_TIME_				(5000)	// 5 seconds

#define		_NFLIX_NORMAL_CLEAN_UP_TIMEOUT			(15) // 15 sec // Incl. DIAL_EXIT
#define		_NFLIX_STANDBY_CLEAN_UP_TIMEOUT			(90) // 90 sec

#define		__SUPPORT_NETFLIX_BACKGROUND__			0

#if defined(CONFIG_OP_TDC)
#define		__SUPPORT_SLEEP_WAKE__					1
#else
#define		__SUPPORT_SLEEP_WAKE__					0
#endif

#define		__NFLIX_WAITING_MODE_CHANGING_COMPLETE__		1

#if __NFLIX_WAITING_MODE_CHANGING_COMPLETE__
#define		NFLIX_MODE_CHANGING_WAIT_TIME			(4000) // 4 sec
#endif

//#if __SUPPORT_SPLUNK_REPORT__

#define		__TESTING_SPLUNK_REPORT__			0
#define		__SUPPORT_CHK_UPLOADSTATISTIC__		1

#if __TESTING_SPLUNK_REPORT__

#define		REPORTING_CHECK_PERIOD			(3000)  // 3 seconds

#define		PORTAL_LOGIN_URL				"http://10.0.12.122/Humax.Tvportal/api/Login?deviceID=%s&macaddress=%s&systemid=%s&swver=%s&serialnumber=%s&timezone=%d"
#define		PORTAL_CHEK_UPLOAD_STATUS_URL	"http://10.0.12.122/Humax.Tvportal/api/CheckUploadStatistics?authticket=%s"
#define		PORTAL_UPLOAD_STATUS_URL		"http://10.0.12.122/Humax.Tvportal/api/UploadStatistics?authticket=%s"

#else

#define		REPORTING_CHECK_PERIOD			(300000)  // 5 minutes

#define		PORTAL_PP_URL				"https://www.humaxtvportal.com/api/GetLegalDocVer?systemid="
//#define		PORTAL_PP_URL				"http://test.humaxtvportal.com/api/GetLegalDocVer?systemid="

//#define		PORTAL_LOGIN_URL				"http://test.humaxtvportal.com/api/Login?deviceID=%s&macaddress=%s&systemid=%s&swver=%s&serialnumber=%s&timezone=%d"
#define		PORTAL_LOGIN_URL				"https://www.humaxtvportal.com/api/Login?deviceID=%s&macaddress=%s&systemid=%s&swver=%s&serialnumber=%s&timezone=%d"
#define		PORTAL_CHEK_UPLOAD_STATUS_URL	"https://www.humaxtvportal.com/api/CheckUploadStatistics?authticket=%s"
#define		PORTAL_UPLOAD_STATUS_URL		"https://www.humaxtvportal.com/api/UploadStatistics?authticket=%s"

#endif

typedef enum
{
	STATUS_LOG_STOP,
	STATUS_LOG_START,
	STATUS_LOG_PROVISIONING_START,
	STATUS_LOG_PROVISIONING_END,
}Status_Type_t;

typedef struct _tag_status_log_t
{
	Status_Type_t		stLogType;
	HUINT8				szTimeStamp[NFLIX_SYSINFO_STR_LEN];
} Status_Log_t;

static HxList_t	*s_logList;// = NULL;

//#endif

static HInputRetType_e   s_eKeyInputRetType;

/**************************************************************
	internal structures and variables
 **************************************************************/


#if __SUPPORT_NETFLIX_BACKGROUND__
static HBOOL	s_bGoToBackground = FALSE;
#endif

typedef	enum
{
	ePowerState_Undefined				= 0,		//	Initial value, Standby key will be skipped.
	ePowerState_InProgressOperationMode,
	ePowerState_OperationMode,
	ePowerState_InProgressStandbyMode, /* ... */
	ePowerState_StandbyMode,  /* ... */
	//sPowerState_AsItWas,
	sPowerState_Max
} PowerState_t;


/**************************************************************
	Application(Netflix) Manager Controller Definitions
 **************************************************************/
 #define	____APPLICATION_MANAGER_CONTROLLER____DEF


/**************************************************************
	Application(Netflix) Manager Definitions
 **************************************************************/
 #define	____APPLICATION_MANAGER____DEF

typedef enum _ClientCommand { /* TODO : Sync-up with enum ClientCommand in partner\platform\humax\ApplicationManagerClient.cpp  */
	//-----------------------------------
	//  Commands from AM to AM client (app)
	//-----------------------------------
	CMD_SUSPEND = 1,
	CMD_RESUME,
	CMD_BACKGROUND,
	CMD_STATUS,

	CMD_SUSPEND_FROM_BACKGROUND,
	CMD_RESUME_LINK,
	CMD_KEY_L,
	CMD_KEY_R,
	CMD_KEY_U,
	CMD_KEY_D,
	CMD_KEY_E,
	CMD_KEY_PAUSE,
	CMD_KEY_PLAY,
	CMD_KEY_OVERLAY_PAUSE, /* overlay popup */
	CMD_KEY_OVERLAY_PLAY, /* overlay close */

	CMD_SUSPEND_APPLICATION_REQUEST,
	CMD_BACKGROUND_APPLICATION_REQUEST,

	// Stop cases
	CMD_STOP_LOW_RESOURCES,
	CMD_STOP_USER_KILL,
	CMD_STOP_DIAL,						//15

	CMD_PREPARE_FORCE_KILL,						// 16

	// Power events
	CMD_POWER_SLEEP, /* ... */
	CMD_POWER_WAKE /* ... */
}Nflix_ClientCmd_t;

typedef enum _ControllerCommand {
	//-----------------------------------
	//  Commands from AM controller to AM
	//-----------------------------------

	CMD_CNT_START_SUSPEND,		// Instantiate app in suspended state
	CMD_CNT_START_FOREGROUND,   // Instantiate app in running state

	CMD_CNT_SUSPEND,			// Suspend app

#if __SUPPORT_NETFLIX_BACKGROUND__
	CMD_CNT_BACKGROUND,			// Background app
#endif

	CMD_CNT_RESUME,				// Resume app
	CMD_CNT_RESUME_LINK,		// Resume app with content to play

	CMD_CNT_STOP_LOW_RESOURCES,	// Terminate app nicely because of anticipated resource contention.
	CMD_CNT_STOP_USER_KILL,		// Terminate app nicely because the user killed Netflix.
	CMD_CNT_STOP_DIAL,			// Terminate app nicely DIAL wants to kill Netflix.
	CMD_CNT_EXIT,				// Terminate AM & app
	CMD_CNT_SIGKILL,			// Send SIGKILL to app process

	CMD_CNT_POWER_SLEEP,		// Device will go into a low power state
	CMD_CNT_POWER_WAKE,			// Device resumed from a low power state

	CMD_CNT_KEY_L,
	CMD_CNT_KEY_R,
	CMD_CNT_KEY_U,
	CMD_CNT_KEY_D,
	CMD_CNT_KEY_E,
	CMD_CNT_KEY_PAUSE,
	CMD_CNT_KEY_PLAY,
	CMD_CNT_KEY_OVERLAY_PAUSE, /* overlay popup */
	CMD_CNT_KEY_OVERLAY_PLAY, /* overlay close */

	CMD_CNT_SUSPEND_APPLICATION_REQUEST,			// Application request :  suspend

#if __SUPPORT_NETFLIX_BACKGROUND__
	CMD_CNT_BACKGROUND_APPLICATION_REQUEST, 		//Application request :  background
#endif

	CMD_CNT_NONE
}Nflix_ControllerCmd_t;

enum AMCommand
{
		CMD_AM_STOP = 0x10,

		//-----------------------------------
		//  Commands from AM client (app) to AM
		//-----------------------------------
		CMD_AM_REQUEST_SUSPEND,
		CMD_AM_REQUEST_BACKGROUND,

		// State changes
		CMD_AM_STARTED,
		CMD_AM_SUSPENDED
};

enum Error {
	Error_No_Error = 0,
	Error_Timeout,
	Error_General,
	Error_ACK_Timeout,
	Error_State
};


typedef enum _AppState {
	NFLIX_NO_INSTANCE 	= 0x1, 		//(1 << 0),
	NFLIX_SUSPENDED		= 0x2,		//(1 << 1),
	NFLIX_FOREGROUNDED	= 0x4,		//(1 << 2),
	NFLIX_BACKGROUNDED	= 0x8,		//(1 << 3),
	NFLIX_NO_CHANGE		= 0x100,
}NFLIX_State_t;


typedef enum _SuspendStatus_t {	
	NFLIX_SunpendStatus_Ongoing,
	NFLIX_SunpendStatus_Timeout,
	NFLIX_SunpendStatus_Completed,
	NFLIX_SunpendStatus_NA,
}NFLIX_SuspendStatus_t;


typedef enum _OverlayPlaybackState_t {
	NFLIX_OVERLAY_PBACTION_NA = 0,
	NFLIX_OVERLAY_PBACTION_PAUSED,
	NFLIX_OVERLAY_PBACTION_RESUMED,
} NFLIX_OverlayPlaybackState_t;

typedef struct _CmdEntry {
	Nflix_ControllerCmd_t   cmd;
	const char *			cmd_str;
	const char *			desc;
}NFLIX_CmdEntry_t;

typedef struct _ActionEntry {
	Nflix_ControllerCmd_t   	cmd;
	int							validStates;
	NFLIX_State_t				newState;
	int 						(*action)(void);
}NFLIX_ActionEntry_t;


typedef struct _DialInfo {
	HBOOL		bHommaActivated;
	HBOOL		bDialActivated;
	HUINT8		szAdditionalUrl[MAX_ENCORDED_URL_LEN];
}NFLIX_DialInfo_t;


/**************************************************************
	Application(Netflix) Manager Common prototype
 **************************************************************/
#define	____APPLICATION_MANAGER_COMMON____PROTOTYPE

static void	RequestExit(NFLIX_Exit_Reason_Type_t exitType);
static const char * ApplicationSocketPath(void);

static HBOOL login_TVPortal(void);
static HERROR __nxman_CheckPrivacyPolicyVer(HBOOL *bUpdated);

/**************************************************************
	Application(Netflix) Manager prototype
 **************************************************************/
 #define	____APPLICATION_MANAGER____PROTOTYPE

static int AM_doStartSuspend(void);
static int AM_doStartRunning(void);
//static int AM_doBackground(void);
static int AM_doResume(void);
static int AM_doResumeLink(void);
static int AM_doSleep(void);
static int AM_doWake(void);
static int AM_doKeyLeft(void);
static int AM_doKeyRight(void);
static int AM_doKeyUp(void);
static int AM_doKeyDown(void);
static int AM_doKeyEnter(void);
static int AM_doKeyPause(void);
static int AM_doKeyPlay(void);
static int AM_doKeyOverlayPause(void); /* Overlay popup/close */
static int AM_doKeyOverlayPlay(void); /* Overlay popup/close */

#if __SUPPORT_NETFLIX_BACKGROUND__
static int AM_doBackground(void);
static int AM_doBackground_from_App(void);
#endif

static int AM_doSuspend(void);
static int AM_doSuspend_from_App(void);
static int AM_doStopLowResources(void);
static int AM_doStopUserKill(void);
static int AM_doStopDial(void);
static int AM_doExit(void);
static int AM_doSigKill(void);
static int AM_handleAction(Nflix_ControllerCmd_t cmd);
//static Nflix_ControllerCmd_t AM_StringToControllerCommand(const char * cmd);
static const char * AM_ControllerCommandToString(Nflix_ControllerCmd_t cmd);
//static void AM_ListCommands(void);
static void AM_SignalChild(int iArg);
//static Nflix_ControllerCmd_t AM_GetCommandFromController(void);
//static void AM_SendReturnToController(char code);
static char AM_SendCommandToApplication(Nflix_ClientCmd_t cmd);
static int AM_killApplication(HBOOL bExitNotify);
static int AM_startApplication(int argc, char *argv[], bool startSuspended);
static int AM_stopApplication(void);
static void* AM_proc(void * arg);
static char *__nxman_getPlaybackStateToStr(NFLIX_NotifyPlaybackState_t state);

/**************************************************************
	Application(Netflix) Manager Global variable
 **************************************************************/
 #define	____APPLICATION_MANAGER____VAR

static HxQueue_t*	s_pAMQueue = NULL;

static const int	s_nRestartTimeoutInSec = 10;
static const int 	s_nAppRequestTimeoutInUsec = 100000;

pid_t 				s_nApplicationPid;// = 0;
HBOOL 				s_bShouldRestart;// = FALSE;
HBOOL 				s_bCrashed;// = FALSE;
int  				s_nRestartCounter;// = 0;

HBOOL				s_bRestartSuspended = TRUE;

static HBOOL		s_bNetActivated;// = FALSE;
static HBOOL		s_bTimeSetted;// = FALSE;

#if __USE_CALLBACK_FOR_DIAL_CHECKING__

static HUINT32				s_ulDialSem;
static NFLIX_DialInfo_t		s_stDialInfo;

#else

static HBOOL		s_bDialActivated;// = FALSE;
static HUINT8		s_szAdditionalUrl[MAX_ENCORDED_URL_LEN];
static HUINT32		s_ulDialCheckStartTime;// = 0;
static HUINT32		s_ulDialCheckCount;// = 0;

#endif

static HBOOL 		s_bSuspendStarted;// = FALSE;

static HBOOL		s_bNFlixActivated;

/* control suspend/resume */
//static HULONG 		s_ulCheckTimerId = 0x0; /* delayed checking for LivePB by timer when fast resume after suspend */
static NFLIX_SuspendStatus_t s_eNetflixSuspendStatus = NFLIX_SunpendStatus_NA;
//static OxMediaPlay_PlayState_e	  s_eSuspendPbState = eOxMP_PLAYSTATE_NONE;

//static HUINT32 		s_ulLastNetflixSuspendStartTime = 0x0;
//static HUINT32 		s_ulLastNetflixSuspendEndTime = 0x0;
//static HUINT32 		s_ulLastNetflixResumeStartTime = 0x0;
//static HUINT32 		s_ulLastNetflixResumeEndTime = 0x0;

static HBOOL		s_bFirstBoot;//	= FALSE;
static HBOOL		s_bFactoryReset;// = FALSE;

static HBOOL		s_bDiskStoreReady;// = FALSE;

#if __SUPPORT_PROVISIONING__
static HBOOL		s_bProvisioningDone;// = FALSE;
static HBOOL		s_bTryProvisioning;// = FALSE;
static HUINT32      s_ulEsnLength = 0;
#endif

static PowerState_t s_eCurPowerState;// = ePowerState_Undefined;
static HBOOL		s_bOnChaingingState;// = FALSE;

//static HBOOL		s_bChaingingStateStartTime;// = 0;  // for resume/suspend changing

static HUINT32 s_ulNxManSem;// = 0x0;


static HUINT32 s_ulOverlaySem;// = 0x0;
static HUINT32 s_ulLastOverlayPopupStartTime;// = 0x0; /* GetSystemTick */
static HUINT32 s_ulLastOverlayPopupEndTime;// = 0x0;
static HUINT32 s_ulLastOverlayCloseStartTime;// = 0x0;
static HUINT32 s_ulLastOverlayCloseEndTime;// = 0x0;
static HUINT32 s_ulLastOverlayEventTime;// = 0x0;
static HUINT32 s_ulLOverlayEventCnt;// = 0x0;
static NFLIX_Overlay_Ctrl_t		s_eOverlayState = NFLIX_OVERLAY_NA;

static NFLIX_OverlayPlaybackState_t s_eOverlayPlaybackAction;// = NFLIX_OVERLAY_PBACTION_NA;

/*
 * Playback state thru hapi
 *	The playback state thru appkit or oapi_media_play may have intermittently inaccurate error.
 *	For example, very late oapi cache updates, ...
 */
static NFLIX_NotifyPlaybackState_t s_eNetflixPlaybackState;// = NFLIX_NOTIFY_PLAYBACK_NA; /* TODO: Make with handle and sema */

static HBOOL	s_bResetVideoResolution;// = FALSE;

static HUINT32		s_ulTryConnt;// = 0;
#define				MAX_TIME_CHECK_COUNT	7

#if __SUPPORT_SLEEP_WAKE__
static HBOOL		s_bNetflixSleep;// = FALSE;
#endif


static int 		s_nArgc;
static char	*s_pArgv[16];// = {NULL, };

struct sockaddr_un s_stApplicationAddr;

static int		s_nApplicationFd = -1;
static int		s_nApplicationSessionFd = -1;

NFLIX_State_t 		s_eNflixState = NFLIX_NO_INSTANCE;

static NFLIX_CmdEntry_t 	s_astCmdTable[] = 	{
												{ CMD_CNT_START_SUSPEND,					"start_suspend",	"Start the application in 'suspend' state."},
												{ CMD_CNT_START_FOREGROUND,					"start_foreground",	"Start the application in 'foreground' state." },
												{ CMD_CNT_SUSPEND,							"suspend",			"Tell the application to move into 'suspend' state."},
#if __SUPPORT_NETFLIX_BACKGROUND__
												{ CMD_CNT_BACKGROUND,						"background",		"Tell the application into 'background' state."},
#endif
												{ CMD_CNT_RESUME,							"resume",			"Tell the application to resume from 'background' or 'suspend' state."},
												{ CMD_CNT_RESUME_LINK,						"resume_link",		"Tell the application to resume from 'background' or 'suspend' state with an asset to play."},
												{ CMD_CNT_STOP_LOW_RESOURCES,				"stop_resources",	"Tell the application to self terminate because of an existing or anticipated resource constraint."},
												{ CMD_CNT_STOP_USER_KILL,					"stop_user",		"Tell the application to self terminate because the user explicity requests it."},
												{ CMD_CNT_STOP_DIAL,		 				"stop_dial",		"Tell the application to self terminate because of a DIAL stop."},
												{ CMD_CNT_EXIT,								"exit",				"Exit the application and the application manager."},
												{ CMD_CNT_SIGKILL,							"sigkill",			"Send the SIGKILL signal to the application to simulate a crash."},
												{ CMD_CNT_POWER_SLEEP,						"sleep",			"Send the 'sleep' event to the application for logging."},
												{ CMD_CNT_POWER_WAKE,						"wake",				"Send the 'wake' event to the application for logging."},
												{ CMD_CNT_KEY_L,			 				"left",				"Send the 'left' key pair (press/release) to the application."},
												{ CMD_CNT_KEY_R,			 				"right",			"Send the 'right' key pair (press/release) to the application."},
												{ CMD_CNT_KEY_U,			 				"up",				"Send the 'up' key pair (press/release) to the application."},
												{ CMD_CNT_KEY_D,			 				"down",				"Send the 'down' key pair (press/release) to the application."},
												{ CMD_CNT_KEY_E,			 				"enter",			"Send the 'enter' key pair (press/release) to the application."},
												{ CMD_CNT_KEY_PAUSE,			 			"pause",			"Send the 'pause' key pair (press/release) to the application."},
												{ CMD_CNT_KEY_PLAY,							"play",				"Send the 'play' key pair (press/release) to the application."},
												{ CMD_CNT_KEY_OVERLAY_PAUSE,				"overlay/pause",	"Send the 'overlay-pause' key pair (press/release) to the application."},
												{ CMD_CNT_KEY_OVERLAY_PLAY,					"overlay/play",		"Send the 'overlay-play' key pair (press/release) to the application."},
												{ CMD_CNT_SUSPEND_APPLICATION_REQUEST,		"suspend_from_app",	"Tell the application to move into 'suspend' state.(request from application)"},
#if __SUPPORT_NETFLIX_BACKGROUND__
												{ CMD_CNT_BACKGROUND_APPLICATION_REQUEST,	"background_from_app", "Tell the application into 'background' state.(request from application)"},
#endif
												{ CMD_CNT_NONE,								"No action",		"No action"}
											};

#if __SUPPORT_NETFLIX_BACKGROUND__

static NFLIX_ActionEntry_t s_astActionTable[] ={
												{ CMD_CNT_START_SUSPEND,					NFLIX_NO_INSTANCE,											NFLIX_SUSPENDED,	&AM_doStartSuspend		},
												{ CMD_CNT_START_FOREGROUND,					NFLIX_NO_INSTANCE,											NFLIX_FOREGROUNDED,	&AM_doStartRunning		},
												{ CMD_CNT_SUSPEND,							NFLIX_FOREGROUNDED | NFLIX_BACKGROUNDED,					NFLIX_SUSPENDED,	&AM_doSuspend			},
												{ CMD_CNT_BACKGROUND,						NFLIX_FOREGROUNDED,											NFLIX_BACKGROUNDED,	&AM_doBackground		},
												{ CMD_CNT_RESUME,							NFLIX_SUSPENDED | NFLIX_BACKGROUNDED,						NFLIX_FOREGROUNDED,	&AM_doResume			},
												{ CMD_CNT_RESUME_LINK,						NFLIX_SUSPENDED | NFLIX_BACKGROUNDED,						NFLIX_FOREGROUNDED,	&AM_doResumeLink		},
												{ CMD_CNT_STOP_LOW_RESOURCES,				NFLIX_SUSPENDED | NFLIX_BACKGROUNDED | NFLIX_FOREGROUNDED,	NFLIX_NO_INSTANCE,	&AM_doStopLowResources	},
												{ CMD_CNT_STOP_USER_KILL,					NFLIX_SUSPENDED | NFLIX_BACKGROUNDED | NFLIX_FOREGROUNDED,	NFLIX_NO_INSTANCE,	&AM_doExit				},
												{ CMD_CNT_STOP_DIAL,						NFLIX_SUSPENDED | NFLIX_BACKGROUNDED | NFLIX_FOREGROUNDED,	NFLIX_NO_INSTANCE,	&AM_doStopDial			},
												{ CMD_CNT_EXIT,								NFLIX_SUSPENDED | NFLIX_BACKGROUNDED | NFLIX_FOREGROUNDED,	NFLIX_NO_CHANGE,	&AM_doExit				},	// Special, kills AM too
												{ CMD_CNT_SIGKILL,							NFLIX_SUSPENDED | NFLIX_BACKGROUNDED | NFLIX_FOREGROUNDED,	NFLIX_NO_INSTANCE,	&AM_doSigKill			},	// Special, simulate crash
												{ CMD_CNT_POWER_SLEEP,						NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doSleep				},
												{ CMD_CNT_POWER_WAKE,						NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doWake				},
												{ CMD_CNT_KEY_L,							NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doKeyLeft			},
												{ CMD_CNT_KEY_R,							NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doKeyRight			},
												{ CMD_CNT_KEY_U,			   				NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doKeyUp				},
												{ CMD_CNT_KEY_D,							NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doKeyDown			},
												{ CMD_CNT_KEY_E,							NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doKeyEnter			},
												{ CMD_CNT_KEY_PAUSE,						NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doKeyPause			},
												{ CMD_CNT_KEY_PLAY,							NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doKeyPlay			},
												{ CMD_CNT_KEY_OVERLAY_PAUSE,				NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doKeyOverlayPause	},
												{ CMD_CNT_KEY_OVERLAY_PLAY,					NFLIX_FOREGROUNDED,											NFLIX_NO_CHANGE,	&AM_doKeyOverlayPlay	},
												{ CMD_CNT_SUSPEND_APPLICATION_REQUEST,		NFLIX_FOREGROUNDED | NFLIX_BACKGROUNDED,					NFLIX_SUSPENDED,	&AM_doSuspend_from_App	},
												{ CMD_CNT_BACKGROUND_APPLICATION_REQUEST,	NFLIX_FOREGROUNDED,											NFLIX_BACKGROUNDED,	&AM_doBackground_from_App}
											};


#else

static NFLIX_ActionEntry_t s_astActionTable[] ={
												{ CMD_CNT_START_SUSPEND,				NFLIX_NO_INSTANCE,										NFLIX_SUSPENDED,	&AM_doStartSuspend		},
												{ CMD_CNT_START_FOREGROUND,				NFLIX_NO_INSTANCE,										NFLIX_FOREGROUNDED,	&AM_doStartRunning		},
												{ CMD_CNT_SUSPEND,						NFLIX_FOREGROUNDED,										NFLIX_SUSPENDED,	&AM_doSuspend			},
												{ CMD_CNT_RESUME,						NFLIX_SUSPENDED,										NFLIX_FOREGROUNDED,	&AM_doResume			},
												{ CMD_CNT_RESUME_LINK,					NFLIX_SUSPENDED,										NFLIX_FOREGROUNDED,	&AM_doResumeLink		},
												{ CMD_CNT_STOP_LOW_RESOURCES,			NFLIX_SUSPENDED,										NFLIX_NO_INSTANCE,	&AM_doStopLowResources	},
												{ CMD_CNT_STOP_USER_KILL,				NFLIX_SUSPENDED | NFLIX_FOREGROUNDED,					NFLIX_NO_INSTANCE,	&AM_doExit				},
												{ CMD_CNT_STOP_DIAL,					NFLIX_SUSPENDED | NFLIX_FOREGROUNDED,					NFLIX_NO_INSTANCE,	&AM_doStopDial			},
												{ CMD_CNT_EXIT,							NFLIX_SUSPENDED | NFLIX_FOREGROUNDED,	 				NFLIX_NO_INSTANCE,	&AM_doExit				},	// Special, kills AM too
												{ CMD_CNT_SIGKILL,						NFLIX_SUSPENDED | NFLIX_FOREGROUNDED,					NFLIX_NO_INSTANCE,	&AM_doSigKill			},	// Special, simulate crash
												{ CMD_CNT_POWER_SLEEP,					NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doSleep				},
												{ CMD_CNT_POWER_WAKE,					NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doWake				},
												{ CMD_CNT_KEY_L,						NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doKeyLeft			},
												{ CMD_CNT_KEY_R,						NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doKeyRight			},
												{ CMD_CNT_KEY_U,						NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doKeyUp				},
												{ CMD_CNT_KEY_D,						NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doKeyDown			},
												{ CMD_CNT_KEY_E,						NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doKeyEnter			},
												{ CMD_CNT_KEY_PAUSE,					NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doKeyPause			},
												{ CMD_CNT_KEY_PLAY,						NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doKeyPlay			},
												{ CMD_CNT_KEY_OVERLAY_PAUSE,			NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doKeyOverlayPause	},
												{ CMD_CNT_KEY_OVERLAY_PLAY,				NFLIX_FOREGROUNDED,										NFLIX_NO_CHANGE,	&AM_doKeyOverlayPlay	},
												{ CMD_CNT_SUSPEND_APPLICATION_REQUEST,	NFLIX_FOREGROUNDED,										NFLIX_SUSPENDED,	&AM_doSuspend_from_App	}
											};

#endif

#if defined(CONFIG_NETFLIX_BARKER_CHANNEL)
static HUINT8		*s_pszCategory[Category_MAX] =
								{
									"BASIC_SD",
									"BASIC_HD",
									"MOVIE_SD",
									"MOVIE_HD",
									"CHILDREN_SD",
									"CHILDREN_HD",
									"LEARNING_SD",
									"LEARNING_HD",
									"INDIE_SD",
									"INDIE_HD",
									"OTHER"
								};
#endif

static 	Launch_Param_t		s_stLaunchParam;// = { 0, };
static  bool				s_bAMRunning;// = false;

#if 0//O_CHECK_DIAL_ACTIVATION__
static  HBOOL				s_bDialActivated = FALSE;
static 	HUINT8				s_szAdditionalUrl[MAX_ENCORDED_URL_LEN] = { 0, };
//static HUINT32 s_ulLastDialUpdateRequestTime = 0x0;
#endif

static 	HUINT8				s_szUiQuery[MAX_NFLIX_UI_QUERY_STR];
static 	HUINT8				s_szNxPayload[MAX_MFLIX_PAYLOAD_STR];
static 	HUINT8				s_szNxCmd[MAX_MFLIX_PAYLOAD_STR];


/**************************************************************
	private functions
 **************************************************************/
#ifdef CONFIG_MEM_PRINT
/**
   * Returns the current resident set size (physical memory use) measured
	* in bytes, or zero if the value cannot be determined on this OS.
	 */
void print_rss(char *function , int line, const char* string)
{
	/* Linux ---------------------------------------------------- */
	long rss = 0L;
	FILE* fp = NULL;
	if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
		return;
	if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
	{
		fclose( fp );
		return;
	}
	fclose( fp );

	HxLOG_Print ("DRAGON NXMANAGER RSS :  %d  : %s(%d) : %s\n",(size_t)rss * (size_t)sysconf( _SC_PAGESIZE),function,line,string);
	return ;

}
#endif


#if defined(CONFIG_ENHANCED_SECURITY)
static 	void dropUserPrevilegeAndChroot(void)
{
	int i;
	struct passwd *u_info;
	struct group  *g_info;
	struct rlimit rlim;

#if (0)
	/* 1. Close open file descriptors */
	if (getrlimit(RLIMIT_NOFILE, &rlim))
	{
		HxLog_Critical("getrlimit() for RLIMIT_NOFILE failed with error %s\n",
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
		HxLog_Critical("\n\t\t =============> APP_ChrootAndDropUserPrevilege : error chroot.....\n");
		exit(-1);
	}
	else
	{
		HxLog_Critical("\n\t\t =============> APP_ChrootAndDropUserPrevilege : succ chroot.....\n");
	}

	if (chdir("/"))
	{
		HxLog_Critical("chdir() failed...\n");
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
			HxLog_Critical("Failed changing GID to %d with error %s\n",
						gid, strerror(errno));
			exit(-1);
		}

		if (setresuid(uid, uid, uid) != 0)
		{
			HxLog_Critical("Failed changing UID to %d with error %s\n",
						uid, strerror(errno));
			exit(-1);
		}

		if (getresuid(&real, &eff, &saved) != 0)
		{
			HxLog_Critical("Failed reading UID with error %s\n", strerror(errno));
			exit(-1);
		}

		if (real != uid || eff != uid || saved != uid)
		{
			HxLog_Critical("UID sanity check failed\n");
			exit(-1);
		}

		if (getresgid(&real, &eff, &saved) != 0)
		{
			HxLog_Critical("Failed reading GID with error %s\n", strerror(errno));
			exit(-1);
		}

		if (real != gid || eff != gid || saved != gid)
		{
			HxLog_Critical("GID sanity check failed\n");
			exit(-1);
		}

		HxLog_Critical("Dropped root priviliges. Now running as UID %u GID %u\n", uid, gid);
	}
	else
	{
		exit(-1);
	}
}
#endif

static void Init_SysInfo(void)
{
	HINT32				i, nDevNum = 0;
	APKS_NETDEVINFO_t 	stDevInfo;

	HxLOG_Print(" [NXMAN] Init_SysInfo++ (%d)\n", s_stSysInfo.bValid);

	if(s_stSysInfo.bValid == FALSE)
	{
		if(APK_NETWORK_CONF_GetNumOfNetDev(&nDevNum) != ERR_OK)
		{
			HxLOG_Error(" [NXMAN]   Error: Getting Network DeviceNumber is failed.\n");
			return;
		}

		for(i = 0; i < nDevNum; i++)
		{
			if((APK_NETWORK_CONF_GetDevInfo(i, &stDevInfo) == ERR_OK) && (stDevInfo.eType == eINTERFACE_TYPE_LAN))
			{
				break;
			}
		}

		if(i == nDevNum)
		{
			HxLOG_Error(" [NXMAN]   Error: Not found LAN device..!!\n");
			return;
		}

		if ( APK_NETWORK_CONF_GetMacAddr(i, s_stSysInfo.szMac, NFLIX_MAC_ADDR_LEN) != ERR_OK )
		{
			HxLOG_Error(" [NXMAN]   Error: Getting the Mac Address is failed.\n");
			return;
		}

		if(APK_SYSINFO_STB_GetSystemId(s_stSysInfo.szSystem_id, NFLIX_SYSINFO_STR_LEN) != ERR_OK)
		{
			HxLOG_Error(" [NXMAN]   Error: Getting System ID is failed.\n");
			return;
		}

		if(APK_SYSINFO_STB_GetSerialNumber(s_stSysInfo.szDevice_id, NFLIX_SYSINFO_STR_LEN) != ERR_OK)
		{
			HxLOG_Error(" [NXMAN]   Error: Getting S/N(Device ID) is failed.\n");
			return;
		}

		if(APK_SYSINFO_STB_GetSoftwareVersion(s_stSysInfo.szVersion, NFLIX_SYSINFO_STR_LEN) != ERR_OK)
		{
			HxLOG_Error(" [NXMAN]   Error: Getting SW Version is failed.\n");
			return;
		}
		else
		{
			HCHAR *pTemp = HxSTD_StrChar(s_stSysInfo.szVersion, ' ');

			if(pTemp != NULL)
			{
				*pTemp = '_';
			}
		}
#if __PRIVACY_POLICY_AUTH__
#else
		{
			HERROR hError = ERR_FAIL;
			HBOOL bUpdated = FALSE;

			/* Check P/P version in here */
			hError=__nxman_CheckPrivacyPolicyVer(&bUpdated);
			if(hError != ERR_OK || bUpdated == TRUE)
			{
				HxLOG_Error(" [NXMAN]	 @@@@  Error > P/P is updated. Need to agree P/P. \n");
				return;
			}
		}

		if(login_TVPortal() == FALSE)
		{
			HxLOG_Error(" [NXMAN]	 @@@@  Error > Portal Login is fail...!!! \n\n\n");
			return;
		}
#endif
		s_stSysInfo.ulTimezone = 9;
		s_stSysInfo.bValid = TRUE;
	}

	HxLOG_Print(" [NXMAN]  Init_SysInfo-- (%d)\n", s_stSysInfo.bValid);
}

static size_t ResponseInfoCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t 				realsize = size * nmemb;
	_HTTP_Response_t		*pResponse = (_HTTP_Response_t *)userp;

	if((pResponse->size + realsize) >= NFLIX_MAX_STR_LEN)
	{
		HxLOG_Print(" [NXMAN] Warning>> ResponseInfoCallback => Too long respones(%d)...!!!!!!!\n", (pResponse->size + realsize));
	}
	else
	{
		HxSTD_MemCopy(&(pResponse->szBuff[pResponse->size]), contents, realsize);
		pResponse->size += realsize;
		pResponse->szBuff[pResponse->size] = 0;
	}

	return realsize;
}

static HUINT32 Request_HTTPS(HUINT8 *pszUrl, HCHAR* pszPostData)
{
	HUINT32		rc  = 0;
	CURL 		*ctx = NULL;
	CURLcode 	curlRet	= CURLE_OK;

	ctx = curl_easy_init();
	if ( !ctx )
	{
		HxLOG_Error(" [NXMAN]   Error: CURL Open error.\n");
		return rc = -1;
	}

#if defined(CONFIG_DEBUG)
	/* turn off the debugging. if you want to display msg of curl, chane 0L -> 1L */
	(void)curl_easy_setopt(ctx, CURLOPT_VERBOSE, 1L);
#endif

	/* signal problem, So set NOSIGNAL */
	(void)curl_easy_setopt(ctx, CURLOPT_NOSIGNAL, 1L);

	/* A parameter set to 1 tells the library to follow any Location: header that the server sends as part of a HTTP header  */
	(void)curl_easy_setopt(ctx, CURLOPT_FOLLOWLOCATION , 1L);

	/* the progress bar option will be disabled */
	(void)curl_easy_setopt(ctx, CURLOPT_NOPROGRESS , 1L);


	/* Set Certificate*/
	#if 1
	curlRet = curl_easy_setopt(ctx, CURLOPT_CAPATH, NFLIX_TVPORTAL_ROOT_CA);
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Error(" [NXMAN]   Error: Invalid CA certificate PATH(%s)...\n", NFLIX_TVPORTAL_ROOT_CA);
		rc = -1;
		goto EXIT_FUNC;
	}

	curlRet = curl_easy_setopt(ctx, CURLOPT_CAINFO, NFLIX_TVPORTAL_ROOT_CA);
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Error(" [NXMAN]   Error: Invalid CA certificate file(%s)...\n", NFLIX_TVPORTAL_ROOT_CA);
		rc = -1;
		goto EXIT_FUNC;
	}

	/* cert is stored PEM or DER coded in file... */
	/* set the cert for client authentication */
	curlRet = curl_easy_setopt(ctx, CURLOPT_SSLCERT, NFLIX_TVPORTAL_CERT);
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Error(" [NXMAN]   Error: Invalid certificate file...\n");
		rc = -1;
		goto EXIT_FUNC;
	}

	curlRet = curl_easy_setopt(ctx, CURLOPT_SSLCERTTYPE, "PEM");
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Error(" [NXMAN]   Error: Invalid certificate type...\n");
		rc = -1;
		goto EXIT_FUNC;
	}

	/* ask libcurl to use TLS version 1.0 or later */
	curlRet = curl_easy_setopt(ctx, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
	if ( curlRet != CURLE_OK )
		{
			HxLOG_Error(" [NXMAN]   Error: Set SSL-Version is failed...\n");
			rc = -1;
			goto EXIT_FUNC;
		}
	#endif
	(void)curl_easy_setopt(ctx, CURLOPT_SSL_VERIFYPEER, 1L);


	/* send all data to this function  */
	(void)curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, ResponseInfoCallback);

	/* we pass our 'chunk' struct to the callback function */
	s_stHttpResponse.size = 0;
	HxSTD_MemSet(s_stHttpResponse.szBuff, 0, NFLIX_MAX_STR_LEN);

	(void)curl_easy_setopt(ctx, CURLOPT_WRITEDATA, (void *)&s_stHttpResponse);

	if(pszPostData != NULL)
	{
		curl_easy_setopt(ctx, CURLOPT_POSTFIELDS, pszPostData);
	}
	else
	{
		/* use a GET to fetch this */
		(void)curl_easy_setopt(ctx, CURLOPT_HTTPGET, 1L);
	}

	/* Request */
	(void)curl_easy_setopt(ctx, CURLOPT_URL, pszUrl);

	curlRet = curl_easy_perform(ctx);
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Error(" [NXMAN]   Error: curl_easy_perform() is failed...!!\n");
		rc = -1;
	}
	else
	{

		// get some info about the xfer:
		long int rcHttp;

		// HTTP 응답코드를 얻어온다.
		if( CURLE_OK == curl_easy_getinfo( ctx , CURLINFO_HTTP_CODE , &rcHttp) )
		{
			HxLOG_Print(" [NXMAN]  HTTP_CODE : %ld\n", rcHttp);

			if(rcHttp == 200) //HTTP_OK
			{
				HxLOG_Print(" [NXMAN]  @@@@@ %lu bytes retrieved :\n", (long)s_stHttpResponse.size);
				HxLOG_Print(" [NXMAN]  \t\t received data : [%s]\n\n\n\n", s_stHttpResponse.szBuff);
			}
			else
			{
				rc = rcHttp;
			}
		}
		else
		{
			rc = -1;
		}
	}

EXIT_FUNC:
	curl_easy_cleanup(ctx);

	return rc;
}

#define NETFLIX_MAX_PPVER_LEN		36
#define NETFLIX_URL_MAX_LEN		512
#define NETFLIX_LIST_MAX_SIZE		50*1024
#define USERDB_PP_VER			"UDB/Privacy_Policy_Ver"
/* Get and check Legal Doc (Privacy & Policy) version from Humax TV Portal */
static HERROR __nxman_ParsePPVersion(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HCHAR *ucPPVersion)
{
	HERROR		eErr = ERR_FAIL;
	HxJSON_t	jsonRoot=NULL,jsonItem=NULL,jsonArray=NULL;
	HCHAR		*ucPrivacy=NULL, *ucPPKey=NULL, *ucPPSeq=NULL;
	HUINT32		i=0,ulCnt=0;

	if(ucPPVersion == NULL)
	{
		HxLOG_Error("ucPPVersion is NULL!!\n");
		return eErr;
	}

	/* json parsing */
	jsonRoot = HLIB_JSON_Decode(pRawBuffer);
	if(jsonRoot == NULL)
	{
		return eErr;
	}
	jsonArray = HLIB_JSON_Object_GetArray(jsonRoot,"list");
	if(jsonArray == NULL)
	{
		HLIB_JSON_Delete(jsonRoot);
		return eErr;
	}

	ulCnt = HLIB_JSON_GetSize(jsonArray);
	for(i=0; i < ulCnt; i++)
	{
		jsonItem = HxJSON_Array_Get(jsonArray,i);
		ucPrivacy = (HCHAR *)HLIB_JSON_Object_GetString(jsonItem,"docType");
		if(ucPrivacy == NULL)
		{
			continue;
		}
		if(!strcmp(ucPrivacy,"privacy"))
		{
			
			ucPPKey = (HCHAR *)HLIB_JSON_Object_GetString(jsonItem,"key");
			if(ucPPKey == NULL)
			{
				break;
			}

			ucPPSeq = (HCHAR *)HLIB_JSON_Object_GetString(jsonItem,"seq");
			if(ucPPSeq == NULL)
			{
				break;
			}
			HxSTD_snprintf(ucPPVersion,NETFLIX_MAX_PPVER_LEN,"%s_%s",ucPPKey,ucPPSeq);
			HxLOG_Debug("P/P Version =[%s]\n",ucPPVersion);
			eErr = ERR_OK;
		}
			
	}

	HLIB_JSON_Delete(jsonRoot);

	return eErr;	
}

static HERROR __nxman_CheckPrivacyPolicyVer(HBOOL *bUpdated)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		pLegalDocPPUrl[NETFLIX_URL_MAX_LEN] = {0,};
	HCHAR		szPPVer[NETFLIX_MAX_PPVER_LEN] = {0,};
	HCHAR		szDbPPVer[NETFLIX_MAX_PPVER_LEN] = {0,};
	HUINT32		ulDbLen=0,ulUrlLen=0;
	HUINT32		rc = 0;

	if(bUpdated == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(szDbPPVer,0x00,NETFLIX_MAX_PPVER_LEN);
	hError=APK_DB_GetStr(USERDB_PP_VER, szDbPPVer, NETFLIX_MAX_PPVER_LEN);
	if(hError != ERR_OK)
	{
		goto EXIT_FUNC;
	}

	HxSTD_MemSet(pLegalDocPPUrl, 0x00, NETFLIX_URL_MAX_LEN);

	HxSTD_snprintf(pLegalDocPPUrl, NETFLIX_URL_MAX_LEN, "%s%s", PORTAL_PP_URL, s_stSysInfo.szSystem_id);

	HxLOG_Debug("URL : %s\n", pLegalDocPPUrl);

	rc = Request_HTTPS(pLegalDocPPUrl, NULL);
	if(rc != 200 && rc != 0)
	{
		goto EXIT_FUNC;
	}

	HxLOG_Debug("Rec DATA: %s [%lu]\n", s_stHttpResponse.szBuff, s_stHttpResponse.size);

	HxSTD_MemSet(szPPVer,0x00,NETFLIX_MAX_PPVER_LEN);

	/* Parsing pulIpepgBuf data based on Json format */
	hError=__nxman_ParsePPVersion(s_stHttpResponse.szBuff, s_stHttpResponse.size, szPPVer);
	if(hError == ERR_OK)
	{
		/* Get PP version from UDB and compare szPPVer from Humax TV Server */
		ulDbLen=HxSTD_StrLen(szDbPPVer);
		ulUrlLen=HxSTD_StrLen(szPPVer);

		HxLOG_Debug("szDbPPVer = %s(%d), szPPVer = %s(%d)\n",szDbPPVer,ulDbLen, szPPVer,ulUrlLen);

		if((ulDbLen != ulUrlLen)
			|| (HxSTD_StrNCmp(szDbPPVer,szPPVer,ulUrlLen) != 0))
		{
			HxLOG_Debug("P/P ver is differ, UDB ver = %s, Server ver = %s \n",szDbPPVer, szPPVer);
			*bUpdated = TRUE;
		}
		else
		{
			HxLOG_Debug("P/P ver is same, UDB ver = %s, Server ver = %s \n",szDbPPVer, szPPVer);
			*bUpdated = FALSE;
		}
	}

EXIT_FUNC:

	return hError;
}

//#if __SUPPORT_SPLUNK_REPORT__

static void add_Netflix_RunStatus_log(Status_Type_t logType)
{
	HINT32				hError;
	Status_Log_t		*pLog = NULL;

	HUINT32				curTime;
	HINT32				nOffsetHour, nOffsetMin;
	HxDATETIME_t		logDateTime;

	/* Get Current Time */
	curTime = HLIB_STD_GetSystemTime();
	if ( curTime < VALID_UNIXTIME )
	{
		return;
	}

	hError = HLIB_DATETIME_GetOffsetTime(&nOffsetHour, &nOffsetMin);
	if (hError != ERR_OK)
	{
		return;
	}

	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(curTime, &logDateTime) != ERR_OK )
	{
		return;
	}

	pLog = (Status_Log_t*)HLIB_STD_MemAlloc(sizeof(Status_Log_t));

	if(pLog != NULL)
	{
		HxSTD_snprintf(pLog->szTimeStamp, NFLIX_SYSINFO_STR_LEN, "%d-%d-%d %02d:%02d:%02d.%03d+%02d:%02d",
			(int)logDateTime.stDate.usYear,(int)logDateTime.stDate.ucMonth,(int)logDateTime.stDate.ucDay, (int)logDateTime.stTime.ucHour,
			(int)logDateTime.stTime.ucMinute,(int)logDateTime.stTime.ucSecond, (int)logDateTime.stTime.usMillisecond, nOffsetHour, nOffsetMin);

		pLog->stLogType = logType;

		s_logList = HLIB_LIST_Append(s_logList, (void*)pLog);
		HxLOG_Print(" [NXMAN]  @@@@  Add status log (%s, %d) \n\n\n", pLog->szTimeStamp, pLog->stLogType);
	}
	else
	{
		HxLOG_Error(" [NXMAN]	 @@@@  Error> Mem allocation is failed...!! => Skip reporting \n\n\n");
	}

}


static HBOOL login_TVPortal(void)
{
	HBOOL				bRet = FALSE;
	HUINT8				szReqUrl[NFLIX_MAX_STR_LEN] = {0, };

	if(s_stSysInfo.pszAuthTicket == NULL)
	{
		HxSTD_MemSet(szReqUrl, 0, NFLIX_MAX_STR_LEN);
		HxSTD_snprintf(szReqUrl, NFLIX_MAX_STR_LEN, PORTAL_LOGIN_URL, s_stSysInfo.szMac, s_stSysInfo.szMac, s_stSysInfo.szSystem_id, s_stSysInfo.szVersion, s_stSysInfo.szDevice_id, s_stSysInfo.ulTimezone);
		HxLOG_Print(" [NXMAN]  @@@@  Login URL : [%s] \n\n\n", szReqUrl);

		if(Request_HTTPS(szReqUrl, NULL) == 0)
		{
			HxJSON_t	 json;

			json = HLIB_JSON_Decode((const HCHAR*)s_stHttpResponse.szBuff);
			if (json && HLIB_JSON_IsJSON((const HCHAR*)json, 0))
			{
				HINT32	 nLen;
				HCHAR	 *pStr;

				pStr = (HCHAR*)HLIB_JSON_Object_GetString(json, "AuthTicket");
				if(pStr != NULL)
				{
					nLen = HxSTD_StrLen(pStr);
					s_stSysInfo.pszAuthTicket = HLIB_STD_MemAlloc(nLen + 1);
					if(s_stSysInfo.pszAuthTicket != NULL)
					{
						HxSTD_snprintf(s_stSysInfo.pszAuthTicket, nLen+1, "%s", pStr);
						HxLOG_Print(" [NXMAN] \n\n\n @@@@  AuthTicket : [%s] \n\n\n", s_stSysInfo.pszAuthTicket);
						bRet = TRUE;
					}
				}

				HLIB_JSON_Delete(json);
			}
		}
	}
	else
	{
		bRet = TRUE;
	}

	return bRet;
}

static HBOOL logout_TVPortal(void)
{
	HBOOL		bRet = FALSE;
	HUINT32 	rc = 0;

	if(s_stSysInfo.pszAuthTicket != NULL)
	{
		HLIB_MEM_Free(s_stSysInfo.pszAuthTicket);
		s_stSysInfo.pszAuthTicket = NULL;
		bRet = TRUE;
	}

	return bRet;
}


static void report_Netflix_RunStatus(Status_Log_t	*pLog)
{
	HUINT8				szReqUrl[NFLIX_MAX_STR_LEN] = {0, };

	/* No need ip-addres : IP Address is updated at TV Portal server*/
#if 0
	HINT32				nDefaultDev;
	APKS_NETIFADDR_t	ptNetAddr;
	HUINT8				szIpAddr[NFLIX_MAC_ADDR_LEN];



	if(APK_NETWORK_CONF_GetDefaultDev(&nDefaultDev) != ERR_OK)
	{
		HxLOG_Error(" [NXMAN]	 Error > Getting DefaultDev is failed.\n");
		return;
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nDefaultDev, &ptNetAddr) != ERR_OK)
	{
		HxLOG_Error(" [NXMAN]	 Error > Getting the Address Information of Network is failed.\n");
		return;
	}

	if (APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulIpAddr, szIpAddr, NFLIX_MAC_ADDR_LEN) != ERR_OK)
	{
		HxLOG_Error(" [NXMAN]	 Error > Getting the IP Address is failed.\n");
		return;
	}
#endif

#if __SUPPORT_CHK_UPLOADSTATISTIC__
	//  CheckUploadStatistics
	{
		HxSTD_MemSet(szReqUrl, 0, NFLIX_MAX_STR_LEN);
		if(s_stSysInfo.pszAuthTicket != NULL)
		{
			HxSTD_snprintf(szReqUrl, NFLIX_MAX_STR_LEN, PORTAL_CHEK_UPLOAD_STATUS_URL, s_stSysInfo.pszAuthTicket);
		}
		//HxLOG_Print(" [NXMAN]  @@@@  CheckUploadStatistics URL : [%s] \n\n\n", szReqUrl);

		if(Request_HTTPS(szReqUrl, NULL) == 0)
		{
			HxJSON_t	 json;

			json = HLIB_JSON_Decode((const HCHAR*)s_stHttpResponse.szBuff);
			if (json && HLIB_JSON_IsJSON((const HCHAR*)json, 0))
			{
				HBOOL	 bStatus;

				bStatus = HLIB_JSON_Object_GetBoolean(json, "status");
				HxLOG_Print(" [NXMAN]  @@@@  CheckUploadStatistics result : [%d] \n\n\n", bStatus);
				/* Don't care status value => Even if status is false, Do UploadStatistics   */

				HLIB_JSON_Delete(json);
			}
		}
	}
#endif

	// UploadStatistics
	{
		HCHAR		szPostData[NFLIX_MAX_STR_LEN];
		HCHAR		*pszPostData = NULL;
		HxJSON_t	jsonPostData = HLIB_JSON_New(eHLIB_JSON_Object);

		HxSTD_MemSet(szReqUrl, 0, NFLIX_MAX_STR_LEN);
		if(s_stSysInfo.pszAuthTicket != NULL)
		{
			HxSTD_snprintf(szReqUrl, NFLIX_MAX_STR_LEN, PORTAL_UPLOAD_STATUS_URL, s_stSysInfo.pszAuthTicket);
		}
		//HxLOG_Print(" [NXMAN]  @@@@  UploadStatistics URL : [%s] \n\n\n", szReqUrl);

		if (jsonPostData)
		{
			HxJSON_Object_Set(jsonPostData, "TIMESTAMP", 	HxJSON_String_New(pLog->szTimeStamp));
			HxJSON_Object_Set(jsonPostData, "MAC",  		HxJSON_String_New(s_stSysInfo.szMac));
			HxJSON_Object_Set(jsonPostData, "SYSTEM_ID",   HxJSON_String_New(s_stSysInfo.szSystem_id));

			/* No need ip-addres : IP Address is updated at TV Portal server*/
			//HxJSON_Object_Set(jsonPostData, "IP_ADDR",	   	HxJSON_String_New(szIpAddr));

#if defined(CONFIG_DEBUG)
			HxJSON_Object_Set(jsonPostData, "MODE",	   	HxJSON_String_New("debug"));
#else
			HxJSON_Object_Set(jsonPostData, "MODE",	   	HxJSON_String_New("release"));
#endif

			switch(pLog->stLogType)
			{
				case STATUS_LOG_START:
					HxJSON_Object_Set(jsonPostData, "CODE",	   	HxJSON_String_New("APPIN"));
					break;

				case STATUS_LOG_STOP:
					HxJSON_Object_Set(jsonPostData, "CODE",	   	HxJSON_String_New("APPOUT"));
					break;

				case STATUS_LOG_PROVISIONING_START:
					HxJSON_Object_Set(jsonPostData, "CODE",	   	HxJSON_String_New("ESNS"));
					break;

				case STATUS_LOG_PROVISIONING_END:
					HxJSON_Object_Set(jsonPostData, "CODE",	   	 HxJSON_String_New("ESNE"));
					if(NULL != s_stSysInfo.pszProvisioningResult)
					{
					    HxJSON_Object_Set(jsonPostData, "DN_STATUS", HxJSON_String_New(s_stSysInfo.pszProvisioningResult));
					    HLIB_MEM_Free(s_stSysInfo.pszProvisioningResult);
					}
					else
					{
						if(TRUE == s_bProvisioningDone)
						{
							HxJSON_Object_Set(jsonPostData, "DN_STATUS", HxJSON_String_New("OK"));
						}
						else
						{
							HxJSON_Object_Set(jsonPostData, "DN_STATUS", HxJSON_String_New("FAIL>>NXMAN:No memory for result-string"));
						}
					}
					break;

				default:
					HxLOG_Warning(" [NXMAN]  @@@@  Unknown report_type: [%d] \n\n\n", pLog->stLogType);
					HLIB_JSON_Delete(jsonPostData);
					return;
			}

			HxJSON_Object_Set(jsonPostData, "APP_NAME",   	HxJSON_String_New("Netflix"));
			HxJSON_Object_Set(jsonPostData, "SW_VERSION",   HxJSON_String_New(s_stSysInfo.szVersion));

			pszPostData = HLIB_JSON_Encode(jsonPostData, NULL, 0);
			HxLOG_Print(" [NXMAN]  @@@@  PostData: [%s] \n\n\n", pszPostData);

			HxSTD_MemSet(szPostData, 0, NFLIX_MAX_STR_LEN);
			HxSTD_snprintf(szPostData, NFLIX_MAX_STR_LEN, "[%s]", pszPostData);

			HLIB_JSON_Delete(jsonPostData);
		}

		if(Request_HTTPS(szReqUrl, szPostData) == 0)
		{
			HxJSON_t	 json;

			json = HLIB_JSON_Decode((const HCHAR*)s_stHttpResponse.szBuff);
			if (json && HLIB_JSON_IsJSON(json, 0))
			{
				HBOOL	 bStatus;

				bStatus = HLIB_JSON_Object_GetBoolean(json, "status");
				HxLOG_Print(" [NXMAN]  @@@@  UploadStatistics result :[%d] \n\n\n", bStatus);
				/* Don't care status value   */

				HLIB_JSON_Delete(json);
			}
		}

		if(pszPostData != NULL)
		{
			HLIB_MEM_Free(pszPostData);
		}
	}
}


static void* report_proc(void * arg)
{
	Status_Log_t	*pLog = NULL;
	HxList_t		*plist = NULL;

	while(TRUE)
	{
		if((s_eCurPowerState == ePowerState_OperationMode) && (s_stSysInfo.bValid == TRUE))
		{
			HxLOG_Print(" [NXMAN] report_proc()\n");
			plist = HLIB_LIST_First(s_logList);
			if(plist == NULL)
			{
				HxLOG_Error(" [NXMAN]	 @@@@  Error >  There is no log lists. \n");
				HLIB_STD_TaskSleep(REPORTING_CHECK_PERIOD);
				continue;
			}
#if __PRIVACY_POLICY_AUTH__
			{
				HERROR hError = ERR_OK;
				HBOOL bUpdated = FALSE;
				/* Check P/P version in here */
				hError = __nxman_CheckPrivacyPolicyVer(&bUpdated);
				if(hError != ERR_OK || bUpdated == TRUE)
				{
					HxLOG_Error(" [NXMAN]	 @@@@  Error > P/P is updated. Need to agree P/P. \n");
					HLIB_STD_TaskSleep(REPORTING_CHECK_PERIOD);
					continue;
				}

				if(s_stSysInfo.pszAuthTicket == NULL)
				{
					if(login_TVPortal() == FALSE)
					{
						HxLOG_Error(" [NXMAN]	 @@@@  Error > Portal Login is fail...!!! \n\n\n");
						HLIB_STD_TaskSleep(REPORTING_CHECK_PERIOD);
						continue;
					}
				}

			}
#endif
			while(plist != NULL)
			{
				HxLOG_Print(" [NXMAN] report_proc()\n");
				pLog = (Status_Log_t*)HLIB_LIST_Data(plist);
				if(pLog != NULL)
				{
					HxLOG_Print(" [NXMAN] report_proc()\n");
					report_Netflix_RunStatus(pLog);

					s_logList = HLIB_LIST_Remove(s_logList, (const void*)pLog);
					HLIB_MEM_Free((void*)pLog);
				}
				else
				{
					s_logList = HLIB_LIST_Remove(s_logList, (const void*)pLog);
				}
				plist = HLIB_LIST_First(s_logList);

				HLIB_STD_TaskSleep(5000);
			}
		}

		HLIB_STD_TaskSleep(REPORTING_CHECK_PERIOD);
	}

	pthread_detach(pthread_self());

	HxLOG_Print(" [NXMAN]  Exit report_proc()\n");

	return NULL;
}

static void do_ReportManager(void)
{
	pthread_t	tid;

	HxLOG_Print(" [NXMAN] \t\t start Splunk report manager\n");
	pthread_create(&tid, NULL, report_proc, NULL);
}

//#endif


#if __SUPPORT_PROVISIONING__
static void *provisioning_proc(void * arg)
{
	pid_t 	launch_pid;
	int		status;
	char *const parmList[] = {NFLIX_MANUFSS_APP, NFLIX_CREDENTIALS_PATH, NFLIX_CLEAR_CREDENTIALS, "nodrm", NULL};

	if ((launch_pid = vfork()) == -1)
	{
		perror("vfork() error");
	}
	else if (launch_pid == 0)
	{
		execvp(parmList[0], parmList);
		exit(-1);
	}

	waitpid(launch_pid, &status, 0);

	pthread_detach(pthread_self());

	return NULL;
}

static void update_provisioning_result(HUINT8 *pszResult)
{
	if(s_stSysInfo.pszProvisioningResult != NULL && pszResult != NULL)
	{
		if(TRUE == s_bProvisioningDone)
		{
			HxSTD_snprintf(s_stSysInfo.pszProvisioningResult, MXT_STR_PROVISIONING_RESULT, "%s", pszResult);
		}
		else
		{
			HxSTD_snprintf(s_stSysInfo.pszProvisioningResult, MXT_STR_PROVISIONING_RESULT, "FAIL>>%s", pszResult);
		}
	}
}

#if __DO_IN_FIELD_PROVISIONING__

static void get_credentials(void)
{
	HBOOL	bGetESN = TRUE;
	HUINT8	szReqUrl[NFLIX_MAX_STR_LEN] = {0, };
	char	szErr[MXT_STR_PROVISIONING_RESULT];
	HUINT32 rc;

	HxSTD_MemSet(szErr, 0, MXT_STR_PROVISIONING_RESULT);

	HxLOG_Print(" [NXMAN]  get_credentials++\n");

	Init_SysInfo();
	if(s_stSysInfo.bValid == FALSE)
	{
		HxLOG_Error(" [NXMAN]   Error > Getting System Info is fail...!!!!!!\n");
		return;
	}

	if(login_TVPortal() == FALSE)
	{
		HxLOG_Error(" [NXMAN]	 @@@@  Error > Portal Login is fail...!!! \n\n\n");
		return;
	}

	add_Netflix_RunStatus_log(STATUS_LOG_PROVISIONING_START);

	s_stSysInfo.pszProvisioningResult = (HUINT8*)HLIB_STD_MemCalloc(MXT_STR_PROVISIONING_RESULT);
	if(s_stSysInfo.pszProvisioningResult == NULL)
	{
		HxLOG_Error(" [NXMAN]   Error > HLIB_STD_MemCalloc(MXT_STR_PROVISIONING_RESULT) is fail...!!!!!!\n");
	}

	HxSTD_snprintf(szReqUrl, NFLIX_MAX_STR_LEN, NFLIX_REQ_CREDENTIAL_URL, s_stSysInfo.szMac, s_stSysInfo.szMac, s_stSysInfo.szSystem_id, s_stSysInfo.szVersion, s_stSysInfo.szDevice_id, s_stSysInfo.ulTimezone);
	HxLOG_Print(" [NXMAN] \n\n\n @@@@  Request URL : [%s] \n\n\n", szReqUrl);

	rc = Request_HTTPS(szReqUrl, NULL);
	if(rc == 0)
	{
		HxFILE_t	file = NULL;
		HxJSON_t	 json;

		json = HLIB_JSON_Decode((const HCHAR*)s_stHttpResponse.szBuff);
		if (json && HLIB_JSON_IsJSON(json, 0))
		{
			HCHAR	 *pStr = NULL;

			if(HLIB_JSON_Object_GetBoolean(json, "Status"))
			{
				file = HLIB_FILE_Open(NFLIX_CLEAR_CREDENTIALS, "wb");
				if(file == NULL)
				{
					HxLOG_Error(" [NXMAN]   @@@@  Error > Can't make(open) %s file...!!!!!!\n\n\n\n", NFLIX_CLEAR_CREDENTIALS);
					(void)HxSTD_snprintf(szErr, MXT_STR_PROVISIONING_RESULT, "NXMAN:Can't make clear credential(ESN.idfile)");
					bGetESN = FALSE;
				}
				else
				{
					pStr = (HCHAR*)HLIB_JSON_Object_GetString(json, "KeyId");
					if(pStr != NULL)
					{
						s_ulEsnLength = HxSTD_StrLen(pStr);
						(void)HLIB_FILE_Write(file, pStr, s_ulEsnLength, 1);
						(void)HLIB_FILE_PutChar(file, '\n');
					}
					else
					{
						(void)HxSTD_snprintf(szErr, MXT_STR_PROVISIONING_RESULT, "NXMAN:JSON-No ESN");
						bGetESN = FALSE;
					}

					pStr = (HCHAR*)HLIB_JSON_Object_GetString(json, "KeyData1");
					if(pStr != NULL)
					{
						(void)HLIB_FILE_Write(file, pStr, HxSTD_StrLen(pStr), 1);
						(void)HLIB_FILE_PutChar(file, '\n');
					}
					else
					{
						(void)HxSTD_snprintf(szErr, MXT_STR_PROVISIONING_RESULT, "NXMAN:JSON-No KeyData1");
						bGetESN = FALSE;
					}

					pStr = (HCHAR*)HLIB_JSON_Object_GetString(json, "KeyData2");
					if(pStr != NULL)
					{
						(void)HLIB_FILE_Write(file, pStr, HxSTD_StrLen(pStr), 1);
					}
					else
					{
						(void)HxSTD_snprintf(szErr, MXT_STR_PROVISIONING_RESULT, "NXMAN:JSON-No KeyData2");
						bGetESN = FALSE;
					}

					HLIB_FILE_Close(file);

					if(bGetESN == FALSE)
					{
						VK_SYSTEM_Command("rm /var/lib/humaxtv_user/netflix/ESN.idfile");
					}
				}
			}
			else
			{
				HCHAR	 *pStrMsg = NULL;
				HCHAR	 *pStrPos = NULL;

				pStrMsg = (HCHAR*)HLIB_JSON_Object_GetString(json, "Message");
				pStrPos = (HCHAR*)HLIB_JSON_Object_GetString(json, "Step");

				(void)HxSTD_snprintf(szErr, MXT_STR_PROVISIONING_RESULT, "%s:%s", pStrPos, pStrMsg);
				bGetESN = FALSE;
			}

			HLIB_JSON_Delete(json);
		}
		else
		{
			(void)HxSTD_snprintf(szErr, MXT_STR_PROVISIONING_RESULT, "NXMAN:JSON is invalid");
			bGetESN = FALSE;
		}
	}
	else
	{
		(void)HxSTD_snprintf(szErr, MXT_STR_PROVISIONING_RESULT, "NXMAN:HTTPS-failed(%d)", rc);
		bGetESN = FALSE;
	}

	if(bGetESN == FALSE)
	{
		HxLOG_Error(" [NXMAN]   \t\t Get ESN Error  : %s \n\n", szErr);

		s_bProvisioningDone = FALSE;
		update_provisioning_result(szErr);

		if (APK_SYSINFO_STB_SetNetflixESN(szErr) != ERR_OK)
		{
			HxLOG_Error(" [NXMAN]   Error: Setting Esn to BoxInfo..\n");
		}
	}
	if(logout_TVPortal() == TRUE)
	{
		HxLOG_Error(" [NXMAN]	 @@@@ Portal Logout done... \n");
	}
	else
	{
		HxLOG_Error(" [NXMAN]	 @@@@ Portal Logout fail or not login before... \n");
	}
	//Not completed, yet.
	/*
	else
	{
		s_bProvisioningDone = TRUE; //
	}
	*/
}
#endif

static void do_provisioning(void)
{
	int			timeout = 70;
	pthread_t	tid;
	struct stat sts;

	chdir("/usr/bin/netflix");

#if __FIX_PROVISIOING_BUG__
	if((stat(NFLIX_FIX_BUG_LOG, &sts) == -1 && errno == ENOENT) && (stat(NFLIX_CREDENTIALS, &sts) == 0))
	{
		HxFILE_t	file = NULL;

		HxLOG_Print(" [NXMAN]  @@ Delete %s.... to fix Provisioning Bug\n", NFLIX_CREDENTIALS);
		VK_SYSTEM_Command("rm /var/lib/humaxtv_user/netflix/manuf_ss.bin");

		file = HLIB_FILE_Open(NFLIX_FIX_BUG_LOG, "wb");

		if(file == NULL)
		{
			HxLOG_Error(" [NXMAN]   @@@@  Error > Can't make(open) %s file...!!!!!!\n\n\n\n", NFLIX_FIX_BUG_LOG);
		}
		else
		{
			(void)HLIB_FILE_PutChar(file, '1');
			HLIB_FILE_Close(file);
		}
	}
#endif

#if __USB_PROVISIONING__
	if(stat(NFLIX_USB_CREDENTIALS, &sts) == 0)
	{
		if(stat(NFLIX_CREDENTIALS, &sts) == 0)
		{
			HxLOG_Print(" [NXMAN]  @@ Delete %s.... \n", NFLIX_CREDENTIALS);
			VK_SYSTEM_Command("rm /var/lib/humaxtv_user/netflix/manuf_ss.bin");
		}

		HxLOG_Print(" [NXMAN]  @@ Copy USB Credentials to Temp-Credentials(%s) \n", NFLIX_CLEAR_CREDENTIALS);
		VK_SYSTEM_Command("cp /media/drive1/ESN.idfile /var/lib/humaxtv_user/netflix/ESN.idfile");
	}
#endif

	if(stat(NFLIX_CREDENTIALS, &sts) == -1 && errno == ENOENT)
	{
		if(stat(NFLIX_CLEAR_CREDENTIALS, &sts) == -1 && errno == ENOENT)
		{
#if __PRIVACY_POLICY_AUTH_IN_FIELD_PROVISION__
			{
				HERROR		hError = ERR_OK;
				HBOOL		bUpdated = FALSE;
				/* Check P/P version in here */
				hError = __nxman_CheckPrivacyPolicyVer(&bUpdated);
				if(hError != ERR_OK || bUpdated == TRUE)
				{
					HxLOG_Error(" [NXMAN]	 @@@@  Error > P/P is updated. Need to agree P/P. \n");
					return;		
				}
			}
#endif
#if __DO_IN_FIELD_PROVISIONING__
			get_credentials();
#endif
		}

		if(stat(NFLIX_CLEAR_CREDENTIALS, &sts) == -1 && errno == ENOENT)
		{
			HxLOG_Warning(" [NXMAN]	 @@ No credentials(%s)....Exit..!!\n", NFLIX_CLEAR_CREDENTIALS);
			add_Netflix_RunStatus_log(STATUS_LOG_PROVISIONING_END);
			return;
		}
		else
		{
			HxLOG_Print(" [NXMAN]  @@ start provisioning..!!\n");
			pthread_create(&tid, NULL, provisioning_proc, NULL);
		}
	}
	else
	{
		s_bProvisioningDone = TRUE;
		return;
	}

	while((timeout!= 0) && (stat(NFLIX_CREDENTIALS, &sts) == -1) && (errno == ENOENT))
	{
		VK_TASK_Sleep(100);
		timeout--;
	}

	if(timeout != 0)
	{
		char			szEsn[64];
		HxFILE_t		file = NULL;
		HUINT32			ulSize = 0;

		HxSTD_MemSet(szEsn, 0, 64);

		file = HLIB_FILE_Open(NFLIX_CLEAR_CREDENTIALS, "r");
		if(file)
		{
			ulSize = HLIB_FILE_Read(file, szEsn, sizeof(char), s_ulEsnLength);
			HLIB_FILE_Close(file);

			if(ulSize < s_ulEsnLength)
			{
				HxLOG_Warning(" [NXMAN]   Warning: Wrong ESN Length(%d)..!!\n", ulSize);
			}

			szEsn[s_ulEsnLength] = '\0';
			HxLOG_Print(" [NXMAN]  \t\t ESN(%d) : %s \n\n", s_ulEsnLength, szEsn);

			if (APK_SYSINFO_STB_SetNetflixESN(szEsn) != ERR_OK)
			{
				HxLOG_Error(" [NXMAN]   Error: Setting Esn to BoxInfo..\n");
			}
		}

		s_bProvisioningDone = TRUE;
		update_provisioning_result("OK");

		VK_SYSTEM_Command("rm /var/lib/humaxtv_user/netflix/ESN.idfile");
	}
	else
	{
		s_bProvisioningDone = FALSE;
		update_provisioning_result("NXMAN:Can't make secure credential(manuf_ss.bin)");
	}

	add_Netflix_RunStatus_log(STATUS_LOG_PROVISIONING_END);
}
#endif

/*
static HCHAR * __url_encode(const HCHAR * in) {

   HCHAR * encbuf = NULL;
   const HCHAR * p = NULL;
   HCHAR * e = NULL;
   int len = 0;

   if (!in) {
	  return NULL;
   }

   len = HxSTD_StrLen(in) * 3;

   encbuf = (HCHAR*)HLIB_STD_MemCalloc(len + 1);
   if (!encbuf) {
	  return NULL;
   }

   HxSTD_MemSet(encbuf, 0, len + 1);

   p = in;
   e = encbuf;
   while (*p) {
	  const HCHAR c = *(p++);

	  if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
	  {
		 *e++ = c;
	  }
	  else
	  {
		 HCHAR hex[3] = {0,};
		 HxSTD_snprintf(hex, sizeof(hex), "%02X", c);
		 *e++ = '%';
		 *e++ = hex[0];
		 *e++ = hex[1];
	  }
   }

   return encbuf;
}
*/

#if		__CLEAR_DUPLICATE_PROCESS__


static void local_getStatus(char *file, char *buf)
{
	HxFILE_t	hfile = NULL;

	hfile = HLIB_FILE_Open(file, "r");
	if(hfile != NULL)
	{
		(void)HLIB_FILE_Read(hfile, buf, 1, 64);
		buf[63] = 0;
		(void)HLIB_FILE_Close(hfile);
	}
}

static HUINT32		GetCount_netflix(void)
{
	DIR *dir;
	struct dirent	*entry;
	struct stat		fileStat;
	HUINT32			ulNxCnt = 0;

	int pid;
	char szTempPath[128];
	char szStatus[64];

	HxLOG_Print("GetCount_netflix\n");

	dir = opendir("/proc");

	while ((entry = readdir(dir)) != NULL)
	{
		if(lstat(entry->d_name, &fileStat) != 0)
		{
			continue;
		}

		if (!S_ISDIR(fileStat.st_mode))
		{
			continue;
		}

		//HxLOG_Print("entry->d_name : %s\n", entry->d_name);
		pid = atoi(entry->d_name);
		if(pid <= 700) // skip foundation(base) process
		{
			continue;
		}

		snprintf(szTempPath, 128, "/proc/%d/status", pid);
		//HxLOG_Print("\t\t  @@@@ %s\n", szTempPath);

		memset(szStatus, 0, 64);
		local_getStatus(szTempPath, szStatus);
		HxLOG_Print("\t\t\t @@@@ status : %s\n", szStatus);

		if((strstr(szStatus, "netflix") != NULL) && (strstr(szStatus, "zombie") == NULL))
		{
			HxLOG_Print("\t\t @@@@ Find Netflix(%d)\n", pid);
			ulNxCnt++;
		}
	}
	closedir(dir);

	HxLOG_Print("\t\t\t @@@@ ulNxCnt: %d\n", ulNxCnt);

	return ulNxCnt;
}

#endif

static HBOOL check_network(void)
{
	HERROR				hError;
	HINT32				nDefaultDev = 0;
	APKS_NETDEVINFO_t	stDevInfo;

	if(APK_NETWORK_CONF_GetDefaultDev(&nDefaultDev) == ERR_OK)
	{
		hError = APK_NETWORK_CONF_GetDevInfo(nDefaultDev, &stDevInfo);

		if(hError == ERR_OK && stDevInfo.bConnected && stDevInfo.bNetActive)
		{
			s_bNetActivated = TRUE;
			HxLOG_Print(" [NXMAN] \t @@ Network is OK..!!\n");
		}
		else
		{
			s_bNetActivated = FALSE;
			//HxLOG_Print(" [NXMAN] \t @@ Network is NG..!!\n");
		}
	}
	else
	{
		s_bNetActivated = FALSE;
	}

	return s_bNetActivated;
}


static HBOOL check_time(void)
{
	unsigned long 		ulCurTime = 0x0;
	HERROR				hError;
	VK_CLOCK_SET_MODE	eVKTimeMode = VK_CLOCK_SETBY_NONE;
	static 	 int		snReq = 0;



	//HxLOG_Print(" [NXMAN]  check_time++\n");

	hError = VK_CLOCK_GetTime(&ulCurTime);
	if(hError != VK_INVALID_CLOCK)
	{
		HxLOG_Print(" [NXMAN] @@ Valid time is set..!!\n");
		s_bTimeSetted = TRUE;
		return s_bTimeSetted;
	}

	hError = VK_CLOCK_GetTimeSource(&eVKTimeMode);
	if((hError == ERR_OK) && (eVKTimeMode == VK_CLOCK_SETBY_SI || eVKTimeMode == VK_CLOCK_SETBY_MICOM))
	{
		HxLOG_Print(" [NXMAN] \t @@ time is set by SI or MICOM..!!\n");
		s_bTimeSetted = TRUE;
		return s_bTimeSetted;
	}

	if(snReq != 0)
	{
		snReq = 0;
		return FALSE;
	}
	else
	{
		snReq = 1;
	}

	if(s_ulTryConnt < MAX_TIME_CHECK_COUNT)
	{
		//HxLOG_Print(" [NXMAN] \t @@ Run NTCP Client...\n");
#if defined(CONFIG_PROD_FVP4000T)
		VK_SYSTEM_Command("ntpclient -s -h uk.pool.ntp.org &");
#elif defined(CONFIG_PROD_HDR3000T)
		VK_SYSTEM_Command("ntpclient -s -h au.pool.ntp.org &");
#else
		VK_SYSTEM_Command("ntpclient -s -h de.pool.ntp.org &");
#endif
		VK_SYSTEM_Command("touch /var/lib/humaxtv/resolv.conf");
		s_ulTryConnt++;
	}

	{
		hError = VK_CLOCK_GetTime(&ulCurTime);
		if(hError != VK_INVALID_CLOCK)
		{
			s_bTimeSetted = TRUE;
		}
		else
		{
			s_bTimeSetted = FALSE;
		}
	}

	HxLOG_Print(" [NXMAN] \t check_time--(%d)\n", s_bTimeSetted);
	return s_bTimeSetted;
}


#if __USE_CALLBACK_FOR_DIAL_CHECKING__

static void init_DialInfo(void)
{
	HxSEMT_Get(s_ulDialSem);

	HxSTD_MemSet(&s_stDialInfo, 0, sizeof(NFLIX_DialInfo_t));
	HxSTD_snprintf(s_stDialInfo.szAdditionalUrl, MAX_ENCORDED_URL_LEN, "%s", DIAL_ADDITIONALDATAURL_DEFAULT);

	HxSEMT_Release(s_ulDialSem);
}

static void check_DIAL(void)
{
	HxSEMT_Get(s_ulDialSem);

	if(s_stDialInfo.bHommaActivated == TRUE)
	{
		if(s_stDialInfo.bDialActivated == FALSE)
		{
			if ((APK_DIAL_GetDialStatus(&s_stDialInfo.bDialActivated) != ERR_OK) || (s_stDialInfo.bDialActivated == FALSE))
			{
				HxLOG_Print(" [NXMAN]  @@  DIAL NOT activated..!! \n");
			}
			else
			{
				HxSTD_MemSet(s_stDialInfo.szAdditionalUrl, 0, MAX_ENCORDED_URL_LEN);
				if(APK_DIAL_GetAdditionalDataUrl(s_stDialInfo.szAdditionalUrl, MAX_ENCORDED_URL_LEN, "Netflix") != ERR_OK)
				{
					HxLOG_Print(" [NXMAN] \t #### No AdditionalUrl...!!!\n\n");
				}
				else
				{
					HxLOG_Print(" [NXMAN] \t #### AdditionalUrl: [%s]\n\n", s_stDialInfo.szAdditionalUrl);
					HxSTD_snprintf(s_stDialInfo.szAdditionalUrl, MAX_ENCORDED_URL_LEN, "%s", DIAL_ADDITIONALDATAURL_DEFAULT);
				}
			}
		}
	}
	else
	{
		HxLOG_Print(" [NXMAN]  @@  HOMMA NOT activated..!! \n");
	}

	HxSEMT_Release(s_ulDialSem);

	return;
}

#else

static void init_DialInfo(void)
{
	s_bDialActivated = FALSE;
	s_ulDialCheckCount = 0;
}

static void check_DIAL(void)
{
	HBOOL 				dialActivated = FALSE;
	HINT32				ulCurTime, ulDiff;

	ulCurTime = VK_TIMER_GetSystemTick();

	ulDiff = ulCurTime - s_ulDialCheckStartTime;

	if(s_bDialActivated == FALSE)
	{
		if(ulDiff < NFLIX_DIAL_CHECKING_PERIOD) return;
		s_ulDialCheckCount++;
	}
	else
	{
		if(ulDiff < NFLIX_DIAL_UPDATING_PERIOD) return;
	}

	s_ulDialCheckStartTime = ulCurTime;

	HxSTD_MemSet(s_szAdditionalUrl, 0, MAX_ENCORDED_URL_LEN);
	//HxLOG_Print(" [NXMAN]  @@  s_ulDialCheckCount(%d)..!! \n", s_ulDialCheckCount);
	if ((APK_DIAL_GetDialStatus(&dialActivated) != ERR_OK) || (dialActivated == FALSE))
	{
		//HxLOG_Print(" [NXMAN]  @@  Error: DIAL NOT activated(%d)..!! \n", dialActivated);
		s_bDialActivated = FALSE;
	}
	else
	{		
		if(APK_DIAL_GetAdditionalDataUrl(s_szAdditionalUrl, MAX_ENCORDED_URL_LEN, "Netflix") != ERR_OK)
		{
			HxLOG_Print(" [NXMAN] \t\t @@@@@ No AdditionalUrl: [%s]...!!!\n\n", s_szAdditionalUrl);
			s_bDialActivated = FALSE;
		}
		else
		{
			HxLOG_Print(" [NXMAN] \t\t @@@@@ AdditionalUrl: [%s]\n\n", s_szAdditionalUrl);
			s_bDialActivated = TRUE;
		}
	}

	if(s_bDialActivated == FALSE)
	{
		HxSTD_snprintf(s_szAdditionalUrl, MAX_ENCORDED_URL_LEN, "%s", DIAL_ADDITIONALDATAURL_DEFAULT);
	}
}

#endif

static void Init_cache(void)
{
	struct stat sts;

	if(s_bFirstBoot == FALSE)
	{
		return;
	}

	/* Cache Info
		/usr/bin/netflix/data/cache -> /mnt/hd2/Media/Download : DiskStore
		/usr/bin/netflix/data/var -> /var/lib/humaxtv_user/netflix/var : DiskCache
	*/

	if(stat(NFLIX_DISK_CACHE_PATH, &sts) == 0)
	{
		HxLOG_Print(" [NXMAN]  ==== Clear Netflix Var for factory reset!!!!!!!\n\n\n\n");
		// Netflix var clear
		VK_SYSTEM_Command("rm -rf /var/lib/humaxtv_user/netflix/var");
	}
}


static void check_diskcache(void)
{
	struct stat sts;
	char		szCmd[256];

#if __RECOVER_USER_DB__
	HxSTD_MemSet(szCmd, 0, 256);
	HxSTD_snprintf(szCmd, 256, "touch %s", NFLIX_USER_PARTITION_TOUCH_FILE);
	VK_SYSTEM_Command(szCmd);

	if(stat(NFLIX_USER_PARTITION_TOUCH_FILE, &sts) == -1 && errno == ENOENT)
	{
#if	__SUPPORT_USB_LOGGING__
		VK_SYSTEM_Command("df > /media/drive1/df.log");
#endif

		HxLOG_Critical(" [NXMAN]	 @@@@  Error > Recreate USER_DB Partition... \n");
		if(DI_NANDFLASH_RecreatePartition(FLASH_DBUSER) == ERR_OK)
		{
			HxLOG_Warning(" [NXMAN]	\t @@@@  USER_DB Partition was recovered...!!!!!!\n\n\n\n");
		}
	}
	else
	{
		HxSTD_MemSet(szCmd, 0, 256);
		HxSTD_snprintf(szCmd, 256, "rm %s", NFLIX_USER_PARTITION_TOUCH_FILE);
		VK_SYSTEM_Command(szCmd);
	}
#endif

	if(stat(NFLIX_PERSISTENET_STORAGE_PATH, &sts) == -1 && errno == ENOENT)
	{
		HxSTD_MemSet(szCmd, 0, 256);
		HxSTD_snprintf(szCmd, 256, "mkdir -p %s", NFLIX_PERSISTENET_STORAGE_PATH);
		HxLOG_Print(" [NXMAN]  @@ Make persistent storage...!!\n");
		VK_SYSTEM_Command(szCmd);
	}

	if(stat(NFLIX_DISK_CACHE_PATH, &sts) == -1 && errno == ENOENT)
	{
		HxSTD_MemSet(szCmd, 0, 256);
		HxSTD_snprintf(szCmd, 256, "mkdir -p %s", NFLIX_DISK_CACHE_PATH);
		HxLOG_Print(" [NXMAN]  @@ Make disk-cache ...!!\n");
		VK_SYSTEM_Command(szCmd);
	}
}

static void check_diskstore(void)
{
	struct stat sts;
	char		szCmd[256];

	/* Waiting for mounting hdd for DiskStore
		/usr/bin/netflix/data/cache -> /mnt/hd3 : DiskStore
	*/
	HxSTD_MemSet(szCmd, 0, 256);
	HxSTD_snprintf(szCmd, 256, "touch %s", NFLIX_DISK_STORE_TOUCH_FILE);
	while(1)
	{
		VK_SYSTEM_Command(szCmd);

		if(stat(NFLIX_DISK_STORE_TOUCH_FILE, &sts) == -1 && errno == ENOENT)
		{
			HLIB_STD_TaskSleep(1500);
		}
		else
		{
			HxLOG_Print(" [NXMAN]  @@ %s is OK !!\n", NFLIX_DISK_STORE_TOUCH_FILE);
			s_bDiskStoreReady = TRUE;
			HxSTD_MemSet(szCmd, 0, 256);
			HxSTD_snprintf(szCmd, 256, "rm %s", NFLIX_DISK_STORE_TOUCH_FILE);
			VK_SYSTEM_Command(szCmd);
			break;
		}
	}

	if(s_bFirstBoot == TRUE)
	{
		HxLOG_Print(" [NXMAN]  @@ FactoryReset=>Initalize DiskStore  !!\n");
		HxSTD_MemSet(szCmd, 0, 256);
		HxSTD_snprintf(szCmd, 256, "rm -rf %s/*", NFLIX_DISK_STORE_PATH);
		VK_SYSTEM_Command(szCmd);
	}
}

static void notify_activated(HBOOL bForceNotify)
{
	HBOOL							bActivated;
	struct	stat					sts;
	static	NFLIX_NotifyState_t		stState;

	if(stat(NFLIX_ACTIVATED_INFO_PATH, &sts) == -1 && errno == ENOENT)
	{
		bActivated = FALSE;
		stState = NFLIX_NOTIFY_DEACTIVATED;
	}
	else
	{
		bActivated = TRUE;
		stState = NFLIX_NOTIFY_ACTIVATED;
	}

	if((bForceNotify == TRUE) || (s_bNFlixActivated != bActivated))
	{
		s_bNFlixActivated = bActivated;

		HxLOG_Error(" [NXMAN] \t #### Notify activate-state, broadcat signal:onNetflixStateChanged(%d)..\n\n", (HUINT32)stState);
		(void)HAPI_BroadcastSignal("signal:onNetflixStateChanged", 0, "i", stState);
	}
}

static void app_started(void)
{
	HxLOG_Print(" [NXMAN] \t #### start app, broadcat signal:onNetflixStateChanged(1)..\n\n");
	if (HAPI_BroadcastSignal("signal:onNetflixStateChanged", 0, "i", NFLIX_NOTIFY_START_NORMAL) != ERR_OK)
	{
		HxLOG_Error(" [NXMAN]   Error> failed to HAPI_SendSignalNumber(signal:onNetflixStateChanged(1))\n");
		(void)HAPI_BroadcastSignal("signal:onNetflixStateChanged", 0, "i", NFLIX_NOTIFY_START_NORMAL);
	}

	s_eNetflixPlaybackState = NFLIX_NOTIFY_PLAYBACK_NA; /* TODO: sema lock */

	(void)APK_OUTPUT_HDMI_SetNetflixDrmStatus(eNFLIX_DRM_INIT);
	HxLOG_Print(" [NXMAN]  APK_OUTPUT_HDMI_SetNetflixDrmStatus (eNFLIX_DRM_INIT)\n");

#if __SUPPORT_SPLUNK_REPORT__
	add_Netflix_RunStatus_log(STATUS_LOG_START);
#endif
}


static void app_exited(void)
{
	HxLOG_Print(" [NXMAN] \t @@@@ exit app, broadcat eHSIG_NetflixStateChanged(0x%X)..\n\n", s_stExitState);
	if (HAPI_BroadcastSignal("signal:onNetflixStateChanged", 0, "i", s_stExitState) != ERR_OK)
	{
		HxLOG_Error(" [NXMAN]   Error> failed to HAPI_SendSignalNumber(eHSIG_NetflixStateChanged:%d) again\n", s_stExitState);
		(void)HAPI_BroadcastSignal("signal:onNetflixStateChanged", 0, "i", s_stExitState);
	}

	/* s_eNetflixPlaybackState = NFLIX_NOTIFY_PLAYBACK_NA; */

	(void)APK_OUTPUT_HDMI_SetNetflixDrmStatus(eNFLIX_DRM_NA);
	HxLOG_Print(" [NXMAN]  APK_OUTPUT_HDMI_SetNetflixDrmStatus (eNFLIX_DRM_NA)\n");

#if __SUPPORT_SPLUNK_REPORT__
	add_Netflix_RunStatus_log(STATUS_LOG_STOP);
#endif

	notify_activated(FALSE);
}

static void init_Overlay(void)
{
	s_eOverlayState = NFLIX_OVERLAY_NA;
	s_ulLastOverlayPopupStartTime = 0x0; /* GetSystemTick */
	s_ulLastOverlayPopupEndTime = 0x0;
	s_ulLastOverlayCloseStartTime = 0x0;
	s_ulLastOverlayCloseEndTime = 0x0;
	s_ulLastOverlayEventTime = 0x0;
	s_ulLOverlayEventCnt = 0x0;
}

static void do_controlOverlay(NFLIX_Overlay_Ctrl_t   eCtrlOverlay)
{
	NFLIX_OverlayPlaybackState_t	ePrevOverlayAction = s_eOverlayPlaybackAction;

	HxSEMT_Get(s_ulOverlaySem);
	if((s_eNflixState != NFLIX_FOREGROUNDED) || (s_eNetflixSuspendStatus < NFLIX_SunpendStatus_Completed) || (s_bAMRunning == false))
	{
		HxLOG_Print(" [NXMAN]  Exit>> [OverlayPlayback] s_eNetflixSuspendStatus(%d) s_bAMRunning(%d)\n", s_eNetflixSuspendStatus, s_bAMRunning);
		goto func_done;
	}

	HxLOG_Print(" [NXMAN] \t do_controlOverlay++ : NflixState(%d), eCtrlOverlay(0x%X)\n", s_eNflixState, eCtrlOverlay);

	if(eCtrlOverlay <= NFLIX_OVERLAY_POPUP_WITHOUT_KEY) // Dialog pop-up
	{
		s_eOverlayState = eCtrlOverlay;

		if ((ePrevOverlayAction == NFLIX_OVERLAY_PBACTION_RESUMED) || (ePrevOverlayAction == NFLIX_OVERLAY_PBACTION_NA))
		{
			if((s_eNetflixPlaybackState == NFLIX_NOTIFY_PLAYBACK_STARTING) || (s_eNetflixPlaybackState == NFLIX_NOTIFY_PLAYBACK_PLAYING))
			{
				s_eOverlayPlaybackAction = NFLIX_OVERLAY_PBACTION_PAUSED;
				HxLOG_Print(" [NXMAN]  [OverlayPlayback] action(%d -> %d) playState(%d) --> PAUSE\n", ePrevOverlayAction, s_eOverlayPlaybackAction, s_eNetflixPlaybackState);
				HLIB_QUEUE_Push(s_pAMQueue, (void*)CMD_CNT_KEY_PAUSE);
			}
		}
	}
	else //  Dialog close
	{
		s_eOverlayState = eCtrlOverlay;
		if(eCtrlOverlay == NFLIX_OVERLAY_CLOSE_WITH_EXIT) // ok by eHSIG_NetflixNotifyOverlay
		{
			s_eOverlayPlaybackAction = NFLIX_OVERLAY_PBACTION_NA;
			HxLOG_Print(" [NXMAN]  [OverlayPlayback] action(%d -> %d) playState(%d) --> SUSPEND\n", ePrevOverlayAction, s_eOverlayPlaybackAction, s_eNetflixPlaybackState);
			HLIB_QUEUE_Push(s_pAMQueue, (void*)CMD_CNT_SUSPEND);
		}
		else // cancel => NFLIX_OVERLAY_CLOSE_WITHOUT_EXIT
		{
			/* Don't use obama's mediapb state since the state change is reflected by late event */
			/* Instead, check the current status based on the last overlay pause/resume command. */
			if (ePrevOverlayAction == NFLIX_OVERLAY_PBACTION_PAUSED)
			{
				s_eOverlayPlaybackAction = NFLIX_OVERLAY_PBACTION_RESUMED;
				HxLOG_Print(" [NXMAN]  [OverlayPlayback] action(%d -> %d) playState(%d) --> PLAY\n", ePrevOverlayAction, s_eOverlayPlaybackAction, s_eNetflixPlaybackState);
				HLIB_QUEUE_Push(s_pAMQueue, (void*)CMD_CNT_KEY_PLAY);
			}
		}
	}

func_done:
	HxSEMT_Release(s_ulOverlaySem);
	return;
}

static void do_ApplicationManagerController(NFLIX_Launch_Source_Type_t srcType, NFLIX_Exit_Reason_Type_t exitType)
{
	Nflix_ControllerCmd_t	eAMC_cmd = CMD_CNT_NONE;
	/*pthread_t				tid;

	HUINT32 ulDiff = 0x0;
	HBOOL checkLastSuspend = TRUE;

	HBOOL skipActionBySuspendCondition = FALSE;
	HBOOL needToDelayedCheck = FALSE;
	HBOOL needToDelayedResume = FALSE;
	HUINT32 ulCurTime = 0x0;*/

	if(s_bAMRunning == false) return;

	HxLOG_Print(" [NXMAN]  do_ApplicationManagerController++ s_eNflixState(%d), src(%d), exit(%d)\n", s_eNflixState, srcType, exitType);

	switch(srcType)
	{
		case Source_NETFLIX_BUTTON: 				// 1
		case Source_DEDICATED_ON_SCREEN_ICON:		// 2,
		case Source_ITEM_IN_APPLICATION_LIST:		// 3
		case Source_iBANNER_AD:						// 9
		case Source_EPG_GRID:						// 17,
		case Source_CHANNEL_NUMBER:					// 18,
		case Source_DIAL:							// 12,
		case Source_POWER_ON_FROM_NETFLIX_BUTTON:	// 19
		case Source_SUSPENDED_CHANNEL_SURF:			// 25
		case Source_SUSPENDED_CHANNEL_INFO_BAR:		// 26
		case Source_NETFLIX_PRE_APP:				// 27
		case Source_PRE_APP_ICON:					// 28
		case Source_OS_PRE_APP:						// 30
			eAMC_cmd = CMD_CNT_START_FOREGROUND;
			break;

		case Source_SUSPENDED_AT_POWER_ON:			// 22,
			eAMC_cmd = CMD_CNT_START_SUSPEND;
			break;

		case Source_SUSPENDED_AFTER_APP_RESTART:	// 23,
			eAMC_cmd = CMD_CNT_START_SUSPEND;
			break;

		case Source_NONE:
			break;

		default:
			eAMC_cmd = CMD_CNT_NONE;
			break;
	}

	switch(exitType)
	{
		case Reason_USER_Kill:					// 1 : User requests Netflix application to self-terminate
			eAMC_cmd = CMD_CNT_STOP_USER_KILL;
			s_stExitState = NFLIX_NOTIFY_EXIT_NORMAL;
			break;

		case Reason_USER_Nav:					// 2 : User navigated away from application (ex: press Home/EPG)
#if __SUPPORT_NETFLIX_BACKGROUND__
			if(s_bGoToBackground == TRUE)
			{
   				eAMC_cmd = CMD_CNT_BACKGROUND;
			}
			else
			{
				eAMC_cmd = CMD_CNT_SUSPEND;
			}
#else
			eAMC_cmd = CMD_CNT_SUSPEND;
#endif
			s_stExitState = NFLIX_NOTIFY_EXIT_NORMAL;
			break;

		case Reason_DIAL_Stop:					// 3 : DIAL issuing 'stop' to application
			eAMC_cmd = CMD_CNT_STOP_DIAL;
			s_stExitState = NFLIX_NOTIFY_EXIT_DIAL;
			break;

		case Reason_DIAL_Other_Launch:
			eAMC_cmd = CMD_CNT_STOP_DIAL;
			s_stExitState = NFLIX_NOTIFY_EXIT_DIAL_OTHER_LAUNCH;
			break;

		case Reason_AM_LowResource:				// 4 : AM terminated application due to low resources
			eAMC_cmd = CMD_CNT_STOP_LOW_RESOURCES;
			s_stExitState = NFLIX_NOTIFY_EXIT_NORMAL;
			break;

		case Reason_STANDBY_ON:
			eAMC_cmd = CMD_CNT_EXIT;
			s_stExitState = NFLIX_NOTIFY_EXIT_STANDBY;
			break;

		case Reason_FACTORY_RESET:
			s_bFactoryReset = TRUE;
			//do_factoryr_reset_kill();
			eAMC_cmd = CMD_CNT_NONE;
			s_stExitState = NFLIX_NOTIFY_EXIT_STANDBY;
			break;

		case Reason_NONE:
			break;

		default:
			eAMC_cmd = CMD_CNT_NONE;
			break;
	}

	//HxLOG_Print(" [NXMAN]   AMC Command(%d, %s) -- suspend(%d/%d)\n", eAMC_cmd, AM_ControllerCommandToString(eAMC_cmd), s_bNetflixSuspending, s_bNetflixSuspendCompleted);

	if(eAMC_cmd != CMD_CNT_NONE)
	{
		/* Execute command */
		HxLOG_Print(" [NXMAN]  Send %s to AM -- eNflixState(%d), src(%d), exit(%d)\n", AM_ControllerCommandToString(eAMC_cmd), s_eNflixState, srcType, exitType);
		HLIB_QUEUE_Push(s_pAMQueue, (void*)eAMC_cmd);
	}
	else
	{
		HxLOG_Warning(" [NXMAN]  Error> CMD_CNT_NONE... Whay? -- eNflixState(%d), src(%d), exit(%d)\n", s_eNflixState, srcType, exitType);
	}

	return;
}

static void do_ApplicationManager(void)
{
	pthread_t	tid;

	if(s_pAMQueue != NULL)
	{
		HLIB_QUEUE_Delete(s_pAMQueue);
	}

	s_pAMQueue = HLIB_QUEUE_New(NULL);

	HxLOG_Print(" [NXMAN] \t start AM\n");
	pthread_create(&tid, NULL, AM_proc, NULL);
}


#if __USE_CALLBACK_FOR_DIAL_CHECKING__

static HBOOL make_LaunchArgs(void)
{

	HBOOL	bRet = TRUE;
#if defined(CONFIG_NETFLIX_BARKER_CHANNEL)
	HCHAR   szPayload[256];//, *pszEncodedPayload = NULL;
#endif

	s_nArgc = 0;

	HxSTD_MemSet(s_pArgv, 0, sizeof(s_pArgv));
	HxSTD_MemSet(s_szUiQuery, 0, MAX_NFLIX_UI_QUERY_STR);

	s_pArgv[s_nArgc++] = NFLIX_APP;
	s_pArgv[s_nArgc++] = NFLIX_CREDENTIALS_STR;

	HxSEMT_Get(s_ulDialSem);

	switch(s_stLaunchParam.enSrc)
	{
		case Source_NETFLIX_BUTTON: 				// 1
		case Source_DEDICATED_ON_SCREEN_ICON:		// 2
		case Source_ITEM_IN_APPLICATION_LIST:		// 3
		case Source_POWER_ON_FROM_NETFLIX_BUTTON:	// 19
		case Source_SUSPENDED_AT_POWER_ON:			// 22
		case Source_SUSPENDED_AFTER_APP_RESTART:	// 23
		case Source_OS_PRE_APP:						// 30
			HxSTD_snprintf(s_szUiQuery, MAX_NFLIX_UI_QUERY_STR-1, NFLIX_UI_QUERY_STR_NORMAL, s_stLaunchParam.enSrc, s_stDialInfo.szAdditionalUrl);
			HxSTD_snprintf(s_szNxPayload, MAX_MFLIX_PAYLOAD_STR-1, NFLIX_NX_STR_SRC, s_stLaunchParam.enSrc, s_stDialInfo.szAdditionalUrl);
			s_pArgv[s_nArgc++] = s_szUiQuery;
			HxLOG_Print(" [NXMAN]   \t s_szUiQuery(%s).\n\n", s_pArgv[s_nArgc-1]);
			HxLOG_Print(" [NXMAN]   \t s_szNxPayload(%s).\n\n", s_szNxPayload);
			break;

#if defined(CONFIG_NETFLIX_BARKER_CHANNEL)
		case Source_EPG_GRID:						// 17,
		case Source_CHANNEL_NUMBER:					// 18,
		case Source_SUSPENDED_CHANNEL_SURF:			// 25
		case Source_SUSPENDED_CHANNEL_INFO_BAR:		// 26
			HxSTD_MemSet(szPayload, 0, 256);
			HxSTD_snprintf(szPayload, 256, NFLIX_BARKER_CH_PAYLOAD, s_stLaunchParam.ulChId
							, s_stLaunchParam.ulPrevChId, s_stLaunchParam.ulNextChId, s_pszCategory[s_stLaunchParam.enCategory]);

			HxSTD_snprintf(s_szUiQuery, MAX_NFLIX_UI_QUERY_STR-1, NFLIX_UI_QUERY_STR_SRC_PAYLOAD
							, s_stLaunchParam.enSrc, szPayload, s_stDialInfo.szAdditionalUrl);

			HxSTD_snprintf(s_szNxPayload, MAX_MFLIX_PAYLOAD_STR-1, NFLIX_NX_STR_SRC_PAYLOAD
							, s_stLaunchParam.enSrc, szPayload, s_stDialInfo.szAdditionalUrl);

			s_pArgv[s_nArgc++] = s_szUiQuery;
			HxLOG_Print(" [NXMAN]   \t s_szUiQuery(%s).\n\n", s_pArgv[s_nArgc-1]);
			HxLOG_Print(" [NXMAN]   \t s_szNxPayload(%s).\n\n", s_szNxPayload);

			break;
#endif

		case Source_iBANNER_AD:						// 9
		case Source_NETFLIX_PRE_APP:				// 27
		case Source_PRE_APP_ICON:					// 28
			HxSTD_snprintf(s_szUiQuery, MAX_NFLIX_UI_QUERY_STR-1, NFLIX_UI_QUERY_STR_DEEP_LINK, s_stLaunchParam.enSrc, s_stLaunchParam.szURL, s_stDialInfo.szAdditionalUrl);
			HxSTD_snprintf(s_szNxPayload, MAX_MFLIX_PAYLOAD_STR-1, NFLIX_NX_STR_DEEP_LINK_PAYLOAD, s_stLaunchParam.enSrc, s_stLaunchParam.szURL, s_stDialInfo.szAdditionalUrl);
			s_pArgv[s_nArgc++] = s_szUiQuery;
			HxLOG_Print(" [NXMAN]   \t s_szUiQuery(%s).\n\n", s_pArgv[s_nArgc-1]);
			HxLOG_Print(" [NXMAN]   \t s_szNxPayload(%s).\n\n", s_szNxPayload);
			break;

		case Source_DIAL:
			HxSTD_snprintf(s_szUiQuery, MAX_NFLIX_UI_QUERY_STR-1, NFLIX_UI_QUERY_STR_DIAL, s_stLaunchParam.enSrc, s_stDialInfo.szAdditionalUrl, s_stLaunchParam.szURL);
			HxSTD_snprintf(s_szNxPayload, MAX_MFLIX_PAYLOAD_STR-1, NFLIX_NX_STR_DIAL, s_stLaunchParam.enSrc, s_stDialInfo.szAdditionalUrl, s_stLaunchParam.szURL);
			s_pArgv[s_nArgc++] = s_szUiQuery;
			HxLOG_Print(" [NXMAN]   \t s_szUiQuery(%s).\n\n", s_pArgv[s_nArgc-1]);
			HxLOG_Print(" [NXMAN]   \t s_szNxPayload(%s).\n\n", s_szNxPayload);
			break;

		default:
			HxLOG_Warning(" [NXMAN]	  \t  Not support source_type(%d) yet..\n\n", s_stLaunchParam.enSrc);
			bRet = FALSE;
			break;
	}

	HxSEMT_Release(s_ulDialSem);

	return bRet;
}


#else

static HBOOL make_LaunchArgs(void)
{

	HBOOL	bRet = TRUE;
#if defined(CONFIG_NETFLIX_BARKER_CHANNEL)
	HCHAR   szPayload[256];//, *pszEncodedPayload = NULL;
#endif

	s_nArgc = 0;

	HxSTD_MemSet(s_pArgv, 0, sizeof(s_pArgv));
	HxSTD_MemSet(s_szUiQuery, 0, MAX_NFLIX_UI_QUERY_STR);

	s_pArgv[s_nArgc++] = NFLIX_APP;
	s_pArgv[s_nArgc++] = NFLIX_CREDENTIALS_STR;

	switch(s_stLaunchParam.enSrc)
	{
		case Source_NETFLIX_BUTTON: 				// 1
		case Source_DEDICATED_ON_SCREEN_ICON:		// 2
		case Source_ITEM_IN_APPLICATION_LIST:		// 3
		case Source_POWER_ON_FROM_NETFLIX_BUTTON:	// 19
		case Source_SUSPENDED_AT_POWER_ON:			// 22
		case Source_SUSPENDED_AFTER_APP_RESTART:	// 23
		case Source_OS_PRE_APP:						// 30
			HxSTD_snprintf(s_szUiQuery, MAX_NFLIX_UI_QUERY_STR-1, NFLIX_UI_QUERY_STR_NORMAL, s_stLaunchParam.enSrc, s_szAdditionalUrl);
			HxSTD_snprintf(s_szNxPayload, MAX_MFLIX_PAYLOAD_STR-1, NFLIX_NX_STR_SRC, s_stLaunchParam.enSrc, s_szAdditionalUrl);
			s_pArgv[s_nArgc++] = s_szUiQuery;
			HxLOG_Print(" [NXMAN]   \t s_szUiQuery(%s).\n\n", s_pArgv[s_nArgc-1]);
			HxLOG_Print(" [NXMAN]   \t s_szNxPayload(%s).\n\n", s_szNxPayload);
			break;

#if defined(CONFIG_NETFLIX_BARKER_CHANNEL)
		case Source_EPG_GRID:						// 17,
		case Source_CHANNEL_NUMBER:					// 18,
		case Source_SUSPENDED_CHANNEL_SURF:			// 25
		case Source_SUSPENDED_CHANNEL_INFO_BAR:		// 26
			HxSTD_MemSet(szPayload, 0, 256);
			HxSTD_snprintf(szPayload, 256, NFLIX_BARKER_CH_PAYLOAD, s_stLaunchParam.ulChId
							, s_stLaunchParam.ulPrevChId, s_stLaunchParam.ulNextChId, s_pszCategory[s_stLaunchParam.enCategory]);

			HxSTD_snprintf(s_szUiQuery, MAX_NFLIX_UI_QUERY_STR-1, NFLIX_UI_QUERY_STR_SRC_PAYLOAD
							, s_stLaunchParam.enSrc, szPayload, s_szAdditionalUrl);

			HxSTD_snprintf(s_szNxPayload, MAX_MFLIX_PAYLOAD_STR-1, NFLIX_NX_STR_SRC_PAYLOAD
							, s_stLaunchParam.enSrc, szPayload, s_szAdditionalUrl);

			s_pArgv[s_nArgc++] = s_szUiQuery;
			HxLOG_Print(" [NXMAN]   \t s_szUiQuery(%s).\n\n", s_pArgv[s_nArgc-1]);
			HxLOG_Print(" [NXMAN]   \t s_szNxPayload(%s).\n\n", s_szNxPayload);

			break;
#endif

		case Source_iBANNER_AD:						// 9
		case Source_NETFLIX_PRE_APP:				// 27
		case Source_PRE_APP_ICON:					// 28
			HxSTD_snprintf(s_szUiQuery, MAX_NFLIX_UI_QUERY_STR-1, NFLIX_UI_QUERY_STR_DEEP_LINK, s_stLaunchParam.enSrc, s_stLaunchParam.szURL, s_szAdditionalUrl);
			HxSTD_snprintf(s_szNxPayload, MAX_MFLIX_PAYLOAD_STR-1, NFLIX_NX_STR_DEEP_LINK_PAYLOAD, s_stLaunchParam.enSrc, s_stLaunchParam.szURL, s_szAdditionalUrl);
			s_pArgv[s_nArgc++] = s_szUiQuery;
			HxLOG_Print(" [NXMAN]   \t s_szUiQuery(%s).\n\n", s_pArgv[s_nArgc-1]);
			HxLOG_Print(" [NXMAN]   \t s_szNxPayload(%s).\n\n", s_szNxPayload);
			break;

		case Source_DIAL:
			HxSTD_snprintf(s_szUiQuery, MAX_NFLIX_UI_QUERY_STR-1, NFLIX_UI_QUERY_STR_DIAL, s_stLaunchParam.enSrc, s_szAdditionalUrl, s_stLaunchParam.szURL);
			HxSTD_snprintf(s_szNxPayload, MAX_MFLIX_PAYLOAD_STR-1, NFLIX_NX_STR_DIAL, s_stLaunchParam.enSrc, s_szAdditionalUrl, s_stLaunchParam.szURL);
			s_pArgv[s_nArgc++] = s_szUiQuery;
			HxLOG_Print(" [NXMAN]   \t s_szUiQuery(%s).\n\n", s_pArgv[s_nArgc-1]);
			HxLOG_Print(" [NXMAN]   \t s_szNxPayload(%s).\n\n", s_szNxPayload);
			break;

		default:
			HxLOG_Warning(" [NXMAN]	  \t  Not support source_type(%d) yet..\n\n", s_stLaunchParam.enSrc);
			bRet = FALSE;
			break;
	}

	return bRet;
}

#endif

/*
static	void	makeNetflixManagerWindow(void)
{
	DFBResult err;

	DFBWindowDescription	windowDesc;

	windowDesc.flags = (DFBWindowDescriptionFlags)(DWDESC_CAPS);
	windowDesc.caps  = (DFBWindowCapabilities)(DWCAPS_INPUTONLY);

	DFBCHECK(m_layer->CreateWindow(m_layer, &windowDesc, &m_window));
	DFBCHECK(m_window->CreateEventBuffer(m_window, &m_buffer));

	DFBCHECK(m_window->SetApplicationID(m_window, eAWINDOW_NETFLIXMANAGER));
}
*/

#if defined(CONFIG_DEBUG)
static HCHAR* __get_PamaStatusString(PxSTATUS_e			ePapiStatus)
{
	switch(ePapiStatus)
	{
		case ePAMA_STATUS_Operation: // 1
			return "Operation";
		case ePAMA_STATUS_Reboot: // 2
			return "Reboot";
		case ePAMA_STATUS_Shutdown: // 3
			return "Shutdown";
		case ePAMA_STATUS_RebootDone: //4 // 4
			return "RebootDone";
		case ePAMA_STATUS_ShutdownDone: // 5
			return "ShutdownDone";

		case ePAMA_STATUS_Undefined:
		case ePAMA_STATUS_Unknown:
		default:
			return "Unknown";
	}
}

static HCHAR* __get_NetflixStatusString(void)
{
	switch(s_eNflixState)
	{
		case NFLIX_NO_INSTANCE: // 1
			return "Nx-Clean";
		case NFLIX_SUSPENDED: // 2
			return "Nx-Suspended";
		case NFLIX_FOREGROUNDED: // 4
			return "Nx-Foregrounded";
		case NFLIX_BACKGROUNDED:
			return "Nx-Backgrounded"; // 8
		case NFLIX_NO_CHANGE:
			return "Nx-NoChange"; // 0x100

		default:
			return "Unknown";
	}
}

#endif



#if defined(CONFIG_PROD_FVP4000T)

static void __nxman_HommaStatusCallback(HBOOL bConnected)
{
	HxLOG_Print("[NXMAN] HOMMA Status(%d)\n", bConnected);

	HxSEMT_Get(s_ulDialSem);

	if(s_stDialInfo.bHommaActivated != bConnected)
	{
		if(bConnected)
		{
			s_stDialInfo.bHommaActivated = TRUE;
		}
		else
		{
			s_stDialInfo.bHommaActivated = FALSE;
			s_stDialInfo.bDialActivated = FALSE;
			HxSTD_MemSet(s_stDialInfo.szAdditionalUrl, 0, MAX_ENCORDED_URL_LEN);
		}
	}

	HxSEMT_Release(s_ulDialSem);

	return;
}

static void __nxman_DialEventCallback(HUINT32 evt, HUINT32 ulParam1, HUINT32 ulParam2)
{
	HxSEMT_Get(s_ulDialSem);

	switch ( evt )
	{
		case eDLNA_DIAL_Started:
			if(s_stDialInfo.bDialActivated == FALSE)
			{
				HxLOG_Print("[NXMAN] DIAL_Started\n");
				s_stDialInfo.bDialActivated = TRUE;
				HxSTD_MemSet(s_stDialInfo.szAdditionalUrl, 0, MAX_ENCORDED_URL_LEN);
				if(APK_DIAL_GetAdditionalDataUrl(s_stDialInfo.szAdditionalUrl, MAX_ENCORDED_URL_LEN, "Netflix") != ERR_OK)
				{
					HxLOG_Print(" [NXMAN] \t\t @@@@@ No AdditionalUrl...!!!\n\n");
				}
				else
				{
					HxLOG_Print(" [NXMAN] \t\t @@@@@ AdditionalUrl: [%s]\n\n", s_stDialInfo.szAdditionalUrl);
				}
			}
			break;

		case eDLNA_DIAL_Stopped:
			if(s_stDialInfo.bDialActivated == TRUE)
			{
				HxLOG_Print("[NXMAN] DIAL_Stopped\n");
				s_stDialInfo.bDialActivated = FALSE;
				HxSTD_MemSet(s_stDialInfo.szAdditionalUrl, 0, MAX_ENCORDED_URL_LEN);
			}
			break;

		default:
			break;
	}

	HxSEMT_Release(s_ulDialSem);

	return;
}

#endif

static	HERROR	__nxman_OnPowerStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	PxSTATUS_e			ePapiStatus;
	HUINT32				ulViewId = 0x0;
	static HUINT32		s_ulWaitCnt = 0;

	ePapiStatus = HxOBJECT_INT(apArgv[0]);

#if defined(CONFIG_DEBUG)
	HxLOG_Print(" [NXMAN]   ++ Papi_status(%s-%s) \n", __get_PamaStatusString(ePapiStatus), __get_NetflixStatusString());
#else
	HxLOG_Print(" [NXMAN]   ++ Papi_status(%s-%s) \n", (ePapiStatus == ePAMA_STATUS_Operation)?"On":"Off", (s_eNflixState==NFLIX_NO_INSTANCE)?"Nx-Clean":"Nx-Run or Suspend");
#endif

#if __SUPPORT_SLEEP_WAKE__
	switch(ePapiStatus)
	{
		case ePAMA_STATUS_Reboot:
			if(s_eCurPowerState != ePowerState_StandbyMode)
			{
				s_eCurPowerState = ePowerState_StandbyMode;
				s_bNetflixSleep = FALSE;

				HxLOG_Print(" [NXMAN]   Goingto Reboot \n");
				if(APK_POWER_WaitShutDown(_STB_SHUTDOWN_WAIT_TIME_) != ERR_OK)
				{
					HxLOG_Error(" [NXMAN]   @@@@@ Error > APK_POWER_WaitShutDown() is failed..!! \n");
				}

				HxLOG_Print(" [NXMAN]   @@@@@ Request stop netflix (reason : %d) \n", Reason_STANDBY_ON);
				RequestExit(Reason_STANDBY_ON);
				s_ulWaitCnt = 0;
			}
            else //if(_eCurPowerState == ePowerState_StandbyMode)
			{
				if((s_eNflixState != NFLIX_NO_INSTANCE) || (s_bOnChaingingState == TRUE))
				{
					HxLOG_Print(" [NXMAN]  @@@@  OnExiting, Wait more time(%d sec) \n", _STB_SHUTDOWN_WAIT_TIME_);
					if(APK_POWER_WaitShutDown(_STB_SHUTDOWN_WAIT_TIME_) != ERR_OK)
					{
						HxLOG_Error(" [NXMAN]   @@@@@ Error > APK_POWER_WaitShutDown() is failed..!! \n");
					}
					s_ulWaitCnt++;

					if(s_ulWaitCnt >= 6)
					{
						HxLOG_Critical(" [NXMAN]  @@@@  OnExiting timeout !!!! => Kill Netflix \n");
						HxSEMT_Release(s_ulNxManSem);
						(void)AM_killApplication(FALSE);
					}
				}
			}
			break;

		case ePAMA_STATUS_Shutdown:
			if(s_eCurPowerState != ePowerState_StandbyMode)
			{
				HxLOG_Print(" [NXMAN]   Goingto Shutdown \n");
				s_eCurPowerState = ePowerState_StandbyMode;

                if(APK_POWER_WaitShutDown(_STB_SHUTDOWN_WAIT_TIME_) != ERR_OK)
				{
					HxLOG_Error(" [NXMAN]   @@@@@ Error > APK_POWER_WaitShutDown() is failed..!! \n");
				}

				if((s_eNflixState == NFLIX_FOREGROUNDED) && (s_bNetflixSleep != TRUE))
				{
#if defined(CONFIG_DEBUG)
					HxLOG_Print(" [NXMAN]  papi(%s), netflixpb(%s)\n", __get_PamaStatusString(ePapiStatus), __nxman_getPlaybackStateToStr(s_eNetflixPlaybackState));
#endif
					if ((ePAMA_STATUS_Shutdown == ePapiStatus) && (s_eNetflixPlaybackState > NFLIX_NOTIFY_PLAYBACK_STOPPING))
					{
						/* Workaround for fixing redmine #110888: Netflix launch 상태에서 standby 진입 후 다시 operation mode로 전환 시 모든 채널의 A/V가 출력 되지 않는 문제점 */
						if (APK_MEDIA_PLAY_GetMainViewId(&ulViewId) == ERR_OK)
						{
#if defined(CONFIG_DEBUG)
							HxLOG_Print(" [NXMAN]  Call MEDIA_PLAY_Stop() in advance by papi(%s), netflixpb(%s)\n", __get_PamaStatusString(ePapiStatus), __nxman_getPlaybackStateToStr(s_eNetflixPlaybackState));
#endif
							(void)APK_MEDIA_PLAY_Stop(ulViewId);
							HLIB_STD_TaskSleep(20); /* TODO: Check this waitTime for oapi processing; 굳이 필요할까? */
						}
					}

					HxLOG_Print(" [NXMAN]   ==> sleep netflix \n");
					//do_ApplicationManagerController(Source_NONE, CMD_CNT_POWER_SLEEP);

#if !defined(CONFIG_OP_TDC)
					s_stExitState = NFLIX_NOTIFY_EXIT_STANDBY;
#endif

					HLIB_QUEUE_Push(s_pAMQueue, (void*)CMD_CNT_POWER_SLEEP);
				}
				else if(s_eNflixState == NFLIX_NO_INSTANCE)
				{
					s_bNetActivated = FALSE;
					s_bTimeSetted = FALSE;
					s_bSuspendStarted = FALSE; /* ... */
					s_bDiskStoreReady = FALSE;
					init_DialInfo();
				}
				else // NFLIX_SUSPENDED
				{
					// No action...
				}
			}
			break;

        case ePAMA_STATUS_ShutdownDone:
            break;

		case ePAMA_STATUS_Operation:
			if(s_eCurPowerState != ePowerState_OperationMode)
			{
				HxLOG_Print(" [NXMAN]   Goingto Operate\n");
				s_eCurPowerState = ePowerState_OperationMode;

#if !__USE_CALLBACK_FOR_DIAL_CHECKING__
				s_ulDialCheckStartTime = VK_TIMER_GetSystemTick();
#endif
			}
			break;

		default:
				break;
	}
#else
	switch(ePapiStatus)
	{
		case ePAMA_STATUS_Reboot:
		case ePAMA_STATUS_Shutdown:
			if(s_eCurPowerState != ePowerState_StandbyMode)
			{
				s_eCurPowerState = ePowerState_StandbyMode;

				if((s_eNflixState != NFLIX_NO_INSTANCE) || (s_bOnChaingingState == TRUE))
				{
					HxLOG_Print(" [NXMAN]   Goingto Shutdown/Reboot \n");
					if(APK_POWER_WaitShutDown(_STB_SHUTDOWN_WAIT_TIME_) != ERR_OK)
					{
						HxLOG_Error(" [NXMAN]   @@@@@ Error > APK_POWER_WaitShutDown() is failed..!! \n");
					}

#if defined(CONFIG_DEBUG)
					HxLOG_Print(" [NXMAN]  papi(%s), netflixpb(%s)\n", __get_PamaStatusString(ePapiStatus), __nxman_getPlaybackStateToStr(s_eNetflixPlaybackState));
#endif
					if ((ePAMA_STATUS_Shutdown == ePapiStatus) && (s_eNetflixPlaybackState > NFLIX_NOTIFY_PLAYBACK_STOPPING))
					{
						/* Workaround for fixing redmine #110888: Netflix launch 상태에서 standby 진입 후 다시 operation mode로 전환 시 모든 채널의 A/V가 출력 되지 않는 문제점 */
						if (APK_MEDIA_PLAY_GetMainViewId(&ulViewId) == ERR_OK)
						{
#if defined(CONFIG_DEBUG)
							HxLOG_Print(" [NXMAN]  Call MEDIA_PLAY_Stop() in advance by papi(%s), netflixpb(%s)\n", __get_PamaStatusString(ePapiStatus), __nxman_getPlaybackStateToStr(s_eNetflixPlaybackState));
#endif
							(void)APK_MEDIA_PLAY_Stop(ulViewId);
							HLIB_STD_TaskSleep(20); /* TODO: Check this waitTime for oapi processing; 굳이 필요할까? */
						}
					}

					HxLOG_Print(" [NXMAN]   @@@@@ Request stop netflix (reason : %d) \n", Reason_STANDBY_ON);
					RequestExit(Reason_STANDBY_ON);
					s_ulWaitCnt = 0;
				}
			}
			else //if(_eCurPowerState == ePowerState_StandbyMode)
			{
				if((s_eNflixState != NFLIX_NO_INSTANCE) || (s_bOnChaingingState == TRUE))
				{
					HxLOG_Print(" [NXMAN]  @@@@  OnExiting(%d), Wait more time(%d sec) \n", s_bOnChaingingState, _STB_SHUTDOWN_WAIT_TIME_);
					if(APK_POWER_WaitShutDown(_STB_SHUTDOWN_WAIT_TIME_) != ERR_OK)
					{
						HxLOG_Error(" [NXMAN]   @@@@@ Error > APK_POWER_WaitShutDown() is failed..!! \n");
					}
					s_ulWaitCnt++;

					if(s_ulWaitCnt >= 6)
					{
						HxLOG_Critical(" [NXMAN]  @@@@  OnExiting timeout !!!! => Kill Netflix \n");
						HxSEMT_Release(s_ulNxManSem);
						(void)AM_killApplication(FALSE);
					}
				}
			}
			break;

		case ePAMA_STATUS_ShutdownDone:
			break;

		case ePAMA_STATUS_Operation:
			s_eCurPowerState = ePowerState_OperationMode;

			if(s_eNflixState == NFLIX_NO_INSTANCE)
			{
				s_bNetActivated = FALSE;
				s_bTimeSetted = FALSE;
				s_bSuspendStarted = FALSE; /* ... */
				s_bDiskStoreReady = FALSE;
				init_DialInfo();
			}

			HxLOG_Print(" [NXMAN]   Goingto Operate \n");
			break;

		default:
				break;
	}
#endif

	HxLOG_Print(" [NXMAN]   Current Power state [%d]\n", s_eCurPowerState);

	return ERR_OK;
}



static HERROR __nxman_onStartNetflixAppl(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	Launch_Param_t  *pLauchParam = NULL;
	if( nArgc < 1 )
	{
		return -1;
	}

	pLauchParam = (Launch_Param_t*)HxOBJECT_BIN_DATA(apArgv[0]);


	HxLOG_Print(" [NXMAN]  [eHSIG_StartNetflixAppl] Request start netflix by signal:onStartNetflixAppl \n");

	HxSTD_MemCopy(&s_stLaunchParam, pLauchParam, sizeof(Launch_Param_t));

#ifdef	CONFIG_DEBUG
	HxLOG_Print(" [NXMAN]  \t enSrc : %d\n", s_stLaunchParam.enSrc);
	HxLOG_Print(" [NXMAN]   \t  enCategory : %d\n", s_stLaunchParam.enCategory);
	HxLOG_Print(" [NXMAN]   \t  ulTrackId : %d\n", s_stLaunchParam.ulTrackId);
	HxLOG_Print(" [NXMAN]   \t  ulChId : %d\n", s_stLaunchParam.ulChId);
	HxLOG_Print(" [NXMAN]   \t   ulPrevChId : %d\n", s_stLaunchParam.ulPrevChId);
	HxLOG_Print(" [NXMAN]   \t   ulNextChId : %d\n", s_stLaunchParam.ulNextChId);
	HxLOG_Print(" [NXMAN]   \t  pucURL : %s\n", s_stLaunchParam.szURL);
	HxLOG_Print(" [NXMAN]   \t   pucTrackURL : %s\n\n", s_stLaunchParam.szTrackURL);
#endif

	if((s_stLaunchParam.enSrc == Source_SUSPENDED_AT_POWER_ON) && (s_bFirstBoot == TRUE))
	{
		s_bFirstBoot = FALSE;
	}
	else
	{
		if(make_LaunchArgs() == TRUE)
		{
			if(s_bAMRunning == false)
			{
				// Start netflix
				HxLOG_Print(" [NXMAN]  [AMRunning:%d] --> do_ApplicationManager()\n", s_bAMRunning);
				do_ApplicationManager();
				HLIB_STD_TaskSleep(100);
			}

			s_bResetVideoResolution = FALSE;
			init_Overlay();
			do_ApplicationManagerController(s_stLaunchParam.enSrc, Reason_NONE);
		}
		else
		{
			HxLOG_Error(" [NXMAN]   make_LaunchArgs() is failed..!!\n");
		}
	}
	return ERR_OK;
}

static HERROR	__nxman_onStopNetflixAppl(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	NFLIX_Exit_Reason_Type_t exitType;

	if( nArgc < 1 )
	{
		return -1;
	}

	exitType = HxOBJECT_INT(apArgv[0]);

	HxLOG_Print(" [NXMAN]  [eHSIG_StopNetflixAppl] Request stop netflix (reason : %d) by signal:onStopNetflixAppl\n", (HUINT32)exitType);
	init_Overlay();
	do_ApplicationManagerController(Source_NONE, exitType);

	return ERR_OK;
}

#if __PRIVACY_POLICY_AUTH__ 
#define NETFLIX_PP_DISAGREE		"disagree"
static	HERROR	__nxman_onPPVerChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*szPPVer = NULL;
	HUINT32	ulPPVerLen = 0;
	if ( nArgc != 1)
	{
		return ERR_FAIL;
	}
	szPPVer = (char *)HxOBJECT_STR(apArgv[0]);
	if(szPPVer == NULL)
	{
		return ERR_FAIL;
	}
	ulPPVerLen=HxSTD_StrLen(szPPVer);
	HxLOG_Debug("szPPVer = %s(%d)\n",szPPVer,ulPPVerLen);
	if(ulPPVerLen != 0 && (HxSTD_StrNCmp(szPPVer, NETFLIX_PP_DISAGREE, ulPPVerLen) == 0))
	{
		if(logout_TVPortal() == TRUE)
		{
			HxLOG_Error(" [NXMAN]	 @@@@ Portal Logout done... \n");
		}
		else
		{
			HxLOG_Error(" [NXMAN]	 @@@@ Portal Logout fail or not login before... \n");
		}
	}
	return ERR_OK;
}
#endif

static char *__nxman_getPlaybackStateToStr(NFLIX_NotifyPlaybackState_t state)
{
	switch(state) {
	ENUM_TO_STR(NFLIX_NOTIFY_PLAYBACK_NA);
	ENUM_TO_STR(NFLIX_NOTIFY_PLAYBACK_STOPPED);
	ENUM_TO_STR(NFLIX_NOTIFY_PLAYBACK_STOPPING);
	ENUM_TO_STR(NFLIX_NOTIFY_PLAYBACK_STARTING);
	ENUM_TO_STR(NFLIX_NOTIFY_PLAYBACK_PAUSED);
	ENUM_TO_STR(NFLIX_NOTIFY_PLAYBACK_PLAYING);
	default:
		break;
	}
	return "N/A";
}

static HERROR __nxman_onNetflixPlaybackChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	/* Fixed redmine #110888: Netflix launch 상태에서 standby 진입 후 다시 operation mode로 전환 시 모든 채널의 A/V가 출력 되지 않는 문제점 */
	NFLIX_NotifyPlaybackState_t playbackState = NFLIX_NOTIFY_PLAYBACK_NA;

	if( nArgc < 1 )
	{
		return -1;
	}
	playbackState = HxOBJECT_INT(apArgv[0]);

	switch(playbackState){
		case NFLIX_NOTIFY_PLAYBACK_NA		: /* eNXLI_NOTIFY_PLAYBACK_NA		*/
		case NFLIX_NOTIFY_PLAYBACK_STOPPED	: /* eNXLI_NOTIFY_PLAYBACK_STOPPED	*/
		case NFLIX_NOTIFY_PLAYBACK_STOPPING	: /* eNXLI_NOTIFY_PLAYBACK_STOPPING	*/
		case NFLIX_NOTIFY_PLAYBACK_STARTING	: /* eNXLI_NOTIFY_PLAYBACK_STARTING	*/
		case NFLIX_NOTIFY_PLAYBACK_PAUSED	: /* eNXLI_NOTIFY_PLAYBACK_PAUSED	*/
		case NFLIX_NOTIFY_PLAYBACK_PLAYING	: /* eNXLI_NOTIFY_PLAYBACK_PLAYED	*/
			/* TODO: sema lock */
			s_eNetflixPlaybackState = playbackState;
			break;
		default :
			break;
	}
   	HxLOG_Print(" [NXMAN]  [signal:onNetflixPlaybackChanged] == [%d,%s]\n", (HUINT32)playbackState, __nxman_getPlaybackStateToStr(playbackState));
	return ERR_OK;
}

static HERROR	__nxman_onNotifyOverlayState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	NFLIX_Overlay_Ctrl_t   eCtrlOverlay;

	HUINT32 ulDiff1 = 0x0, ulDiff2 = 0x0;
	/*
	HUINT32 ulDiff = 0x0;
	HUINT32 ulCurTime = 0x0;
	HBOOL checkLastSuspend = TRUE;
	HBOOL skipActionBySuspendCondition = FALSE;
	*/

	if( nArgc < 1 )
	{
		return -1;
	}

	eCtrlOverlay = (NFLIX_Overlay_Ctrl_t)HxOBJECT_INT(apArgv[0]);

	HxLOG_Print(" [NXMAN]  [NotifyOverlay] ++++ prev_overlay(%d), s_eNflixState(%d)\n", s_eOverlayState, s_eNflixState);
	{
		s_ulLastOverlayCloseStartTime = VK_TIMER_GetSystemTick();

		/* Check the time interval with last overlay action */
		ulDiff1 = ulDiff2 = 0x0;
		if (s_ulLastOverlayPopupEndTime) ulDiff1 = (s_ulLastOverlayCloseStartTime - s_ulLastOverlayPopupEndTime);
		if (s_ulLastOverlayPopupStartTime) ulDiff2 = (s_ulLastOverlayCloseStartTime - s_ulLastOverlayPopupStartTime);

		if ((ulDiff1 && (ulDiff1 < _NFLIX_OVERLAY_WITH_LAST_ACTION_GAP))
			|| (ulDiff2  && (ulDiff2 < _NFLIX_OVERLAY_WITH_LAST_ACTION_GAP)))
		{
			HxLOG_Error(" [NXMAN]   [NotifyOverlay] Error> too short time interval(diff1:%d, diff2:%d)\n", ulDiff1, ulDiff2);
		}

		/* With the HOME menu overlay close() by webapp, do next pause/resume playback action */
		HxLOG_Print(" [NXMAN]  [NotifyOverlay] Request Control Overlay(%d) \n", eCtrlOverlay);
		do_controlOverlay(eCtrlOverlay);
		s_ulLastOverlayEventTime = s_ulLastOverlayCloseEndTime = VK_TIMER_GetSystemTick();
	}
	HxLOG_Print(" [NXMAN]  [NotifyOverlay] ---- overlay result(%d), s_eNflixState(%d)\n", s_eOverlayState, s_eNflixState);

	return ERR_OK;
}

#if 0
static HBOOL	isDFBKeyEventOccurred(DFBEvent *event)
{
	if (m_buffer->HasEvent(m_buffer) == DFB_OK)
	{
		if (m_buffer->GetEvent(m_buffer, event) == DFB_OK)
		{
			if ((event->clazz == DFEC_WINDOW) && (event->window.type == DWET_KEYDOWN))
				return TRUE;
		}
	}

	return FALSE;
}
#endif

static void start_up_NetflixApp(void)
{
	HxLOG_Print(" [NXMAN]   start_up_NetflixApp()++\n");

	if(s_bProvisioningDone == FALSE)
	{
#if __USB_PROVISIONING__
		s_bTryProvisioning = FALSE;
#endif

		if(s_bTryProvisioning == TRUE)
		{
			s_bSuspendStarted = TRUE;
		}

		HxLOG_Warning(" [NXMAN]   No ESN & KEY =>> start_up_NetflixApp()--\n");
		return;
	}

	if(s_bAMRunning == false)
	{
		// Start netflix
		HxLOG_Print(" [NXMAN]  --> do_ApplicationManager()\n");
		do_ApplicationManager();
		HLIB_STD_TaskSleep(100);
	}

	HxSTD_MemSet(&s_stLaunchParam, 0, sizeof(Launch_Param_t));

	s_stLaunchParam.enSrc = Source_SUSPENDED_AT_POWER_ON;
	if(make_LaunchArgs() == TRUE)
	{
		do_ApplicationManagerController(s_stLaunchParam.enSrc, Reason_NONE);
		s_bSuspendStarted = TRUE;
	}
	else
	{
		HxLOG_Error(" [NXMAN]   make_LaunchArgs() is failed..!!\n");
	}

	HxLOG_Print(" [NXMAN]   start_up_NetflixApp()--\n");

	return;
}


static void	RequestExit(NFLIX_Exit_Reason_Type_t exitType)
{
	HxLOG_Print(" [NXMAN]   RequestExit(%d)++\n", (HUINT32)exitType);

	s_bResetVideoResolution = TRUE;

	do_ApplicationManagerController(Source_NONE, exitType);


	/* When going out to LIVE from the netflix under PIG videoWindow size, ...
	 * LIVE has small videoWindow size without returning to original full screen size
	 * Sometimes too slow, this cause videoResize() error of live playback when switching to the LiveTV screen
	 * So, move this here to do videoResize() at the time of returning DIKS_EXIT
	*/
	if(s_bResetVideoResolution == TRUE)
	{
		HERROR  hErr = ERR_OK;
		HUINT32 ulViewId;

		(void)APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
		hErr = APK_MEDIA_PLAY_ResetVideoSize(ulViewId);
		if(hErr != ERR_OK)
		{
			HxLOG_Error(" [NXMAN]   @@@@@  Failed to ResetVideoSize...!!!\n\n");
		}
		s_bResetVideoResolution = FALSE;
	}

	HxLOG_Print(" [NXMAN]  RequestExit()--\n");
}


static void	AM_keyevent_handler(void *arg)
{
	HInputEvent_t	hKeyEvent;

	while (1)
	{
		/* hapi key process  */
		if (HAPI_GetInputEvent(eAWINDOW_NETFLIXMANAGER, &hKeyEvent, 0) == TRUE)
		{
			//HxLOG_Print(" [NXMAN]  Get Ke Event [%s-%d], s_eNflixState(%d)\n", (hKeyEvent.type==DIET_KEYPRESS)?"Press":"Release", hKeyEvent.key_symbol, s_eNflixState);

			if(hKeyEvent.type==DIET_KEYPRESS)
			{
				if(s_eNflixState != NFLIX_FOREGROUNDED)
				{

#if 0 //for test (provisioning, splunk reporting, etc..)
					if(	hKeyEvent.key_symbol == DIKS_YELLOW)
					{
						if(hKeyEvent.type == DIET_KEYPRESS)
						{
							/*
							//add_Netflix_RunStatus_log(STATUS_LOG_START);

							#if __DO_IN_FIELD_PROVISIONING__
								HxLOG_Print(" [NXMAN]   do test-provisioning.. \n");
								get_credentials();
							#endif

							#if __TESTING_SPLUNK_REPORT__
								HxLOG_Print(" [NXMAN]   do test-splunk reporting.. \n");
								add_Netflix_RunStatus_log(STATUS_LOG_START);
							#endif
							*/

							HxSTD_MemSet(&s_stLaunchParam, 0, sizeof(Launch_Param_t));

							s_stLaunchParam.enSrc = Source_SUSPENDED_AT_POWER_ON;
							if(make_LaunchArgs() == TRUE)
							{
								AM_doStartSuspend();
							}
							else
							{
								HxLOG_Error(" [NXMAN]   make_LaunchArgs() is failed..!!\n");
							}
						}
						HxLOG_Print(" [NXMAN]  Consume[Press-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
						HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_CONSUMED, &hKeyEvent);
						s_eKeyInputRetType = eHINPUTRET_CONSUMED;
					}
					else
#endif
					{
						if((s_bOnChaingingState == TRUE) && (s_eNetflixSuspendStatus >= NFLIX_SunpendStatus_Completed)) // when start/resuming to foreground, ignore key-input(ex: ch+, ch+, ch+..)
						{
							HxLOG_Print(" [NXMAN]  OnChaingingState => Consume[Press-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
							HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_CONSUMED, &hKeyEvent);
							s_eKeyInputRetType = eHINPUTRET_CONSUMED;
						}
						else
						{
							HxLOG_Print(" [NXMAN]  Bypass[Press-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
							HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_PASS, &hKeyEvent);
							s_eKeyInputRetType = eHINPUTRET_PASS;
						}
					}
				}
				else if(s_eOverlayState > NFLIX_OVERLAY_POPUP_WITHOUT_KEY)
				{
					if(s_bOnChaingingState == TRUE)
					{
						if(s_eNetflixSuspendStatus >= NFLIX_SunpendStatus_Completed) // resuming
						{
							HxLOG_Print(" [NXMAN]  OnChaingingState => Consume[Press-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
							HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_CONSUMED, &hKeyEvent);
							s_eKeyInputRetType = eHINPUTRET_CONSUMED;
						}
						else
						{
							HxLOG_Print(" [NXMAN]  Bypass[Press-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
							HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_PASS, &hKeyEvent);
							s_eKeyInputRetType = eHINPUTRET_PASS;
						}
					}
					else
					{
						switch(hKeyEvent.key_symbol)
						{
#if !defined(CONFIG_OP_TDC)
							case DIKS_EXIT:
								/* Prevent for LIVE black-out when exiting Netflix or channel up/down */
								(void)HAPI_BroadcastSignal("signal:onNetflixStateChanged", 0, "i", NFLIX_NOTIFY_TRYTO_EXIT);

								HxLOG_Print(" [NXMAN]   [DIKS_EXIT] Goto Suspend-mode via Key(%d)...\n", hKeyEvent.key_symbol);
								RequestExit(Reason_USER_Nav);
								s_stExitState = NFLIX_NOTIFY_EXIT_NORMAL;

								HxLOG_Print(" [NXMAN]  Consume[Press-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
								HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_CONSUMED, &hKeyEvent);
								s_eKeyInputRetType = eHINPUTRET_CONSUMED;
								break;
#endif

							case DIKS_CHANNEL_UP:
							case DIKS_CHANNEL_DOWN:
#if !defined(CONFIG_NETFLIX_BARKER_CHANNEL)
								HxLOG_Print(" [NXMAN]   Consume[Press-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
								HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_CONSUMED, &hKeyEvent);
								s_eKeyInputRetType = eHINPUTRET_CONSUMED;
								break;
#endif

#if !defined(CONFIG_OP_TDC)
                            case DIKS_HOME:
							case DIKS_MENU: // home key in the remote app
#endif
							case DIKS_EPG:
							case DIKS_INTERNET: // TV APPS
							case DIKS_PLAYER: // Min side (TDC YSR-2000)
								//s_eSuspendPbState = s_ePbState;
								/* Prevent for LIVE black-out when exiting Netflix or channel up/down */
								(void)HAPI_BroadcastSignal("signal:onNetflixStateChanged", 0, "i", NFLIX_NOTIFY_TRYTO_EXIT);

								HxLOG_Print(" [NXMAN]   Goto Suspend-mode via Key(%d)...\n", hKeyEvent.key_symbol);
								RequestExit(Reason_USER_Nav);

#if defined(CONFIG_NETFLIX_BARKER_CHANNEL)
								switch(hKeyEvent.key_symbol)
								{
									case DIKS_CHANNEL_UP:
										s_stExitState = NFLIX_NOTIFY_EXIT_CH_UP;
										break;
									case DIKS_CHANNEL_DOWN:
										s_stExitState = NFLIX_NOTIFY_EXIT_CH_DOWN;
										break;
#if defined(CONFIG_OP_TDC)
                                    case DIKS_EPG:
                                        s_stExitState = NFLIX_NOTIFY_EXIT_EPG;
										break;
        							case DIKS_PLAYER: // Min side (TDC YSR-2000)
        							    s_stExitState = NFLIX_NOTIFY_EXIT_MY_PAGE;
										break;
#endif
									default:
										s_stExitState = NFLIX_NOTIFY_EXIT_NORMAL;
										break;
								}
#else
								s_stExitState = NFLIX_NOTIFY_EXIT_NORMAL;
#endif

								HxLOG_Print(" [NXMAN]  Bypass[Press-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
								HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_PASS, &hKeyEvent);
								s_eKeyInputRetType = eHINPUTRET_PASS;
								break;

							default:
								//Key will be used in other application
								//HxLOG_Print(" [NXMAN]   Bypass Key(%d)...\n", hKeyEvent.key_symbol);
								HxLOG_Print(" [NXMAN]  Bypass[Press-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
								HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_PASS, &hKeyEvent);
								s_eKeyInputRetType = eHINPUTRET_PASS;
								break;
						}
					}
				}
				else if(s_eOverlayState == NFLIX_OVERLAY_POPUP_WITH_KEY)
				{
					HxLOG_Print(" [NXMAN]  Overlay Popup with key => Bypass[Press-%d], s_eNflixState(%d), overlay(%d)\n"
						, hKeyEvent.key_symbol, s_eNflixState, s_eOverlayState);
					HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_PASS, &hKeyEvent);
					s_eKeyInputRetType = eHINPUTRET_PASS;
				}
				else
				{
					HxLOG_Print(" [NXMAN]  Overlay Popup without key => Consume[Press-%d], s_eNflixState(%d), overlay(%d)\n"
						, hKeyEvent.key_symbol, s_eNflixState, s_eOverlayState);
					HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, eHINPUTRET_CONSUMED, &hKeyEvent);
					s_eKeyInputRetType = eHINPUTRET_CONSUMED;
				}
			}
			else // key release
			{
				if(s_eKeyInputRetType == eHINPUTRET_NONE)
				{
					HxLOG_Warning(" [NXMAN]  Invalid event[Release-%d], s_eNflixState(%d)\n", hKeyEvent.key_symbol, s_eNflixState);
				}
				else
				{
					HxLOG_Print(" [NXMAN]  %s[Release-%d], s_eNflixState(%d)\n", (s_eKeyInputRetType==eHINPUTRET_PASS)?"Bypass":"Consume", hKeyEvent.key_symbol, s_eNflixState);
					HAPI_ReturnKeyEvent(eAWINDOW_NETFLIXMANAGER, s_eKeyInputRetType, &hKeyEvent);
					s_eKeyInputRetType = eHINPUTRET_NONE;
				}
			}

		}

		HLIB_STD_TaskSleep(10);
	}
}


static	void	runNetflixManager(void)
{
	HBOOL		bRunning = TRUE;
	/*
	HUINT32				ulDiff1 = 0x0, ulDiff2 = 0x0, ulDiff3 = 0x0, ulDiff4 = 0x0;
	HInputRetType_e		eKeyRet = eHINPUTRET_NONE;
	HBOOL				bNormalOverlay = TRUE;
	*/
	/*
	HUINT32 ulDiff = 0x0;
	HUINT32 ulCurTime = 0x0;
	HBOOL checkLastSuspend = TRUE;
	HBOOL skipActionBySuspendCondition = FALSE;
	*/
#if __USB_PROVISIONING__
	HUINT32 ulTryCnt = 0;
#endif

	notify_activated(TRUE);

#if __USE_CALLBACK_FOR_DIAL_CHECKING__
	check_DIAL();
#else
	s_ulDialCheckStartTime = VK_TIMER_GetSystemTick();
#endif	

 	while (bRunning)
	{
		/* appkit event dispatch */
		(void)APK_EVENT_Dispatch();

		if(s_bFactoryReset == TRUE)
		{
			HLIB_STD_TaskSleep(10);
			continue;
		}

		if(s_eCurPowerState != ePowerState_OperationMode)
		{
			//HxLOG_Print(" [NXMAN]  Not operationMode...!!\n");
			if(APK_POWER_GoingToOperate() == TRUE)
			{
				s_eCurPowerState = ePowerState_OperationMode;
			}
			HLIB_STD_TaskSleep(10);
			continue;
		}

		if(s_bNetActivated == FALSE)
		{
			(void)check_network();
		}
		else
		{
			if(s_bTimeSetted == FALSE)
			{
				(void)check_time();
			}
			else
			{
				if(s_stSysInfo.bValid == FALSE)
				{
					HxLOG_Print(" [NXMAN]   ####Do Init_SysInfo().. \n");
					Init_SysInfo();
					HLIB_STD_TaskSleep(10);
				}
#if __DO_IN_FIELD_PROVISIONING__
				else if(s_bTryProvisioning == FALSE)
				{
					HxLOG_Print(" [NXMAN]   ####Do In-field provisioning.. \n");
					do_provisioning();
					s_bTryProvisioning = TRUE;
					HLIB_STD_TaskSleep(10);
					continue;
				}
#endif
			}
		}

#if __USB_PROVISIONING__
		if(s_bTryProvisioning == FALSE)
		{
			struct stat sts;

			if(stat(NFLIX_USB_CREDENTIALS, &sts) == 0)
			{
				HxLOG_Print(" [NXMAN]   ####Do USB Provisioning.. \n");
				do_provisioning();
				s_bTryProvisioning = TRUE;
			}
			else
			{
				ulTryCnt++;
				HxLOG_Print(" [NXMAN]  ## ");

				if(ulTryCnt > 300)
				{
					s_bTryProvisioning = TRUE;
					ulTryCnt = 0;
				}
			}

			HLIB_STD_TaskSleep(100);
			continue;
		}
#endif

		/* NRDP 4.2 change : You must start Netflix in suspend mode regardless of connectivity */
		if(s_bDiskStoreReady == FALSE)
		{
			HxLOG_Print(" [NXMAN]   ####Do check_diskstore().. \n");
			check_diskstore();
			HLIB_STD_TaskSleep(10);
			continue;
		}

#if __USE_CALLBACK_FOR_DIAL_CHECKING__

		if((s_bSuspendStarted == FALSE) && (s_bFirstBoot == FALSE))
		{
			HxLOG_Print(" [NXMAN]   ####Do start_up_NetflixApp().. \n");
			start_up_NetflixApp();          
		}
#if __SUPPORT_SLEEP_WAKE__        
		else if(s_bNetflixSleep == TRUE)
		{
			check_DIAL();

			HxLOG_Print(" [NXMAN]   #### Wake NetflixApp.. \n");
			HLIB_QUEUE_Push(s_pAMQueue, (void*)CMD_CNT_POWER_WAKE);
			s_bNetflixSleep = FALSE;
		}
#endif

#else
		check_DIAL();

		if((s_bSuspendStarted == FALSE) && (s_bFirstBoot == FALSE))
		{
			if((s_bDialActivated == TRUE) || (s_ulDialCheckCount >= NFLIX_DIAL_CHECK_OVERCOUNT))
			{
				HxLOG_Print(" [NXMAN]   ####Do start_up_NetflixApp().. \n");
				start_up_NetflixApp();
			}
		}

#if __SUPPORT_SLEEP_WAKE__
		else if(s_bNetflixSleep == TRUE)
		{
			if((s_bDialActivated == TRUE) || (s_ulDialCheckCount >= NFLIX_DIAL_CHECK_OVERCOUNT))
			{
				HxLOG_Print(" [NXMAN]   #### Wake NetflixApp.. \n");
				HLIB_QUEUE_Push(s_pAMQueue, (void*)CMD_CNT_POWER_WAKE);
				s_bNetflixSleep = FALSE;
			}
		}
#endif

#endif

#if		__CLEAR_DUPLICATE_PROCESS__

		if((s_bSuspendStarted == TRUE))
		{
			s_ulDuplicateCheckCount++;
			if(s_ulDuplicateCheckCount >= NFLIX_DUPLICATE_CHECKING_PERIOD)
			{
				s_ulDuplicateCheckCount = 0;
				if(GetCount_netflix() > 1)
				{
					HxLOG_Print(" [NXMAN]   ####Killall Netflix ...!!! \n");
					VK_SYSTEM_Command("killall netflix");
					HLIB_STD_TaskSleep(45000);
					s_bShouldRestart = TRUE;
					s_bRestartSuspended = TRUE;
				}
			}
		}

#endif

		HLIB_STD_TaskSleep(10);
	}

	HxLOG_Print(" [NXMAN]   Exit NxMAN....!!! \n");
}



/**************************************************************
	Application(Netflix) Manager Common
 **************************************************************/
#define	____APPLICATION_MANAGER_COMMON____

static const char * ApplicationSocketPath(void)
{
	const char * c = "/tmp/nfappman";
	return c;
}


/**************************************************************
	Application(Netflix) Manager
 **************************************************************/
 #define	____APPLICATION_MANAGER____

static int AM_doStartSuspend(void)
{
	int		rc;

	HxLOG_Print(" [NXMAN]   AM_doStartSuspend++\n");

	s_bOnChaingingState = TRUE;
	s_eNetflixSuspendStatus = NFLIX_SunpendStatus_Ongoing;

	rc = AM_startApplication(s_nArgc, s_pArgv, true);

	HxLOG_Print(" [NXMAN]   AM_doStartSuspend--\n");

	return rc;
}

static int AM_doStartRunning(void) /* start_foreground */
{
	int rc;

	HxLOG_Print(" [NXMAN]   AM_doStartRunning++\n");

	s_bOnChaingingState = TRUE;

	rc = AM_startApplication(s_nArgc, s_pArgv, false);

	HxLOG_Print(" [NXMAN]   AM_doStartRunning--\n");

	return rc;
}

static int AM_doResume(void)
{
	char		rc = 0;
	HBOOL		bRestart = FALSE;
#if __NFLIX_WAITING_MODE_CHANGING_COMPLETE__
	HUINT32		ulStartTime, ulCurTime, ulDiff;

	ulStartTime = VK_TIMER_GetSystemTick();
#endif

	HxLOG_Print(" [NXMAN]   AM_doResume++\n");

	//if (s_bNetflixSuspending == TRUE) s_bNetflixSuspending = FALSE;
	//if (s_bNetflixSuspendCompleted == TRUE) s_bNetflixSuspendCompleted = FALSE;
	//s_ulLastNetflixSuspendStartTime = 0x0; /* Check only when turning to resume from suspend */
	//s_ulLastNetflixSuspendEndTime = 0x0;

	s_bOnChaingingState = TRUE;

	if(s_eNetflixSuspendStatus == NFLIX_SunpendStatus_Ongoing)
	{
		//HAPI_SendKeyHandlerReady(FALSE, 20000);
		HLIB_STD_TaskSleep(2000);
#if 0
		s_eNetflixSuspendStatus = NFLIX_SunpendStatus_NA;
		HxLOG_Print(" [NXMAN] \n\n\n  Suspend not completed => Kill Netflix(%d) !!\n", s_eSuspendPbState);
#else
		if(s_eNetflixSuspendStatus == NFLIX_SunpendStatus_Ongoing)
		{
			HxLOG_Warning(" [NXMAN]	\n\n\n  Suspend not completed => Kill Netflix !!\n");
			(void)AM_killApplication(FALSE);
			//s_bRestartSuspended = FALSE;
			//s_bShouldRestart = TRUE;
			s_eNetflixSuspendStatus = NFLIX_SunpendStatus_NA;
			bRestart = TRUE;
		}
		else
		{
			//HAPI_SendKeyHandlerReady(TRUE, 0);
		}
#endif
	}

	if(bRestart == FALSE)
	{
		rc = AM_SendCommandToApplication(CMD_RESUME);

		if(rc == Error_ACK_Timeout)
		{
			bRestart = TRUE;
			(void)AM_killApplication(FALSE);
		}
		else if(rc != 0)
		{
			app_started();

#if __NFLIX_WAITING_MODE_CHANGING_COMPLETE__
			do
			{
				HLIB_STD_TaskSleep(1000);
				ulCurTime = VK_TIMER_GetSystemTick();
				ulDiff = ulCurTime - ulStartTime;
				//HxLOG_Print(" [NXMAN] \t\t @#R \n");
			}while(ulDiff < NFLIX_MODE_CHANGING_WAIT_TIME);
#endif
			rc = 0;
		}
		else
		{
			app_started();
		}
	}

	if(bRestart == TRUE)
	{
		HLIB_STD_TaskSleep(1000);
		rc = AM_startApplication(s_nArgc, s_pArgv, false);
	}

	s_bOnChaingingState = FALSE;

	HxLOG_Print(" [NXMAN]   AM_doResume--(%d)\n", rc);
	return (int)rc;
}

static int AM_doResumeLink(void)
{
	char		rc = 0;
	HBOOL		bRestart = FALSE;
#if __NFLIX_WAITING_MODE_CHANGING_COMPLETE__
	HUINT32		ulStartTime, ulCurTime, ulDiff;

	ulStartTime = VK_TIMER_GetSystemTick();
#endif


	HxLOG_Print(" [NXMAN]   AM_doResumeLink++\n");

	//if (s_bNetflixSuspending == TRUE) s_bNetflixSuspending = FALSE;
	//if (s_bNetflixSuspendCompleted == TRUE) s_bNetflixSuspendCompleted = FALSE;
	//s_ulLastNetflixSuspendStartTime = 0x0; /* Check only when turning to resume from suspend */
	//s_ulLastNetflixSuspendEndTime = 0x0;

	s_bOnChaingingState = TRUE;

	if(s_eNetflixSuspendStatus == NFLIX_SunpendStatus_Ongoing)
	{
		//HAPI_SendKeyHandlerReady(FALSE, 20000);
		HLIB_STD_TaskSleep(2000);

		if(s_eNetflixSuspendStatus == NFLIX_SunpendStatus_Ongoing)
		{
			HxLOG_Warning(" [NXMAN]   Suspend not completed => Kill Netflix !!\n");
			(void)AM_killApplication(FALSE);
			s_eNetflixSuspendStatus = NFLIX_SunpendStatus_NA;
			bRestart = TRUE;
		}
		else
		{
			//HAPI_SendKeyHandlerReady(TRUE, 0);
		}
	}

	if(bRestart == FALSE)
	{
		rc = AM_SendCommandToApplication(CMD_RESUME);

		if(rc == Error_ACK_Timeout)
		{
			bRestart = TRUE;
			(void)AM_killApplication(FALSE);
		}
		else if(rc != 0)
		{
			app_started();

#if __NFLIX_WAITING_MODE_CHANGING_COMPLETE__
			do
			{
				HLIB_STD_TaskSleep(1000);
				ulCurTime = VK_TIMER_GetSystemTick();
				ulDiff = ulCurTime - ulStartTime;
				//HxLOG_Print(" [NXMAN] \t\t @#R \n");
			}while(ulDiff < NFLIX_MODE_CHANGING_WAIT_TIME);
#endif
			rc = 0;
		}
		else
		{
			app_started();
		}
	}

	if(bRestart == TRUE)
	{
		HLIB_STD_TaskSleep(1000);
		rc = AM_startApplication(s_nArgc, s_pArgv, false);
	}

	s_bOnChaingingState = FALSE;

	return (int)rc;
}

static int AM_doSleep(void)
{
	char		rc = 0;

#if __SUPPORT_SLEEP_WAKE__

	HxLOG_Print(" [NXMAN]   AM_doSleep++\n");
	rc = AM_SendCommandToApplication(CMD_POWER_SLEEP); /* ... */

	s_bNetflixSleep = TRUE;

	init_DialInfo();

#if !defined(CONFIG_OP_TDC)
	app_exited();
#endif
	
	signal(SIGCHLD, SIG_DFL);
#endif
	return rc;
}

static int AM_doWake(void)
{
	char		rc = 0;
#if __SUPPORT_SLEEP_WAKE__

	HxLOG_Print(" [NXMAN]   AM_doWake++\n");
	rc = AM_SendCommandToApplication(CMD_POWER_WAKE); /* ... */

#if !defined(CONFIG_OP_TDC)
	app_started();
#endif
	
	signal(SIGCHLD, AM_SignalChild);
#endif
	return rc;
}

static int AM_doKeyLeft(void)
{
	HxLOG_Print(" [NXMAN]   AM_doKeyLeft++\n");
	return AM_SendCommandToApplication(CMD_KEY_L);
}

static int AM_doKeyRight(void)
{
	HxLOG_Print(" [NXMAN]   AM_doKeyRight++\n");
	return AM_SendCommandToApplication(CMD_KEY_R);
}

static int AM_doKeyUp(void)
{
	HxLOG_Print(" [NXMAN]   AM_doKeyUp++\n");
	return AM_SendCommandToApplication(CMD_KEY_U);
}

static int AM_doKeyDown(void)
{
	HxLOG_Print(" [NXMAN]   AM_doKeyDown++\n");
	return AM_SendCommandToApplication(CMD_KEY_D);
}

static int AM_doKeyEnter(void)
{
	HxLOG_Print(" [NXMAN]   AM_doKeyEnter++\n");
	return AM_SendCommandToApplication(CMD_KEY_E);
}

static int AM_doKeyPause(void)
{
	HxLOG_Print(" [NXMAN]   AM_doKeyPause++\n");
	return AM_SendCommandToApplication(CMD_KEY_PAUSE);
}

static int AM_doKeyPlay(void)
{
	HxLOG_Print(" [NXMAN]   AM_doKeyPlay++\n");
	return AM_SendCommandToApplication(CMD_KEY_PLAY);
}

static int AM_doKeyOverlayPause(void)
{
	HxLOG_Print(" [NXMAN]  AM_doKeyOverlayPause++\n");
	return AM_SendCommandToApplication(CMD_KEY_OVERLAY_PAUSE);
}

static int AM_doKeyOverlayPlay(void)
{
	HxLOG_Print(" [NXMAN]  AM_doKeyOverlayPlay++\n");
	return AM_SendCommandToApplication(CMD_KEY_OVERLAY_PLAY);
}

#if __SUPPORT_NETFLIX_BACKGROUND__
static int AM_doBackground(void)
{
	int rc;

	HxLOG_Print(" [NXMAN]   AM_doBackground++\n");

	s_bOnChaingingState = TRUE;
	ulStartTime = VK_TIMER_GetSystemTick();

	rc = AM_SendCommandToApplication(CMD_BACKGROUND);

	s_bOnChaingingState = FALSE;

	return rc;
}

static int AM_doBackground_from_App(void)
{
	int rc;

	HxLOG_Print(" [NXMAN]   AM_doBackground_from_App++\n");

	s_bOnChaingingState = TRUE;
	ulStartTime = VK_TIMER_GetSystemTick();

	rc = AM_SendCommandToApplication(CMD_BACKGROUND_APPLICATION_REQUEST);

	s_bOnChaingingState = FALSE;

	return rc;
}

#endif

static int AM_doSuspend(void)
{
	int rc = 0;
#if __NFLIX_WAITING_MODE_CHANGING_COMPLETE__
	HUINT32	 ulStartTime, ulCurTime, ulDiff;

	ulStartTime = VK_TIMER_GetSystemTick();
#endif

	HxLOG_Print(" [NXMAN]   AM_doSuspend++\n");

	s_bOnChaingingState = TRUE;
	s_eNetflixSuspendStatus = NFLIX_SunpendStatus_Ongoing;

#if __SUPPORT_NETFLIX_BACKGROUND__

	if(s_eNflixState == NFLIX_BACKGROUNDED)
	{
		rc = AM_SendCommandToApplication(CMD_SUSPEND_FROM_BACKGROUND);
	}
	else
	{
		rc = AM_SendCommandToApplication(CMD_SUSPEND);
	}

#else

	rc = AM_SendCommandToApplication(CMD_SUSPEND);

#endif

	if(rc != 0)
	{
		HxLOG_Error(" [NXMAN]   Error> Suspended Time-OUT!!!..\n");

#if __NFLIX_WAITING_MODE_CHANGING_COMPLETE__
		do
		{
			HLIB_STD_TaskSleep(1000);
			ulCurTime = VK_TIMER_GetSystemTick();
			ulDiff = ulCurTime - ulStartTime;
			//HxLOG_Print(" [NXMAN] \n\t\t @#S \n");
		}while(ulDiff < NFLIX_MODE_CHANGING_WAIT_TIME);
#endif

		// Perform typical process, kill in resume acion
		rc = 0;
	}
	else
	{
		 s_eNetflixSuspendStatus = NFLIX_SunpendStatus_Completed;
	}

	HxLOG_Print(" [NXMAN]  call app_exited()..\n");
	app_exited();

	return rc;
}

static int AM_doSuspend_from_App(void)
{
	int			rc;
#if __NFLIX_WAITING_MODE_CHANGING_COMPLETE__
	HUINT32		ulStartTime, ulCurTime, ulDiff;

	ulStartTime = VK_TIMER_GetSystemTick();
#endif

	HxLOG_Print(" [NXMAN]   AM_doSuspend_from_App++\n");

	s_bOnChaingingState = TRUE;
	s_eNetflixSuspendStatus = NFLIX_SunpendStatus_Ongoing;

#if __SUPPORT_NETFLIX_BACKGROUND__

	if (s_eNflixState == NFLIX_BACKGROUNDED)
	{
		rc = AM_SendCommandToApplication(CMD_SUSPEND_FROM_BACKGROUND);
	}
	else
	{
		rc = AM_SendCommandToApplication(CMD_SUSPEND_APPLICATION_REQUEST);
	}

#else

	rc = AM_SendCommandToApplication(CMD_SUSPEND_APPLICATION_REQUEST);

#endif

	if(rc != 0)
	{
		HxLOG_Error(" [NXMAN]   Error> Suspended Time-OUT!!!..\n");

#if __NFLIX_WAITING_MODE_CHANGING_COMPLETE__
		do
		{
			HLIB_STD_TaskSleep(1000);
			ulCurTime = VK_TIMER_GetSystemTick();
			ulDiff = ulCurTime - ulStartTime;
		}while(ulDiff < NFLIX_MODE_CHANGING_WAIT_TIME);
#endif

		// Perform typical process, kill in resume acion
		rc = 0;
	}
	else
	{
		 s_eNetflixSuspendStatus = NFLIX_SunpendStatus_Completed;
	}

	HxLOG_Print(" [NXMAN]  call app_exited()..\n");
	app_exited();

	return rc;
}


static int AM_doStopLowResources(void)
{
	int rc;

	HxLOG_Print(" [NXMAN]   AM_doStopLowResources++\n");

	s_bOnChaingingState = TRUE;

	rc = AM_SendCommandToApplication(CMD_STOP_LOW_RESOURCES);
	s_bShouldRestart = true;

	if(rc != 0)
	{
		HxLOG_Print(" [NXMAN]  Error> Cleanup Time-OUT!!!..\n");
		rc = 0;
		//s_bCrashed = false;
	}

	{
		rc = AM_stopApplication(); /* waitpid for SIGCHLD */
		HxLOG_Print(" [NXMAN]  call app_exited()..\n");
		app_exited();
	}

	s_eNetflixSuspendStatus = NFLIX_SunpendStatus_NA;
	s_bOnChaingingState = FALSE;

	return rc;
}

static int AM_doStopDial(void)
{
	int rc;

	HxLOG_Print(" [NXMAN]   AM_doStopDial++\n");
	s_bOnChaingingState = TRUE;

	rc = AM_SendCommandToApplication(CMD_STOP_DIAL);
	s_bShouldRestart = false;

	if(rc != 0)
	{
		HxLOG_Error(" [NXMAN]   Error> Cleanup Time-OUT!!!..\n");
		rc = 0;
		//s_bCrashed = false;
	}

	{
		app_exited();
		rc = AM_stopApplication(); /* waitpid for SIGCHLD */
		HxLOG_Print(" [NXMAN]  call app_exited()..\n");
	}

	s_eNetflixSuspendStatus = NFLIX_SunpendStatus_NA;
	s_bOnChaingingState = FALSE;

	return rc;
}

static int AM_doExit(void)
{
	int rc;

	HxLOG_Print(" [NXMAN]   AM_doExit++\n");
	s_bOnChaingingState = TRUE;

	rc = AM_SendCommandToApplication(CMD_STOP_USER_KILL);
	s_bShouldRestart = false;

	if(rc != 0)
	{
		if(Error_ACK_Timeout == rc)
		{
			HxLOG_Critical(" [NXMAN]   Error> Cleanup ACK Time-OUT!! \n");
		}
		rc = 0;
	}

	{
		rc = AM_stopApplication(); /* waitpid for SIGCHLD */
		HxLOG_Print(" [NXMAN]  call app_exited()..\n");
		app_exited();
	}

	s_eNetflixSuspendStatus = NFLIX_SunpendStatus_NA;
	s_bOnChaingingState = FALSE;

	return rc;
}

static int AM_doSigKill(void)
{
	return AM_killApplication(TRUE);
}

static int AM_handleAction(Nflix_ControllerCmd_t cmd)
{
	int rc = Error_General;

	if(cmd >= CMD_CNT_NONE)
	{
		return rc;
	}

	HxLOG_Print(" [NXMAN]   AM_handleAction(%d)++\n", cmd);

	HxSEMT_Get(s_ulNxManSem);

	if (!(s_eNflixState & s_astActionTable[cmd].validStates))
	{
		HxLOG_Error(" [NXMAN]   @@ Error state : cmd(%d), %d(%d), s_bOnChaingingState : %d\n", cmd, s_eNflixState, s_astActionTable[cmd].validStates, s_bOnChaingingState);
		rc = Error_State;
		goto EXIT_HANDLER;
	}

	rc = s_astActionTable[cmd].action();
	if (rc)
	{
		goto EXIT_HANDLER;
	}

	HxLOG_Print(" [NXMAN]   rc(%d), newState(%d)\n", rc, s_astActionTable[cmd].newState);

	if (s_astActionTable[cmd].newState != NFLIX_NO_CHANGE)
	{
		s_eNflixState = s_astActionTable[cmd].newState;
		HxLOG_Print(" [NXMAN]   @@ New state : %d\n", s_eNflixState);
	}

EXIT_HANDLER:
	HxSEMT_Release(s_ulNxManSem);

	HxLOG_Print(" [NXMAN]   AM_handleAction--\n");

	return rc;
}

static const char * AM_ControllerCommandToString(Nflix_ControllerCmd_t cmd)
{
	const char * s = "unknown";

	if(cmd <= CMD_CNT_NONE)
	{
		return s_astCmdTable[cmd].cmd_str;
	}
	else
	{
		return s;
	}
}

static void AM_SignalChild(int iArg)
{
	int pid;
	int status;

	HxLOG_Print(" [NXMAN]   @@@@ AM_SignalChild++\n");
	HxSEMT_GetTimeout(s_ulNxManSem, 25);	

	if(s_eNflixState == NFLIX_NO_INSTANCE)
	{
		s_nApplicationPid = 0;
		s_nRestartCounter = 0;
		s_bOnChaingingState = FALSE;
		goto EXIT_SIGCHLD;
	}

	pid = waitpid(-1, &status, WUNTRACED | WCONTINUED);

	HxLOG_Print(" [NXMAN]   signal's wait() caught pid %d\n", pid) ;
	if(s_nApplicationPid != pid)
	{
		goto EXIT_SIGCHLD;
	}

	do {
		if (WIFSTOPPED(status)) {
			HxLOG_Print(" [NXMAN]   application stopped by pid %d\n", WSTOPSIG(status));
			break;
		}

		if (WIFCONTINUED(status)) {
			HxLOG_Print(" [NXMAN]   application continued\n");
			break;
		}

		s_nApplicationPid = 0;
		s_nRestartCounter = 0;
		s_bOnChaingingState = FALSE;
		s_bCrashed = true;
		s_eNflixState = NFLIX_NO_INSTANCE;

		if(s_eCurPowerState == ePowerState_StandbyMode)
		{
			s_bNetActivated = FALSE;
			s_bTimeSetted = FALSE;
			s_bSuspendStarted = FALSE; /* ... */
			s_bDiskStoreReady = FALSE;
			init_DialInfo();
		}
		else
		{
			HxLOG_Print(" [NXMAN]  call app_exited (abnormal)..\n");
			app_exited();
		}

		if (WIFEXITED(status))
		{
			HxLOG_Print(" [NXMAN]   application exited with rc of %d\n", WEXITSTATUS(status));
			if (WEXITSTATUS(status) == 0) {
				s_bCrashed = false;
			}
		}

		if (WIFSIGNALED(status))
		{
			HxLOG_Print(" [NXMAN]   application exited due to signal %d\n", WTERMSIG(status));
		}

	} while (0);

EXIT_SIGCHLD:
	signal(SIGCHLD, SIG_DFL);
	HxSEMT_Release(s_ulNxManSem);

	HxLOG_Print(" [NXMAN]   @@@@ AM_SignalChild--\n") ;
}

static char AM_SendCommandToApplication(Nflix_ClientCmd_t cmd)
{
	char rc = 0;
	fd_set set;
	struct timeval timeout;

	HxLOG_Print(" [NXMAN]   AM_SendCommandToApplication(%d)++\n", (int)cmd);

	if (s_nApplicationSessionFd == -1)
	{
		HxLOG_Error(" [NXMAN]   Application session is disconnected..!!\n");
		return Error_State;
	}

	HxSTD_MemSet(s_szNxCmd, 0, MAX_MFLIX_PAYLOAD_STR);

	s_szNxCmd[0] = (char)cmd;
	if((cmd == CMD_RESUME) || (cmd == CMD_RESUME_LINK))
	{
		//s_szUiQuery[1] = (char)s_stLaunchParam.enSrc;
		HxSTD_snprintf(&s_szNxCmd[1], MAX_MFLIX_PAYLOAD_STR-2, "%s", s_szNxPayload);
	}

	FD_ZERO(&set);
	FD_SET(s_nApplicationSessionFd, &set);

	timeout.tv_sec = (int)_NFLIX_ACK_TIMEOUT_;
	timeout.tv_usec = 0;

	if (HxSTD_StrLen(s_szNxCmd) != write(s_nApplicationSessionFd, s_szNxCmd, HxSTD_StrLen(s_szNxCmd)))
	{
		return Error_General;
	}

	if (1 == select(FD_SETSIZE, &set, NULL, NULL, &timeout))
	{
		read(s_nApplicationSessionFd, &rc, 1);
	}
	else
	{
		HxLOG_Error(" [NXMAN]   No ACK from AMClient..!!!\n");
		rc = Error_ACK_Timeout;
	}

	return rc;
}

static int AM_killApplication(HBOOL bExitNotify)
{
	int rc = Error_State;

	if (s_nApplicationPid > 0)
	{
		signal(SIGCHLD, SIG_DFL);
		HxLOG_Print(" [NXMAN]  Kill Netflix (abnormal)..\n");
		rc = kill(s_nApplicationPid, SIGKILL);
		wait(NULL);

		s_nApplicationPid = 0;
		s_nRestartCounter = 0;
		s_bOnChaingingState = FALSE;
		s_bCrashed = true;
		s_eNflixState = NFLIX_NO_INSTANCE;

		if(s_eCurPowerState == ePowerState_StandbyMode)
		{
			s_bNetActivated = FALSE;
			s_bTimeSetted = FALSE;
			s_bSuspendStarted = FALSE; /* ... */
			s_bDiskStoreReady = FALSE;
			init_DialInfo();
		}

		if(bExitNotify == TRUE)
		{
			HxLOG_Print(" [NXMAN]  call app_exited (abnormal)..\n");
			s_stExitState = NFLIX_NOTIFY_EXIT_NORMAL;
			app_exited();
		}

	}

	if(s_nApplicationSessionFd != -1)
	{
		close(s_nApplicationSessionFd);
		s_nApplicationSessionFd = -1;
	}

	return rc;
}

static int AM_startApplication(int argc, char *argv[], bool startSuspended)
{
	/*  -T trace_option : https://nrd.netflix.com/pages/viewpage.action?pageId=39911630 */
	char * opt[] = {
							//"--loglevel=200",
							//"-T JAVASCRIPT",
							//"--show-fps",
							//"--suspend-mode",									// Suspend handling enabled.
							//"--dpi-has-suspend",								// the device supports suspend mode
							//"--gibbon-platform=application-manager",			// Use application-manager platform
							//"--trace-areas=DPI,NCCP,MEDIACONTROL,DISK_CACHE,DISK_STORE",
							//"--logfile=/media/drive1/netflix.log",
							//"--inject-js=https://ppd.builds.test.netflix.net/view/NRDJS/job/PPD-NRDJS-master/1949/artifact/origin/master/nrdjs.min.js"
					};

	const char *arglist[32] = { NULL, };
	unsigned int		i = 0, j = 0, opt_cnt = 0;
	char				szCmd[64] = {0, };

	if (s_nApplicationPid != 0)
	{
		HxLOG_Print(" [NXMAN]   Already Launched '%s' \n",argv[0]);
		return Error_State;
	}

	HxLOG_Print(" [NXMAN]   AM_startApplication++\n");

    opt_cnt = sizeof(opt)/sizeof(char*);

	chdir("/usr/bin/netflix");

	HxLOG_Print(" [NXMAN]  ++Launching '%s' \n",argv[0]);
	s_nApplicationPid = vfork();
	HxLOG_Print(" [NXMAN]  --Launching '%s' -- [pid:%d] \n",argv[0], s_nApplicationPid);
	if (s_nApplicationPid == 0)
	{
		for(i = 0; i < argc; i++)
		{
			arglist[j] = argv[i];
			j++;
		}

		if (startSuspended)
		{
			arglist[j] = "--suspend";
			j++;
		}

		for (i = 0; i < opt_cnt; i++)
		{
			arglist[j] = opt[i];
			j++;
		}

		arglist[j] = NULL;

		execvp(argv[0], (char * const*)arglist);
		HxLOG_Print(" [NXMAN]  start > suspended :(%d)\n", startSuspended);
		exit(1);
	}
	else if (s_nApplicationPid < 0)
	{
		HxLOG_Print(" [NXMAN]  Error> failed to fork the netflix\n");
		return Error_General;
	}

	HxLOG_Print(" [NXMAN]   PID is '%d'.\n", s_nApplicationPid);
	s_bShouldRestart = false;

	HxSTD_snprintf(szCmd, 64, "echo 15 > /proc/%d/oom_adj", s_nApplicationPid);
	VK_SYSTEM_Command(szCmd);

	if ((s_nApplicationSessionFd = accept(s_nApplicationFd, NULL, NULL)) == -1)
	{
		HxLOG_Error(" [NXMAN]   accept error\n");
	}

	if(startSuspended == false)
	{
		app_started();
	}

	signal(SIGCHLD, AM_SignalChild);

	HxLOG_Print(" [NXMAN]   AM_startApplication--\n");

	return 0;
}

static int AM_stopApplication(void)
{
	int i;

	HxLOG_Print(" [NXMAN]   AM_stopApplication++\n");

	if (s_nApplicationPid > 0)
	{
		bool	exited = false;
		int		status;
		HUINT32	ulTimeOut;
		signal(SIGCHLD, SIG_DFL);

		if(s_stExitState == NFLIX_NOTIFY_EXIT_STANDBY)
		{
			ulTimeOut = _NFLIX_STANDBY_CLEAN_UP_TIMEOUT; // 90 sec
		}
		else
		{
			ulTimeOut = _NFLIX_NORMAL_CLEAN_UP_TIMEOUT; // 15 sec
		}

		for (i = 0; i < ulTimeOut; i++)
		{
			HxLOG_Print(" [NXMAN]  waitpid(%d) -- [%d]\n", s_nApplicationPid, i);
			if (waitpid(s_nApplicationPid, &status, WNOHANG) == s_nApplicationPid)
			{
				if (WIFEXITED(status))
				{
					HxLOG_Print(" [NXMAN]   application exited normally with rc of %d\n", WEXITSTATUS(status));
					exited = true;
					break;
				}

				if (WIFSIGNALED(status))
				{
					HxLOG_Print(" [NXMAN]   application exited due to signal %d\n", WTERMSIG(status));
					exited = true;
					break;
				}
			}
			HLIB_STD_TaskSleep(1000);
		}

		if (!exited)
		{
			(void)AM_killApplication(FALSE);
			s_bCrashed = FALSE;
			HxLOG_Critical(" [NXMAN]	 Time out...=> Kill Netflix..!! --\n");
			return 0;
		}

		s_nApplicationPid = 0;
		s_nRestartCounter = 0;
	}

	s_eNflixState = NFLIX_NO_INSTANCE;

	if(s_eCurPowerState == ePowerState_StandbyMode)
	{
		s_bNetActivated = FALSE;
		s_bTimeSetted = FALSE;
		s_bSuspendStarted = FALSE; /* ... */
		s_bDiskStoreReady = FALSE;
		init_DialInfo();
	}

	if(s_nApplicationSessionFd != -1)
	{
		close(s_nApplicationSessionFd);
		s_nApplicationSessionFd = -1;
	}

	HxLOG_Print(" [NXMAN]   AM_stopApplication--\n");
	return 0;
}


static void* AM_proc(void * arg)
{
	Nflix_ControllerCmd_t cmd;
	int rc;
	int		status;

	fd_set set;
	struct timeval timeout;

	s_nApplicationFd = socket(AF_UNIX, SOCK_STREAM, 0);

	if (s_nApplicationFd == -1)
	{
		HxLOG_Error(" [NXMAN]   NflxMgr socket error => Exit..\n");
		exit(-1);
	}

	memset(&s_stApplicationAddr, 0, sizeof(s_stApplicationAddr));

	s_stApplicationAddr.sun_family = AF_UNIX;
	strncpy(s_stApplicationAddr.sun_path, ApplicationSocketPath(), sizeof(s_stApplicationAddr.sun_path)-1);

	unlink(ApplicationSocketPath());

	rc = bind(s_nApplicationFd, (struct sockaddr*)&s_stApplicationAddr, sizeof(s_stApplicationAddr));

	if (rc == -1)
	{
		HxLOG_Error(" [NXMAN]   NflxMgr bind error => Exit.. rc:%d\n", rc);
		exit(-1);
	}

	rc = listen(s_nApplicationFd, 1);	// Only one application

	if (rc == -1)
	{
		HxLOG_Error(" [NXMAN]   NflxMgr listen error => Exit.. rc:%d\n", rc);
		exit(-1);
	}

	s_bAMRunning = true;

	while (TRUE)
	{
		FD_ZERO(&set);

		if((s_bFactoryReset == TRUE) || (s_bDiskStoreReady == FALSE))
		{
			HLIB_STD_TaskSleep(1000);
			continue;
		}

		if ((s_nApplicationSessionFd != -1) && (s_nApplicationPid > 0))
		{
			FD_SET(s_nApplicationSessionFd, &set);
		}
		timeout.tv_sec = 0;
		timeout.tv_usec = s_nAppRequestTimeoutInUsec;

		// Is the application requesting to be suspended or backgrounded?
		if (1 == select(FD_SETSIZE, &set, NULL, NULL, &timeout))
		{
			if (FD_ISSET(s_nApplicationSessionFd, &set))
			{
				char cmd = 0;
				read(s_nApplicationSessionFd, &cmd, 1);
				if (cmd == (char)CMD_AM_REQUEST_SUSPEND)
				{
					HxLOG_Print(" [NXMAN]   NflxMgr received Suspend-request from AMClient.\n");
					RequestExit(Reason_USER_Nav);
					s_stExitState = NFLIX_NOTIFY_EXIT_NORMAL;
				}
				else if (cmd == (char)CMD_AM_REQUEST_BACKGROUND)
				{
					HxLOG_Print(" [NXMAN]   NflxMgr received Background-request from AMClient.\n");
					RequestExit(Reason_USER_Nav);
					s_stExitState = NFLIX_NOTIFY_EXIT_NORMAL;
				}
				else if (cmd == (char)CMD_AM_STOP) /* GibbonOEM_Cleanup by Netflix NRDP */
				{
					// cleanup and need to waitpid
					HxLOG_Print(" [NXMAN]   NflxMgr received Cleanup-request  from AMClient.\n");
					RequestExit(Reason_USER_Kill);
				}
				else if (cmd == (char)CMD_AM_STARTED) /* ApplicationManagerAnnounceEventStarted() by Netflix NRDP's GibbonOEM_Start event */
				{
					HxLOG_Print(" [NXMAN]  @@@@  NflxMgr received EventStarted from AMClient \n\n\n");
					s_bOnChaingingState = FALSE;
				}
				else if (cmd == (char)CMD_AM_SUSPENDED) /* ApplicationManagerAnnounceEventSuspended() by Netflix NRDP's GibbonOEM_Suspended event */
				{
					HxLOG_Print(" [NXMAN] \t  @@@@@ NflxMgr received EventSuspend-completed from AMClient ++[suspend:%d]\n\n\n", s_eNetflixSuspendStatus);
					s_bOnChaingingState = FALSE;
					s_eNetflixSuspendStatus = NFLIX_SunpendStatus_Completed;
				}
				else
				{
					;//continue;
				}
			}
		}

		// The application should always be running if resources are available
		// unless the user exits, there is a DIAL stop or the user via some
		// external UI kills the application.
		if (s_nApplicationPid == 0)
		{
			/* HxLOG_Print(" [NXMAN]  Error> invalid ApplicationPid(fd:%d)... maybe killed\n", s_nApplicationSessionFd); */
			if(s_nApplicationSessionFd != -1)
			{
				close(s_nApplicationSessionFd);
				s_nApplicationSessionFd = -1;
			}

			if((s_eCurPowerState == ePowerState_OperationMode) && (s_bFactoryReset == FALSE) && (s_bShouldRestart/* || s_bCrashed : NRDP 4.2 chnage*/))
			{
				if (s_nRestartCounter >= (s_nRestartTimeoutInSec*1000000/s_nAppRequestTimeoutInUsec))
				{
					HBOOL	bLaunchArgs = FALSE;

					HxLOG_Print(" [NXMAN]   Try to re-Launching Netflix ..\n");
					s_bCrashed = false;
					s_nRestartCounter = 0;

					HxSTD_MemSet(&s_stLaunchParam, 0, sizeof(Launch_Param_t));

					if(s_bRestartSuspended == TRUE)
					{
						s_stLaunchParam.enSrc = Source_SUSPENDED_AFTER_APP_RESTART;
					}
					else
					{
						s_stLaunchParam.enSrc = Source_DEDICATED_ON_SCREEN_ICON;
					}

					bLaunchArgs = make_LaunchArgs();
					if(bLaunchArgs == TRUE)
					{
						s_bShouldRestart = FALSE;
						if(s_bRestartSuspended == TRUE)
						{
							HxLOG_Print(" [NXMAN]  --> AM_handleAction(CMD_CNT_START_SUSPEND)\n");
							rc = AM_handleAction(CMD_CNT_START_SUSPEND);
						}
						else
						{
							HxLOG_Print(" [NXMAN]  --> AM_handleAction(CMD_CNT_START_FOREGROUND)\n");
							rc = AM_handleAction(CMD_CNT_START_FOREGROUND);
							s_bRestartSuspended = TRUE;
						}
						HxLOG_Print(" [NXMAN]   re-start caused by foced kill... rc : %d\n", rc);
					}
					else
					{
						HxLOG_Error(" [NXMAN]   Error> make_LaunchArgs() is failed..!!\n");
					}
				}
				else
				{
					s_nRestartCounter++;
				}
			}
		}

		#if 0
		if ((s_nControllerSessionFd = accept(s_nControllerFd, NULL, NULL)) == -1)
		{
			continue;
		}

		//cmd = AM_GetCommandFromController();
		HxLOG_Print(" [NXMAN]   ReceiveCmd '%s' from AMC \n", AM_ControllerCommandToString(cmd));
		#endif

		if((HLIB_QUEUE_Length(s_pAMQueue) == 0) || (s_bOnChaingingState == TRUE))
		{
			continue;
		}
		cmd = (Nflix_ControllerCmd_t)HLIB_QUEUE_Pop(s_pAMQueue);

		if(cmd == CMD_CNT_START_FOREGROUND)
		{
			if((s_eNflixState == NFLIX_SUSPENDED) || (s_eNflixState == NFLIX_BACKGROUNDED))
			{
				cmd = CMD_CNT_RESUME;
			}
			else
			{
				cmd = CMD_CNT_START_FOREGROUND;
			}
		}

		rc = AM_handleAction(cmd);
		switch (rc)
		{
			case Error_General:
				HxLOG_Print(" [NXMAN]   'Error_General'\n");
				break;
			case Error_Timeout:
				HxLOG_Print(" [NXMAN]   'Error_Timeout'\n");
				break;
			case Error_State:
				HxLOG_Print(" [NXMAN]   'Error_State'\n");
				break;
			default:
				HxLOG_Print(" [NXMAN]   rc : %d\n", rc);
				break;
		}

		HLIB_STD_TaskSleep(10);
	}


	waitpid(s_nApplicationPid, &status, 0);

	pthread_detach(pthread_self());

	s_bAMRunning = false;

	HxLOG_Print(" [NXMAN]   Exit AM_Proc()\n");

	return NULL;
}

/**************************************************************
	public functions
 **************************************************************/

#define	____APPLICATION_MANAGER____MAIN

int		main(int argc, char **argv)
{
	HERROR				rErr;
	struct stat			sts;

	unsigned long		tid;

	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_DFL);

	//HxLog_SetLevel(HxLOG_DOMAIN, HxLOG_ALL);

	//	initialize vkernel
	VK_Init();

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif

#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();
#endif

	HxSEMT_Create(&s_ulOverlaySem, "netflix_overlay_sema", HxSEMT_FIFO);
	HxSEMT_Create(&s_ulNxManSem, "NxMan_sema", HxSEMT_FIFO);
#if __USE_CALLBACK_FOR_DIAL_CHECKING__
	HxSEMT_Create(&s_ulDialSem, "NxMan_dial_sema", HxSEMT_FIFO);
#endif

	if(APK_DB_Init() != ERR_OK)
	{
		HxLOG_Print(" [NXMAN]   APK_DB_Init() failed...!\n");
	}

	if(APK_SYSINFO_STB_Init() != ERR_OK)
	{
		HxLOG_Print(" [NXMAN]   APK_SYSINFO_STB_Init() failed...!\n");
	}

	if(APK_NETWORK_Init() != TRUE)
	{
		HxLOG_Print(" [NXMAN]   APK_NETWORK_Init() failed...!\n");
	}

	if(APK_OUTPUT_HDMI_Init() != ERR_OK)
	{
		HxLOG_Print(" [NXMAN]   APK_OUTPUT_HDMI_Init() failed...!\n");
	}

	if(APK_MEDIA_PLAY_Init() != ERR_OK)
	{
		HxLOG_Print(" [NXMAN]   APK_MEDIA_PLAY_Init() failed...!\n");
	}

	init_DialInfo();

	APK_DIAL_Init();
#if __USE_CALLBACK_FOR_DIAL_CHECKING__
	APK_DLNA_COMMON_RegisterNotifier(__nxman_HommaStatusCallback);				// For checking HOMMA(RPC) Starting 
	APK_DIAL_RegisterNotifier((APKS_DLNA_CbHandler) __nxman_DialEventCallback);	// For checking DIAL Activation
#else
	HxSTD_MemSet(s_szAdditionalUrl, 0, MAX_ENCORDED_URL_LEN);
#endif

	if(APK_POWER_Init() != ERR_OK)
	{
		HxLOG_Print(" [NXMAN]   APK_POWER_Init() failed...!\n");
	}

	s_bNetActivated = FALSE;
	s_bTimeSetted = FALSE;
	s_bSuspendStarted = FALSE;
	s_eNetflixSuspendStatus = NFLIX_SunpendStatus_NA;
	s_eOverlayState = NFLIX_OVERLAY_NA;
	s_bFirstBoot	= FALSE;
	s_bFactoryReset = FALSE;
	s_bOnChaingingState = FALSE;
	s_bDiskStoreReady = FALSE;

#if __SUPPORT_SLEEP_WAKE__
	s_bNetflixSleep = FALSE;
#endif

	HxSTD_MemSet(&s_stSysInfo, 0, sizeof(NFlix_SysInfo_t));

	HxLOG_Print(" [NXMAN]   @@ Netflix Manager is launched....!!\n");



	rErr = APK_DB_GetInt(UDB_SYSTEM_FIRSTTIME_BOOT, &s_bFirstBoot);
	if (rErr != ERR_OK)
	{
		HxLOG_Print(" [NXMAN]  @@@@  Getting FirstBoot is failed..!!\n");
		s_bFirstBoot = TRUE;
	}
	else
	{
		HxLOG_Print(" [NXMAN]  @@@@  bFirstBoot(%d)...\n", s_bFirstBoot);
	}

	Init_cache();
	check_diskcache();

	if(stat(NFLIX_CREDENTIALS, &sts) == -1 && errno == ENOENT)
	{
		s_bTryProvisioning = FALSE;
		s_bProvisioningDone = FALSE;
	}
	else
	{
		s_bTryProvisioning = TRUE;
		s_bProvisioningDone = TRUE;
	}

	if(APK_POWER_GoingToOperate() == TRUE)
	{
		s_eCurPowerState = ePowerState_OperationMode;
		HxLOG_Print(" [NXMAN]   s_eCurPowerState (%d) \n", s_eCurPowerState);
	}

	HAPI_AddVirtualWindow(eAWINDOW_NETFLIXMANAGER);

	HAPI_ConnectSignal("signal:onStartNetflixAppl", NULL, __nxman_onStartNetflixAppl);
	HAPI_ConnectSignal("signal:onStopNetflixAppl",  NULL, __nxman_onStopNetflixAppl);
	HAPI_ConnectSignal("signal:onNetflixPlaybackChanged",  NULL, __nxman_onNetflixPlaybackChanged);
	HAPI_ConnectSignal("signal:onNotifyOverlayState",  NULL, __nxman_onNotifyOverlayState);
	HAPI_ConnectSignal("signal:onSystemStateChanged",  NULL, __nxman_OnPowerStateChanged);
#if __PRIVACY_POLICY_AUTH__				
	HAPI_ConnectSignal("signal:onPPVerChanged", NULL, __nxman_onPPVerChanged);
#endif

	HAPI_SetWindowStyle(eAWINDOW_NETFLIXMANAGER, eHWS_WindowInputAlways, FALSE);

	HAPI_RegisterAcceptableKey(eAWINDOW_NETFLIXMANAGER, DIKS_EXIT, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_NETFLIXMANAGER, DIKS_CHANNEL_UP, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_NETFLIXMANAGER, DIKS_CHANNEL_DOWN, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_NETFLIXMANAGER, DIKS_EPG, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_NETFLIXMANAGER, DIKS_HOME, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_NETFLIXMANAGER, DIKS_MENU, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_NETFLIXMANAGER, DIKS_INTERNET, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_NETFLIXMANAGER, DIKS_PLAYER, TRUE);

	//	for key task
	VK_TASK_Create(AM_keyevent_handler, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "NxManKey", NULL, &tid, 0);
	VK_TASK_Start(tid);

	do_ReportManager();

	runNetflixManager();

#if 0//O_CHECK_DIAL_ACTIVATION__

	/* deinitialize */
	APK_DIAL_UnRegisterNotifier((APKS_DLNA_CbHandler) local_rcDialEventCallback); /* Unused, but for updating appkit context by OCTO */
	//APK_MEDIACTRL_UnRegisterNotifier(_mediapbHandle, local_mediapbNotifier);
#endif

	if(APK_SYSINFO_STB_DeInit() != ERR_OK)
	{
		HxLOG_Error(" [NXMAN]   APK_SYSINFO_STB_DeInit() failed...!\n");
	}

	/*
	if(APK_NETWORK_DeInit() != ERR_OK)
	{
		HxLOG_Error(" [NXMAN]   APK_NETWORK_DeInit() failed...!\n");
	}
	*/

	/*
	if(APK_OUTPUT_HDMI_DeInit() != ERR_OK)
	{
		HxLOG_Error(" [NXMAN]   APK_OUTPUT_HDMI_DeInit() failed...!\n");
	}
	*/
	/*
	if(APK_MEDIA_PLAY_Deinit() != ERR_OK)
	{
		HxLOG_Error(" [NXMAN]   APK_MEDIA_PLAY_Deinit() failed...!\n");
	}
	*/

	if (s_nApplicationFd > 0) {
		close(s_nApplicationFd);
		s_nApplicationFd = (-1);
	}
	if (s_nApplicationSessionFd > 0) {
		close(s_nApplicationSessionFd);
		s_nApplicationSessionFd = (-1);
	}
	return 0;
}

