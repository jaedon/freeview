/**************************************************************
 *	@file		keyboard.c
 *	System Keyboard Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/14
 *	@author			humax
 *	@brief			System Keyboard Application
 **************************************************************/
#include <ondk.h>

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

#include "keyboard.h"
#include "kbd_keitai_jap.h"


#define	MAKE_KEYUP_PAIR

/**************************************************************
	internal structures and variables
 **************************************************************/

#undef		COMMAND_NAME
#define		COMMAND_NAME				"APP_KEYBD"
#define		HxCMD_PORT_APP_SUB			HxCMD_PORT_KEYBOARD  // telnet port!!!

#define		KBD_SCREEN_WIDTH				1280
#define		KBD_SCREEN_HEIGHT				720
#define		KBD_TITLE_MAXLENGTH			80
//#define 		JAPANESE_FONT_PATH 	"/usr/fonts/FreeSerif.ttf"
#define 		JAPANESE_FONT_PATH 	"/usr/fonts/JP-ARIB-JGTR01M.ttf"


static		HUINT8	_gKeywordSearch[KBD_TITLE_MAXLENGTH] = 		// the Title :  "KEYWORD SEARCH"
{	0xE3,0x82,0xAD, 0xE3,0x83,0xBC, 0xE3,0x83,0xAF, 0xE3,0x83,0xBC, 0xE3,0x83,0x89, 0xE6,0xA4,0x9C, 0xE7,0xB4,0xA2, 0x00, 0x00, 0x00,
};


typedef struct	_KEYBDData
{
	HBOOL			show;
	HBOOL			isUpdated;			// 불필요하게 반복적으로 그려지는 것을 방지하기 위함. - Update (키 입력) 이 있을 때에만 새로 그림.
	HBOOL			isPasswordMode;
	ONDK_Rect_t		InvalidateRect;
	HUINT32			parent;
	HUINT8			title[KBD_TITLE_MAXLENGTH];
} KEYBDData_t;

static 	KEYBDData_t			_stKeyboardInfo;

static		ONDKWindowHandle	gKBDWnd;
static		ONDKSurfaceHandle 	gKBDSurface;
static		ONDKSurfaceHandle 	gShadowBitmap = NULL;
static		ONDKFontHandle		gKBDFont, gKBDSmallFont;
static 	HUINT32				m_KeyID;

// global dfb window id for hevent process
HUINT32			m_window_id;

//#define TEST_CASE
#if defined(TEST_CASE)
static 	HUINT32 timerid = 0;
#endif


#define	MAX_INPUT_LENGTH	1024

static 	unsigned char sLocalBufferUTF8[MAX_INPUT_LENGTH];
//static	int	caret_position = 0;



/**************************************************************
	private functions
 **************************************************************/
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

void 	APKeyboard_CMD_Register(HUINT8 *cmdname)
{
#define	hCmdHandle		"KEYBD_CMD"
}

#endif

#if defined(TEST_CASE)
static void cbTimeoutOccur(HLONG ulTimerId, HUINT32 *param)
{
	HxLOG_Debug("cbTimeoutOccur : [%x]\n", ulTimerId);

	static int i = 0;
	if (i < 2)
	{	// test
		HAPI_BroadcastSignal("signal:onOSKResult", 0, "s", "This is a result of keyboard.");
		HAPI_BroadcastSignal("signal:onOSKShow", 0, "i", 0);
		i++;
		HxLOG_Print("++++++++++++++++++++++++ %s %d\n", __FUNCTION__, __LINE__);
	}	// test
	else
	{
		VK_TIMER_Cancel(timerid);
		i = 0;
	}
}
#endif

/**************************************************************
  Event Handler
 **************************************************************/
static HERROR	onOSKModeChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	_stKeyboardInfo.isPasswordMode = HxOBJECT_INT(apArgv[0]) ? TRUE : FALSE;

	return ERR_OK;
}

static HERROR	onOSKCharModeChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	KBD_SetAvailableInputMode(HxOBJECT_INT(apArgv[0]));

	return ERR_OK;
}

static HERROR	onOSKDefaultString(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HLIB_STD_StrNCpySafe(sLocalBufferUTF8, HxOBJECT_STR(apArgv[0]), MAX_INPUT_LENGTH);
	KBD_SetStringToEdit( sLocalBufferUTF8 );
	KBD_SetEditWindow(gKBDFont, 221, 307, 838, 42 );
	KBD_SetBufferMaxLength( 96 );//MAX_INPUT_LENGTH );

	return ERR_OK;
}

static HERROR	onOSKInitPosition(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	KBD_SetInputPosition(HxOBJECT_INT(apArgv[0]));

	return ERR_OK;
}

static HERROR	onOSKMaxLengthChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	KBD_SetBufferMaxLength(HxOBJECT_INT(apArgv[0]));

	return ERR_OK;
}

static HERROR	onOSKShow(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	if (HxOBJECT_INT(apArgv[0]))
	{
#if defined(TEST_CASE)
		ONDK_Rect_t src_rect;

		VK_TIMER_EventEvery(5000,cbTimeoutOccur,NULL,0,&timerid);
		ONDK_WINDOW_Clear(gKBDWnd);
		src_rect = ONDK_Rect(KBD_SCREEN_WIDTH / 2 - 200, KBD_SCREEN_HEIGHT / 2 - 150, 400, 300);
		ONDK_DRAW_FillRect(gKBDSurface, src_rect, ONDK_Color(0xFF, 0xFF, 0xFE, 0xFF));
		ONDK_SURFACE_SetCopy(gKBDSurface);
		ONDK_SURFACE_StretchBlit(gKBDWnd, gKBDSurface, src_rect, src_rect);
		ONDK_WINDOW_Update(gKBDWnd, &src_rect);
#endif
		_stKeyboardInfo.show = TRUE;
		_stKeyboardInfo.isUpdated = TRUE;
		_stKeyboardInfo.InvalidateRect.x = 0;	// 최초 전체 화면을 ReDraw 해야
		_stKeyboardInfo.InvalidateRect.y = 0;
		_stKeyboardInfo.InvalidateRect.w = KBD_SCREEN_WIDTH;
		_stKeyboardInfo.InvalidateRect.h = KBD_SCREEN_HEIGHT;

		ONDK_WINDOW_Clear(gKBDWnd);
		ONDK_WINDOW_Update(gKBDWnd, NULL);
		ONDK_WINDOW_SetShow(gKBDWnd, TRUE);
		HxLOG_Print("++++++++++++++++++++++++ %s %d\n", __FUNCTION__, __LINE__);
	} else
	{
		ONDK_WINDOW_SetShow(gKBDWnd, FALSE);
		_stKeyboardInfo.show = FALSE;
		HxLOG_Print("++++++++++++++++++++++++ %s %d\n", __FUNCTION__, __LINE__);
	}

	return ERR_OK;
}

static HERROR	onOSKDestroy(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	KBD_SetStringToEdit( sLocalBufferUTF8 );
	KBD_SetInputPosition( 0 );
	KBD_SetBufferMaxLength( 96 );//MAX_INPUT_LENGTH );
	KBD_SetAvailableInputMode(KBD_ALL_AVAILABLE );

	return ERR_OK;
}

/**************************************************************
	Static Function
 **************************************************************/

//#include "kbd_keitai_jap.c"

#ifdef	MAKE_KEYUP_PAIR
#define	MAX_KEYDOWNMEM	8
HUINT32	keydownState[MAX_KEYDOWNMEM] = { KEY_UNKNOWN, };

void	clear_keydown_states(void)
{	int	i;
	for (i=0; i<MAX_KEYDOWNMEM; i++)
	{	keydownState[i] =KEY_UNKNOWN;
	}
}
bool	is_keydown_states(HUINT32 keycode)
{	int	i;
	for (i=0; i<MAX_KEYDOWNMEM; i++)
	{	if ( keydownState[i] == keycode)		// 이미 down 상태.
			return	TRUE;
	}
	return	FALSE;
}

void 	remember_keydown(HUINT32 keycode)
{	int	i;
	for (i=0; i<MAX_KEYDOWNMEM; i++)
	{	if ( ( KEY_UNKNOWN == keydownState[i] ) ||( keycode == keydownState[i] ) )
		{	keydownState[i] = keycode;
			return;	//break;
		}
	}
}
void 	forget_keydown(HUINT32 keycode)
{	int	i;
	for (i=0; i<MAX_KEYDOWNMEM; i++)
	{	if ( keycode == keydownState[i] )
		{	keydownState[i] = KEY_UNKNOWN ;
			return;	//break;
		}
	}
}

#else
#endif

static 	void	makeKeyboardWindow(void)
{
	gKBDWnd = ONDK_WINDOW_Create(eAWINDOW_KEYBOARD, 0, 0, KBD_SCREEN_WIDTH, KBD_SCREEN_HEIGHT, "ARGB8888", TRUE);

	ONDK_WINDOW_SetShow(gKBDWnd, TRUE);
	_stKeyboardInfo.show = FALSE;
	strncpy( _stKeyboardInfo.title, _gKeywordSearch, KBD_TITLE_MAXLENGTH);
	ONDK_WINDOW_Update(gKBDWnd, NULL);
//	gKBDSurface = MLIB_CreateSurface(KBD_SCREEN_WIDTH, KBD_SCREEN_HEIGHT, "ARGB8888");
	gKBDSurface = ONDK_SURFACE_GetFromWindow(gKBDWnd);

//	ONDK_WINDOW_SetResolution(gKBDWnd, 960, 540);
//	ONDK_WINDOW_SetWindowPos( gKBDWnd, 0, 0, 640, 360);
#ifdef	MAKE_KEYUP_PAIR
	clear_keydown_states();
#else
#endif
}


#define	SHADOW_CORNER_SIZE		10
#define	SHADOW_COLOR_BRIGHT		0x00101010
#define	SHADOW_COLOR_DARK		0xCF202020

static 	void	makeshadowBitmap(void)
{
	int		i;
	ONDK_Color_t 		color;
	gShadowBitmap = ONDK_SURFACE_Create(SHADOW_CORNER_SIZE*2, SHADOW_CORNER_SIZE*2, "ARGB8888");
	ONDK_SURFACE_Clear(gShadowBitmap);
	for (i=SHADOW_CORNER_SIZE-1; i>0; i--)
	{
		color = ONDK_COLOR_BlendWithAlpha(SHADOW_COLOR_BRIGHT, SHADOW_COLOR_DARK, (unsigned char)(i*256/SHADOW_CORNER_SIZE) );
		ONDK_DRAW_FillCircle(gShadowBitmap, SHADOW_CORNER_SIZE, SHADOW_CORNER_SIZE, i, color);
	}
}

static 	void	drawShadow(ONDKSurfaceHandle surface, int x, int y, int w, int h)
{
	ONDK_Rect_t	_rc = { 0, 0, SHADOW_CORNER_SIZE, SHADOW_CORNER_SIZE };

	if (gShadowBitmap == NULL )
	{	HxLOG_Print("WARNING: /// Not ready to draw shadow. call 'makeshadowBitmap( )' first.\n");
		return;
	}

	ONDK_SURFACE_SetCopy(surface);
	ONDK_SURFACE_Blit(gShadowBitmap, surface, _rc, x, y);
	_rc.x = SHADOW_CORNER_SIZE;
	ONDK_SURFACE_Blit(gShadowBitmap, surface, _rc, x+w-SHADOW_CORNER_SIZE, y);
	_rc.y = SHADOW_CORNER_SIZE;
	ONDK_SURFACE_Blit(gShadowBitmap, surface, _rc, x+w-SHADOW_CORNER_SIZE, y+h-SHADOW_CORNER_SIZE);
	_rc.x = 0;
	ONDK_SURFACE_Blit(gShadowBitmap, surface, _rc, x, y+h-SHADOW_CORNER_SIZE);

	_rc.x = x+SHADOW_CORNER_SIZE;		_rc.y = y;
	_rc.w = w-SHADOW_CORNER_SIZE-SHADOW_CORNER_SIZE;	_rc.h = SHADOW_CORNER_SIZE;
	ONDK_DRAW_FillGradationRect(surface, _rc, TRUE, SHADOW_COLOR_BRIGHT, SHADOW_COLOR_DARK);
	_rc.y = y+h-SHADOW_CORNER_SIZE;
	ONDK_DRAW_FillGradationRect(surface, _rc, TRUE, SHADOW_COLOR_DARK, SHADOW_COLOR_BRIGHT);
	_rc.x = x;	_rc.y = y+SHADOW_CORNER_SIZE;
	_rc.w = SHADOW_CORNER_SIZE;		_rc.h = h-SHADOW_CORNER_SIZE-SHADOW_CORNER_SIZE;
	ONDK_DRAW_FillGradationRect(surface, _rc, FALSE, SHADOW_COLOR_BRIGHT, SHADOW_COLOR_DARK);
	_rc.x = x+w-SHADOW_CORNER_SIZE;
	ONDK_DRAW_FillGradationRect(surface, _rc, FALSE, SHADOW_COLOR_DARK, SHADOW_COLOR_BRIGHT);

	_rc.x = x+SHADOW_CORNER_SIZE;		_rc.y = y+SHADOW_CORNER_SIZE;
	_rc.w = w-SHADOW_CORNER_SIZE-SHADOW_CORNER_SIZE;		_rc.h = h-SHADOW_CORNER_SIZE-SHADOW_CORNER_SIZE;
	ONDK_DRAW_FillRect(surface, _rc, SHADOW_COLOR_DARK);
}

static	void	drawBoxShadow(ONDKSurfaceHandle surface, int x, int y, int w, int h)
{
	int	offset_minus = (SHADOW_CORNER_SIZE+SHADOW_CORNER_SIZE+SHADOW_CORNER_SIZE)>>2;		// 1.0 * 3/4;
	int	offset_plus = SHADOW_CORNER_SIZE+SHADOW_CORNER_SIZE;
	drawShadow(surface, x-offset_minus , y-offset_minus , w+offset_plus , h+offset_plus  );
}
static	void	eraseBoxShadow(ONDKSurfaceHandle surface, int x, int y, int w, int h)
{
	ONDK_Rect_t	_rc;
	int	offset_minus = (SHADOW_CORNER_SIZE+SHADOW_CORNER_SIZE+SHADOW_CORNER_SIZE)>>2;		// 1.0 * 3/4;
	int	offset_plus = SHADOW_CORNER_SIZE+SHADOW_CORNER_SIZE;
	_rc.x = x-offset_minus ;
	_rc.y = y-offset_minus ;
	_rc.w = w+offset_plus ;
	_rc.h = h+offset_plus ;
	ONDK_DRAW_FillRect(surface, _rc, 0x00000000 );
}


#define	KBD_POS_W	870		//660
#define	KBD_POS_H	250		//136
#define	KBD_POS_X	205		//300
#define	KBD_POS_Y	((720-KBD_POS_H)/2)		//237

#define	KBD_BG_GRADATION_COLOR_DARK		0x00101010
#define	KBD_BG_GRADATION_COLOR_BRIGHT		0xFF606060	//0xE8606060
#define	KBD_INPUTBOX_BGCOL_BRIGHT	0xFFC0C0C0
#define	KBD_INPUTBOX_BGCOL_DARK		0xFF808080
#define	KBD_INPUTBOX_BGCOL_SHADOW		0xFF404040

#define	KBD_FONT_SIZE	30

static		void	local_drawInputBoxBG(ONDKSurfaceHandle surface, int x, int y, int w, int h)
{
	ONDK_Rect_t	_rc;

	_rc.x = x;	_rc.y = y;	_rc.w = w;	_rc.h = h;
	ONDK_DRAW_FillGradationRect(gKBDSurface, _rc, TRUE, KBD_INPUTBOX_BGCOL_BRIGHT, KBD_INPUTBOX_BGCOL_DARK );
	_rc.y += _rc.h-1;	_rc.h = 2;		// 틔   체.
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_BRIGHT );
	_rc.y = y;					//   恝 체.
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_SHADOW );
	_rc.h = h;	_rc.w = 2;		// 恝  체.
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_SHADOW );
	_rc.x = (x + w) -1;					//   체.
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_BRIGHT );
}

static	 HUINT8	kanji_utf8[] = { 0xE6, 0xBC, 0xA2, 0x00 };	//
static	 HUINT8	hankaku_utf8[] = { 0xEF, 0xBD, 0xB6, 0xEF, 0xBE, 0x85, 0x00 };	//腑
static	 HUINT8	eigo_utf8[] = { 0xEF, 0xBC, 0xA1, 0x00 };	//
static	 HUINT8	suuji_utf8[] = { 0xEF, 0xBC, 0x91, 0x00 };	//

	#if	1	// USE PNG
	#else
static	 HUINT8	blue_utf8[] = { 0xE9, 0x9D, 0x92, 0x00 };	//청
static	 HUINT8	red_utf8[] = { 0xE8, 0xB5, 0xA4, 0x00 };	//
static	 HUINT8	green_utf8[] = { 0xE7, 0xB7, 0x91, 0x00 };	//
static	 HUINT8	yellow_utf8[] = { 0xE9, 0xBB, 0x84, 0x00 };	//황
static	 HUINT8	back_utf8[] = { 0xE6, 0x88, 0xBB, 0xE3, 0x82, 0x8B, 0x00 };	//醍돈
static	 HUINT8	left_utf8[] = { 0xE2, 0x97, 0x80, 0x00 };	// 화표
static	 HUINT8	right_utf8[] = { 0xE2, 0x96, 0xB6, 0x00 };	// 화
	#endif

static	 HUINT8	modechange_utf8[] = {  0xE5, 0x85,0xA5, 0xE5, 0x8A,0x9B,0xE3,0x83,0xA2,0xE3,0x83,0xBC,0xE3,0x83,0x89,0x00 };	//韜쨍
static	 HUINT8	convert_utf8[] = { 0xE5,0xA4,0x89,0xE6,0x8F,0x9B,0x00 };	//환
static	 HUINT8	delete_utf8[] = { 0xEF,0xBC,0x91,0xE6,0x96,0x87,0xE5,0xAD,0x97,0xE5,0x89,0x8A,0xE9,0x99,0xA4, 0x00 };	//1
static	 HUINT8	confirm_utf8[] = { 0xE7,0xA2,0xBA,0xE5,0xAE,0x9A, 0x00 };	//확
static	 HUINT8	cancel_utf8[] = { 0xE5,0x85,0xA5,0xE5,0x8A,0x9B,0xE5,0x8F,0x96,0xE6,0xB6,0x88, 0x00 };	//
static	 HUINT8	cursor_utf8[] = { 0xE5,0x85,0xA5,0xE5,0x8A,0x9B,0xE4,0xBD,0x8D,0xE7,0xBD,0xAE,0xE5,0xA4,0x89,0xE6,0x9B,0xB4, 0x00 };	//韜치


unsigned char utf8string[48] = {       0xe3, 0x081, 0x82,            0xe2, 0x80, 0x94,              0xe3, 0x081, 0x84,
             0x42,  0xcc, 0x81,             0x42,  0xcc, 0x80,             0x42,  0xcc, 0x88,             0x42,  0xcc, 0x82,             0x42,  0xcc, 0x85,
             0xe3, 0x80, 0x9c,              0xe2, 0x88, 0x92,
             0x00, 0x00
};

/*
static	char	multiline_utf8string[] = {
			0xE6,0x97,0xA5,	0xE6,0x9C,0xAC,	0xE8,0xAA,0x9E,	0xE3,0x82,0x82,
			0xE5,0x87,0xBA,	0xE6,0x9D,0xA5,	0xE3,0x82,0x8B,	0x41,0x42,   0xE3,0x81,0x8B,	0x0a,
			0xE5,0x88,0x86,	0xE3,0x81,0x8B,	0xE3,0x82,0x89,	0xE3,0x81,0xAA,
			0xE3,0x81,0x84,	0x33,  0xE3,0x81,0xA0,	0xE3,0x82,0x8D,	0xE3,0x81,0x86,
			0xE3,0x80,0x82,	0x0a,		0xE3,0x81,0xA7,	0xE3,0x81,0x97,
			0xE3,0x82,0x87,	0xE3,0x81,0x86,	0xEF,0xBC,0x9F,	0x00,
			};
*/
#define	BUTTON_WIDTH		48		//34
#define	BUTTON_HEIGHT		32		//30
#define	BUTTON_TEXT_X		52		//42
#define	BUTTON_SPACE		208		//((KBD_POS_W-BLUE_X-BLUE_X)/4)


#define	RCU_BLUE_PNG		"/usr/browser/webapps/images/181_3_00_Help1_Blue.png"
#define	RCU_RED_PNG		"/usr/browser/webapps/images/181_3_00_Help2_Red.png"
#define	RCU_GREEN_PNG		"/usr/browser/webapps/images/181_3_00_Help3_Green.png"
#define	RCU_YELLOW_PNG	"/usr/browser/webapps/images/181_3_00_Help4_Yellow.png"
#define	RCU_LEFT_PNG		"/usr/browser/webapps/images/181_1_00_Help5_Left.png"
#define	RCU_RIGHT_PNG		"/usr/browser/webapps/images/181_1_00_Help6_Right.png"
#define	RCU_BACK_PNG		"/usr/browser/webapps/images/181_1_00_Help1_Back.png"

#ifdef	DRAW_KEYBOARD_TITLE
#define	KBD_TITLE_H	46
#else
#define	KBD_TITLE_H	0
#endif
#undef	KBD_POS_H
#define	KBD_POS_H	(KBD_TITLE_H+206)

#define	BLUE_X		20		//10
#define	BLUE_Y		(KBD_TITLE_H+100)
#define	RED_X		(BLUE_X+BUTTON_SPACE)
#define	RED_Y		(BLUE_Y)
#define	GREEN_X		(RED_X+BUTTON_SPACE)
#define	GREEN_Y		(BLUE_Y)
#define	YELLOW_X	(GREEN_X+BUTTON_SPACE)
#define	YELLOW_Y	(BLUE_Y)
#define	LEFT_X		(BLUE_X)
#define	LEFT_Y		(BLUE_Y+46)
#define	RIGHT_X		(LEFT_X+52)
#define	RIGHT_Y		(LEFT_Y)
#define	BACK_X	(LEFT_X+BUTTON_SPACE+BUTTON_SPACE)
#define	BACK_Y	(LEFT_Y)
#define	FONT_OFFSET	12

#define	BTN_COLOR_BLUE		0xFF0000F0
#define	BTN_COLOR_RED		0xFFF00000
#define	BTN_COLOR_GREEN	0xFF00D000
#define	BTN_COLOR_YELLOW	0xFFC3C300
#define	BTN_COLOR_GREY	0xFF303030
#define	LABEL_COLOR		0xFFF0F0F0
#define	LABEL_DISABLE_COLOR		0xFF909090
#define	EDIT_TEXT_COLOR	0xFF454545

#define	EDITTEXT_X			10
#define	EDITTEXT_Y			(KBD_TITLE_H+44)
#define	EDITTEXT_H			(KBD_FONT_SIZE + 12)

#define	OFFSET_KANJI_MARK	(-144)
#define	OFFSET_KANA_MARK	(OFFSET_KANJI_MARK+34)
#define	OFFSET_ALPHABET_MARK	(OFFSET_KANA_MARK+34)
#define	OFFSET_NUMBER_MARK	(OFFSET_ALPHABET_MARK+34)
#define	MODE_MARK_Y		(KBD_TITLE_H+10)

#define	OFFSET_PADDING_SPACE_X	10
#define	OFFSET_PADDING_SPACE_Y	6
#define	SAPERATE_LINE_COLOR		0xFF9F9F9F
#define	CANDIDATE_TEXT_COLOR	0xFF575757

#define	NUM_DISPLAY_CANDIDATE	6
#define	SCROLLER_THICKNESS			12

/* 스크롤 바를 그린다. */
static void		local_drawScrollBar( int x, int y, int w, int h, int range, int start, int size)
{
	ONDK_Rect_t	_rc;

	if (start >= range-size)			// 스크롤 범위가 오버하지 않도록 조정.
		start = range-size;

	// 테두리 그리고
	_rc.x = x;		_rc.y = y;		_rc.w = SCROLLER_THICKNESS;		_rc.h = h;
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_DARK );
	h-=2;
	// 전체 바탕 그리고
	_rc.x = x+1;		_rc.y = y+1;		_rc.w = SCROLLER_THICKNESS-2;		_rc.h = h;
	ONDK_DRAW_FillGradationRect(gKBDSurface, _rc, FALSE, KBD_INPUTBOX_BGCOL_DARK, KBD_INPUTBOX_BGCOL_BRIGHT );
	// 스크롤 위치/크기 만큼 손잡이의 테두리를 그림.
	_rc.x = x+1;	_rc.y = y+1+(start*h/range);		_rc.w = SCROLLER_THICKNESS-2;		_rc.h = (size*h/range)+1;
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_DARK );
	// 스크롤 위치에 맞게 손잡이(?) 그린다.
	_rc.x = x+2;	_rc.y = y+2+(start*h/range);		_rc.w = SCROLLER_THICKNESS-4;		_rc.h = (size*h/range)-1;
	ONDK_DRAW_FillGradationRect(gKBDSurface, _rc, FALSE, KBD_INPUTBOX_BGCOL_BRIGHT, KBD_INPUTBOX_BGCOL_DARK );
}

/* 후보 한자 List 를 그린다. - 스크롤 기능 포함. */

static  int	start_index = 0;		// 후보 리스트를 표시할 리스트의 첫번째 항목 인덱스.

static void		local_drawCandidateList( int x, int y, int w, int h )
{
	ONDK_Rect_t	_rc;
	int		num_candidate, selected_index, i;
	//int		start, end;
	HUINT8	*candidate_string;

#define	FRAME_THICKNESS	2
	//drawBoxShadow(gKBDSurface, x, y, w, h);
	_rc.x = x;		_rc.y = y;		_rc.w = w;		_rc.h = FRAME_THICKNESS;
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_DARK );			// Window BG
	_rc.y = y+h-FRAME_THICKNESS;
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_DARK );			// Window BG
	_rc.y = y;		_rc.w = FRAME_THICKNESS;		_rc.h = h;
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_DARK );			// Window BG
	_rc.x = x+w-FRAME_THICKNESS;
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_DARK );			// Window BG

	_rc.x = x+FRAME_THICKNESS;		_rc.y = y+FRAME_THICKNESS;
	_rc.w = w-FRAME_THICKNESS-FRAME_THICKNESS;		_rc.h = h-FRAME_THICKNESS-FRAME_THICKNESS;
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_INPUTBOX_BGCOL_BRIGHT );		// Window BG

	num_candidate = KBD_GetCandidateNumber();
	selected_index = KBD_GetSelectedCandidate();

	if (selected_index < start_index )
		start_index = selected_index;
	if (selected_index >= (start_index+NUM_DISPLAY_CANDIDATE) )
		start_index = selected_index-(NUM_DISPLAY_CANDIDATE-1);

	if (num_candidate > NUM_DISPLAY_CANDIDATE)		// 스크롤러가 필요한지 확인하여 그림.
	{	local_drawScrollBar( x+w-SCROLLER_THICKNESS-4, y+4, SCROLLER_THICKNESS, h-8, num_candidate, start_index, NUM_DISPLAY_CANDIDATE);
		w -= SCROLLER_THICKNESS;
	}

	for (i=start_index; i < start_index+NUM_DISPLAY_CANDIDATE; i++)
	{
		if ( i >= num_candidate )
			break;
		candidate_string = KBD_GetCandidatestring(i);
		if (candidate_string != NULL)
		{
	    		ONDK_Rect_t  	_rc;
			ONDK_Color_t	fontColor;

			if ( i != start_index)
			{
				_rc.x = x+FRAME_THICKNESS+OFFSET_PADDING_SPACE_X;
				_rc.y = y+FRAME_THICKNESS+OFFSET_PADDING_SPACE_Y/2+(i-start_index)*(KBD_FONT_SIZE+8);
				_rc.w = w-FRAME_THICKNESS-FRAME_THICKNESS-OFFSET_PADDING_SPACE_X-OFFSET_PADDING_SPACE_X;
				_rc.h = 2;
				ONDK_DRAW_FillRect(gKBDSurface, _rc, SAPERATE_LINE_COLOR );
			}

			if ( i == selected_index)
			{
				_rc.x = x+FRAME_THICKNESS+OFFSET_PADDING_SPACE_X;
				_rc.y = y+FRAME_THICKNESS+OFFSET_PADDING_SPACE_Y/2+3+(i-start_index)*(KBD_FONT_SIZE+8);
				_rc.w = w-FRAME_THICKNESS-FRAME_THICKNESS-OFFSET_PADDING_SPACE_X-OFFSET_PADDING_SPACE_X;
				_rc.h = (KBD_FONT_SIZE+4);
				ONDK_DRAW_FillRect(gKBDSurface, _rc, CANDIDATE_TEXT_COLOR );
				fontColor = 0xFFFFFFFF;
			}
			else
			{
				fontColor = CANDIDATE_TEXT_COLOR;
			}

			ONDK_FONT_DrawString(gKBDSurface, gKBDFont, candidate_string,
					x+FRAME_THICKNESS+OFFSET_PADDING_SPACE_X+OFFSET_PADDING_SPACE_X,
					y+FRAME_THICKNESS+OFFSET_PADDING_SPACE_Y+(i-start_index)*(KBD_FONT_SIZE+8) ,
					fontColor, DSTF_TOPLEFT);
		}
		else
		{
			printf("<'))))>< ....  candidate string is NULL \n");
		}
	}

#undef	FRAME_THICKNESS

}


static	 void		local_drawKeyboardWindow(int x, int y, int w, int h)
{
	ONDK_Rect_t	_rc, _candidate_rect;
	SMS_INPUT_AVAILABLE available_input_mode;
	ONDK_Color_t		mode_color;
#ifdef	DRAW_KEYBOARD_TITLE
	int		title_w, title_h;
#endif
	_candidate_rect.x = x + 100;	_candidate_rect.y = y+EDITTEXT_Y+48;
	_candidate_rect.w = w;		_candidate_rect.h = NUM_DISPLAY_CANDIDATE*(KBD_FONT_SIZE+8)+10;

	if ( KBD_GetIMEMode() !=KBD_CANDIDATE_SELECT_MODE)		// 후보리스트가 있을지 모르니, 지우고 시작 함. (깜박임 생길라나 ? )
	{
		ONDK_SURFACE_SetCopy(gKBDSurface);
		ONDK_DRAW_FillRect(gKBDSurface, _candidate_rect, 0x00000000);
	}

	eraseBoxShadow(gKBDSurface, x, y, w, h);	// 그림자만 지우고 새로 그린다. 안그러면, 그림자가 계속 겹쳐서 점차 짙어 짐.
	drawBoxShadow(gKBDSurface, x, y, w, h);
	_rc.x = x;		_rc.y = y;		_rc.w = w;		_rc.h = h;
	ONDK_DRAW_FillRect(gKBDSurface, _rc, KBD_BG_GRADATION_COLOR_BRIGHT );			// Window BG
	#ifdef	DRAW_KEYBOARD_TITLE
	_rc.x = x+4;		_rc.y = y+4;		_rc.w = w-8;		_rc.h = KBD_TITLE_H;			// Window Title Bar
	ONDK_DRAW_FillRect(gKBDSurface, _rc, BTN_COLOR_GREY);
	ONDK_FONT_GetWidthMultiLine( gKBDFont, _stKeyboardInfo.title, 1280, 0, &title_w, &title_h);
	ONDK_FONT_DrawString(gKBDSurface, gKBDFont, _stKeyboardInfo.title, x+(w-title_w)/2, y+12, LABEL_COLOR, DSTF_TOPLEFT);
	#endif

	switch(KBD_GetIMEMode() )
	{
		case	KBD_NUMERIC_MODE:
			_rc.x = x+w+OFFSET_NUMBER_MARK;		_rc.y = y+MODE_MARK_Y+2;		_rc.w = 20+8;		_rc.h = 20+8;
			break;
		case	KBD_ALPHABET_MODE:
			_rc.x = x+w+OFFSET_ALPHABET_MARK;	_rc.y = y+MODE_MARK_Y+2;		_rc.w = 20+8;		_rc.h = 20+8;
			break;
		case	KBD_KATAKANA_MODE:
			_rc.x = x+w+OFFSET_KANA_MARK;		_rc.y = y+MODE_MARK_Y+2;		_rc.w = 20+8;		_rc.h = 20+8;
			break;
		//case	KBD_KANJI_MODE:
		//case	KBD_CANDIDATE_SELECT_MODE:
		default:
			_rc.x = x+w+OFFSET_KANJI_MARK;		_rc.y = y+MODE_MARK_Y+2;		_rc.w = 20+8;		_rc.h = 20+8;
			break;
	}
	ONDK_DRAW_FillRect(gKBDSurface, _rc, 0xFF000000 );

	available_input_mode = KBD_GetAvailableIMEMode( );
	if (available_input_mode & KBD_KANJI_AVAILABLE)
		mode_color = LABEL_COLOR;
	else
		mode_color = LABEL_DISABLE_COLOR;
	ONDK_FONT_DrawString(gKBDSurface, gKBDSmallFont, kanji_utf8, x+w+OFFSET_KANJI_MARK+2, y+MODE_MARK_Y+2, mode_color, DSTF_TOPLEFT);
	if (available_input_mode & KBD_KATAKANA_AVAILABLE )
		mode_color = LABEL_COLOR;
	else
		mode_color = LABEL_DISABLE_COLOR;
	ONDK_FONT_DrawString(gKBDSurface, gKBDSmallFont, hankaku_utf8, x+w+OFFSET_KANA_MARK+2, y+MODE_MARK_Y+2, mode_color, DSTF_TOPLEFT);
	if (available_input_mode & KBD_ALPHABET_AVAILABLE )
		mode_color = LABEL_COLOR;
	else
		mode_color = LABEL_DISABLE_COLOR;
	ONDK_FONT_DrawString(gKBDSurface, gKBDSmallFont, eigo_utf8, x+w+OFFSET_ALPHABET_MARK+2, y+MODE_MARK_Y+2, mode_color, DSTF_TOPLEFT);
	if (available_input_mode & KBD_NUMERIC_AVAILABLE )
		mode_color = LABEL_COLOR;
	else
		mode_color = LABEL_DISABLE_COLOR;
	ONDK_FONT_DrawString(gKBDSurface, gKBDSmallFont, suuji_utf8, x+w+OFFSET_NUMBER_MARK+2, y+MODE_MARK_Y+2, mode_color, DSTF_TOPLEFT);

	local_drawInputBoxBG(gKBDSurface, x+EDITTEXT_X, y+EDITTEXT_Y, w-EDITTEXT_X-EDITTEXT_X, EDITTEXT_H  );	//

	// RCU 키  표
	{
		ONDKImageHandle		pngImage;

		pngImage = ONDK_IMG_CreateSelf(RCU_BLUE_PNG);
		if ( pngImage != NULL)
		{	ONDK_IMG_Blit( pngImage, gKBDSurface, x+BLUE_X, y+BLUE_Y );
			ONDK_IMG_Destroy(pngImage);
		}
		else
		{	printf("<'))))>< .... BLUE PNG load failed. !!\n");
		}
		ONDK_FONT_DrawString(gKBDSurface, gKBDFont, modechange_utf8, x+BLUE_X+BUTTON_TEXT_X, y+BLUE_Y, LABEL_COLOR, DSTF_TOPLEFT);
		pngImage = ONDK_IMG_CreateSelf(RCU_RED_PNG);
		if ( pngImage != NULL)
		{	ONDK_IMG_Blit( pngImage, gKBDSurface, x+RED_X, y+RED_Y );
			ONDK_IMG_Destroy(pngImage);
		}
		else
		{	printf("<'))))>< .... RED PNG load failed. !!\n");
		}
		ONDK_FONT_DrawString(gKBDSurface, gKBDFont, convert_utf8, x+RED_X+BUTTON_TEXT_X, y+RED_Y, LABEL_COLOR, DSTF_TOPLEFT);
		pngImage = ONDK_IMG_CreateSelf(RCU_GREEN_PNG);
		if ( pngImage != NULL)
		{	ONDK_IMG_Blit( pngImage, gKBDSurface, x+GREEN_X, y+GREEN_Y );
			ONDK_IMG_Destroy(pngImage);
		}
		else
		{	printf("<'))))>< .... GREEN PNG load failed. !!\n");
		}
		ONDK_FONT_DrawString(gKBDSurface, gKBDFont, delete_utf8, x+GREEN_X+BUTTON_TEXT_X, y+GREEN_Y, LABEL_COLOR, DSTF_TOPLEFT);
		pngImage = ONDK_IMG_CreateSelf(RCU_YELLOW_PNG);
		if ( pngImage != NULL)
		{	ONDK_IMG_Blit( pngImage, gKBDSurface, x+YELLOW_X, y+YELLOW_Y );
			ONDK_IMG_Destroy(pngImage);
		}
		else
		{	printf("<'))))>< .... YELLOW PNG load failed. !!\n");
		}
		ONDK_FONT_DrawString(gKBDSurface, gKBDFont, confirm_utf8, x+YELLOW_X+BUTTON_TEXT_X, y+YELLOW_Y, LABEL_COLOR, DSTF_TOPLEFT);
		pngImage = ONDK_IMG_CreateSelf(RCU_LEFT_PNG);
		if ( pngImage != NULL)
		{	ONDK_IMG_Blit( pngImage, gKBDSurface, x+LEFT_X, y+LEFT_Y );
			ONDK_IMG_Destroy(pngImage);
		}
		else
		{	printf("<'))))>< .... LEFT PNG load failed. !!\n");
		}
		pngImage = ONDK_IMG_CreateSelf(RCU_RIGHT_PNG);
		if ( pngImage != NULL)
		{	ONDK_IMG_Blit( pngImage, gKBDSurface, x+RIGHT_X, y+RIGHT_Y );
			ONDK_IMG_Destroy(pngImage);
		}
		else
		{	printf("<'))))>< .... RIGHT PNG load failed. !!\n");
		}
		ONDK_FONT_DrawString(gKBDSurface, gKBDFont, cursor_utf8, x+RIGHT_X+BUTTON_TEXT_X, y+RIGHT_Y, LABEL_COLOR, DSTF_TOPLEFT);
		pngImage = ONDK_IMG_CreateSelf(RCU_BACK_PNG);
		if ( pngImage != NULL)
		{	ONDK_IMG_Blit( pngImage, gKBDSurface, x+BACK_X, y+BACK_Y );
			ONDK_IMG_Destroy(pngImage);
		}
		else
		{	printf("<'))))>< .... BACK PNG load failed. !!\n");
		}
		ONDK_FONT_DrawString(gKBDSurface, gKBDFont, cancel_utf8, x+BACK_X+BUTTON_TEXT_X+24, y+BACK_Y, LABEL_COLOR, DSTF_TOPLEFT);
	}

	KBD_DisplayEditingString(gKBDSurface, gKBDFont, x+(EDITTEXT_X+6), y+(EDITTEXT_Y+6), w-(EDITTEXT_X+6)-(EDITTEXT_X+6), EDITTEXT_H,  EDIT_TEXT_COLOR, TRUE );

	if ( KBD_GetIMEMode()==KBD_CANDIDATE_SELECT_MODE)
	{	int	i, max_str_width = 0;
		int	num_candidate = KBD_GetCandidateNumber();
		HUINT8	*candidate_string;

		for (i=0; i < num_candidate; i++)
		{
			candidate_string = KBD_GetCandidatestring(i);
			if (candidate_string != NULL)
			{	int	width;
				width = ONDK_FONT_GetStringWidth( gKBDFont, candidate_string);
				if (width > max_str_width)
					max_str_width = width;
			}
		}
		_candidate_rect.w = max_str_width + 48;
		local_drawCandidateList( _candidate_rect.x, _candidate_rect.y, _candidate_rect.w, _candidate_rect.h );
	}

	ONDK_WINDOW_Update(gKBDWnd, NULL);
	_stKeyboardInfo.isUpdated = FALSE;
}



HInputRetType_e	local_KeyInput(int keycode, int type)
{
	HInputRetType_e	keyUsed = eHINPUTRET_NONE;

	keyUsed = KBD_KeitaiInput(keycode, type);
	_stKeyboardInfo.isUpdated = TRUE;
	return	keyUsed;		/* Pass to other process.*/
}


/* 입력을 종료 시킬 때, 취소(모도루 키로 종료) 하면, apply_change 에 FALSE 로 전달 되므로,
	문자열을 적용하지 않도록 한다. */
void		local_callback_InputDone(HUINT8 *complete_string, HBOOL apply_change)
{
	if (apply_change)
	{
		HAPI_BroadcastSignal("signal:onOSKResult", 0, "s", complete_string);
	}
	else
	{
		HAPI_BroadcastSignal("signal:onOSKCancel", 0, "s", "");
	}

	/* 키보드를 닫는다. */
	ONDK_WINDOW_SetShow(gKBDWnd, FALSE);
	_stKeyboardInfo.show = FALSE;
	KBD_SetStringToEdit( sLocalBufferUTF8 );
//	KBD_SetEditWindow(gKBDFont, KBD_POS_X, KBD_POS_Y, KBD_POS_W, KBD_POS_H  );
	//KBD_SetInputPosition( 0 );
	KBD_SetBufferMaxLength( 96 );//MAX_INPUT_LENGTH );
	KBD_SetAvailableInputMode(KBD_ALL_AVAILABLE );
}


/**
 * AppKeyboard Key ondk callback
 *
 */

static ONDK_Result_t ONDK_KEYBOARD_KeyNotifyCallback(HUINT32 ucKeyCode, ONDK_KEY_PressMode_t pressMode, ONDK_KEY_Device_t device, void *keyTypeData)
{
	HInputRetType_e	consumed = eHINPUTRET_NONE;
	ONDK_Print("[%s:%d]\n", __FUNCTION__, __LINE__);

#ifdef	MAKE_KEYUP_PAIR
		if ( pressMode == eKeyPress_UP) // KEYUP은	KEYDOWN 과 PAIR 로 처리하기 위한.
		{
			if (is_keydown_states(ucKeyCode) )		// 이전에 KEY DOWN 상태라면,
			{	consumed = eHINPUTRET_CONSUMED; // 짝을 맞추어 처리해 버림.
			}
			else
				consumed = eHINPUTRET_PASS; 	// 짝이 안 맞으면, 통과.
			forget_keydown(ucKeyCode);
		}
#else
#endif

	if (_stKeyboardInfo.show)
		consumed = local_KeyInput(ucKeyCode, pressMode );

#ifdef	MAKE_KEYUP_PAIR
	if ( pressMode != eKeyPress_UP)	// KEYUP은  KEYDOWN 과 PAIR 로 처리하기 위한.
	{	if(consumed == eHINPUTRET_CONSUMED)
		{	remember_keydown(ucKeyCode);
		}
	}
#else
#endif


	if(consumed == eHINPUTRET_CONSUMED)
	{
		ONDK_Print("[%s:%d] eHEVENT_CONSUMED\n", __FUNCTION__, __LINE__);
		return ONDK_MESSAGE_CONSUMED;
	}
	ONDK_Print("[%s:%d]  ONDK_MESSAGE_NONCONSUMED\n", __FUNCTION__, __LINE__);
	return ONDK_MESSAGE_NONCONSUMED;
}


/**************************************************************
	public functions
 **************************************************************/

#if 	1		// Original Keyboard Application -
int		main(int argc, char **argv)
{
    #if defined(CONFIG_ENHANCED_SECURITY)
	dropUserPrevilegeAndChroot();
    #endif

	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

	VK_Init();

	ONDK_Init(argc, argv);

	//	Initialize Internal Structure
	HxSTD_MemSet(&_stKeyboardInfo, 0, sizeof(KEYBDData_t));

	//	Initialize OAPI and APPKIT
	HAPI_ConnectSignal("signal:onOSKModeChanged", NULL, onOSKModeChanged);
	HAPI_ConnectSignal("signal:onOSKCharModeChanged", NULL, onOSKCharModeChanged);
	HAPI_ConnectSignal("signal:onOSKDefaultString", NULL, onOSKDefaultString);
	HAPI_ConnectSignal("signal:onOSKInitPosition", NULL, onOSKInitPosition);
	HAPI_ConnectSignal("signal:onOSKMaxLengthChanged", NULL, onOSKMaxLengthChanged);
	HAPI_ConnectSignal("signal:onOSKShow", NULL, onOSKShow);
	HAPI_ConnectSignal("signal:onOSKDestroy", NULL, onOSKDestroy);

#if defined(TEST_CASE)
	HAPI_SetWindowStyle(eAWINDOW_KEYBOARD, eHWS_WindowInputAlways, FALSE);
#endif
	makeKeyboardWindow();
	gKBDFont = ONDK_FONT_CreateGreyFont( JAPANESE_FONT_PATH, KBD_FONT_SIZE, KBD_FONT_SIZE);
	gKBDSmallFont = ONDK_FONT_CreateGreyFont( JAPANESE_FONT_PATH, 24, 24);
	makeshadowBitmap();
	KBD_RegisterCallBack(local_callback_InputDone);		// 키 입력이 완료되면 처리할 함수를 등록.
	KBD_SetStringToEdit( sLocalBufferUTF8 );
	KBD_SetEditWindow(gKBDFont, KBD_POS_X, KBD_POS_Y, KBD_POS_W, KBD_POS_H  );
	KBD_SetAvailableInputMode(KBD_ALL_AVAILABLE);

	_stKeyboardInfo.show = FALSE;

	m_KeyID = ONDK_KEY_RegisterKeyCallback(gKBDWnd, ONDK_KEYBOARD_KeyNotifyCallback);

#ifdef	CONFIG_DEBUG
	APKeyboard_CMD_Register(COMMAND_NAME);
#endif

	while (1)
	{
		VK_TASK_Sleep(10);
		if ( (_stKeyboardInfo.show ) && (_stKeyboardInfo.isUpdated == TRUE) )
		{
			local_drawKeyboardWindow( KBD_POS_X, KBD_POS_Y, KBD_POS_W, KBD_POS_H );
		}
	}

	return 0;
}

#elif	1
		// df_qna.c - DirectFB QnA sample source with Broadcom.

#include <directfb.h>
#include <stdio.h>

#define	TEST_8BPP
//#define JAPANESE_FONT_PATH 	"/usr/local/share/directfb-1.4.15/fonts/sfd/FreeSerif.ttf"

#define DFBCHECK(x...)                                                     \
               err = x;                                                    \
               if (err != DFB_OK) {                                        \
                    fprintf( stdout, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
                    DirectFBErrorFatal( #x, err );                         \
               }


int     main(int argc, char **argv)
{
    DFBResult       err;
    IDirectFB           *m_dfb = NULL;
    IDirectFBDisplayLayer   *m_layer;
    DFBWindowDescription    wdesc;
    IDirectFBWindow     *dfbWindow;
    IDirectFBSurface    *windowSurface;
    IDirectFBFont           *pFont;
    DFBFontDescription      sFontDesc;
    int     windowsurface_w, windowsurface_h;


    if (m_dfb == NULL)
    {
        DirectFBInit(&argc, &argv);
        /* Set DirectFB window manager to SawMan */
        DirectFBSetOption("wm", "sawman");

        DFBCHECK(DirectFBCreate(&m_dfb));
        DFBCHECK(m_dfb->SetCooperativeLevel(m_dfb, DFSCL_NORMAL));

        DFBCHECK(m_dfb->GetDisplayLayer(m_dfb, DLID_PRIMARY, &m_layer));
        DFBCHECK(m_layer->SetCooperativeLevel(m_layer, DLSCL_ADMINISTRATIVE));

//	DFBCHECK(m_layer->SetScreenRectangle (m_layer, 0, 30, 960, 240));		이거 안 먹음.
//	DFBCHECK(m_layer->SetScreenLocation (m_layer, 0, 30, 960, 240));			이것도 안 먹음.

    }

    wdesc.flags = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_OPTIONS | DWDESC_PIXELFORMAT;
    wdesc.caps  = DWCAPS_ALPHACHANNEL;

    //wdesc.options = DWOP_ALPHACHANNEL ;
    wdesc.options = DWOP_ALPHACHANNEL| DWOP_SCALE;

    wdesc.posx  = 0;
    wdesc.posy  = 0;
    wdesc.width  = 960;
    wdesc.height = 540;
	#ifdef	TEST_8BPP
    wdesc.pixelformat = DSPF_LUT8;
	#else
    wdesc.pixelformat = DSPF_ARGB;
	#endif
    DFBCHECK(m_layer->CreateWindow(m_layer, &wdesc, &dfbWindow));
    DFBCHECK(dfbWindow->GetSurface(dfbWindow, &windowSurface));
    DFBCHECK(windowSurface->Clear(windowSurface, 0, 0, 0, 0));
    DFBCHECK(dfbWindow->SetOpacity(dfbWindow, 0xff));
    DFBCHECK(windowSurface->Flip(windowSurface, NULL, DSFLIP_BLIT));
    DFBCHECK (windowSurface->GetSize (windowSurface, &windowsurface_w, &windowsurface_h));

	/* Font Check */
	sFontDesc.flags = (DFBFontDescriptionFlags)(DFDESC_HEIGHT | DFDESC_WIDTH | DFDESC_ATTRIBUTES);
	sFontDesc.attributes = (DFBFontAttributes)(DFFA_BOLD | DFFA_ITALIC);
	sFontDesc.height = 72;
	sFontDesc.width = 72;
	DFBCHECK(m_dfb->CreateFont(m_dfb, JAPANESE_FONT_PATH, &sFontDesc, &pFont));


    while (1)
    {
	#ifdef	TEST_8BPP
	DFBCHECK(windowSurface->SetColorIndex(windowSurface, 0x23));
	#else
	DFBCHECK(windowSurface->SetColor(windowSurface, 0xFF, 0xFF, 0x00, 0xFF));
	#endif
	DFBCHECK(windowSurface->FillTriangle(windowSurface, 100, 100, 340, 180, 220, 360 ));

	DFBCHECK(windowSurface->SetFont(windowSurface, pFont));
	#ifdef	TEST_8BPP
	DFBCHECK(windowSurface->SetColorIndex(windowSurface, 0x36));
	#else
	DFBCHECK(windowSurface->SetColor(windowSurface, 0xff, 0x00, 0xff, 0xff));
	#endif
	DFBCHECK(windowSurface->DrawString(windowSurface, "Window Surface", 14, 340, 100, DSTF_TOPCENTER) );
	DFBCHECK(dfbWindow->Resize(dfbWindow, 1280, 720));

	windowSurface->Flip(windowSurface, NULL, 0);


	sleep(1000);
    }

    return 0;
}

#else	// DFB 의 최종 OSD 에 대해서 Aspect Ratio 조정하는 것을 테스트.
		# error  Undefined main().
#endif

