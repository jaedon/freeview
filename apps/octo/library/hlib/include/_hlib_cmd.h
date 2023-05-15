/*************************************************************
 * @file		_hlib_cmd.h
 * @date		2011/01/17
 * @author	hwang ji yeon(jyhwang@humaxdigital.com)
 * @brief		HUMAX common library header, related with command debugger
 *
 * http://www.humaxdigital.com
 *************************************************************/
#ifndef	__HxCMD_H__
#define	__HxCMD_H__

#define	CMDCONSOLE_ENABLE

#define	HxCMD_OK					0
#define	HxCMD_ERR					1

#define	HxCMD_MAX_WORD_LEN			128
#define	HxCMD_MAX_PROMPT_LEN		128
#define	HxCMD_MAX_HELP_LEN			128
#define	HxCMD_MAX_USAGE_LEN			128
#define	HxCMD_MAX_FILE_LEN			256
#define	HxCMD_MAX_ARGU_LEN			1024
#define	HxCMD_MAX_LOG_LEN			1024

//#define	HxCMD_SIGNAL_CMD			"/tmp/.hxcmd_signal.tmp"

#define	HxCMD_MAX_CMDTYPE_LEN		4
#define	HxCMD_TYPE_REGISTER			"REG"
#define	HxCMD_TYPE_PRINTF			"PRT"
#define	HxCMD_TYPE_REQUEST			"REQ"
#define	HxCMD_TYPE_LOGCAPTURE		"LCT"
#define	HxCMD_TYPE_LOGSYNC			"LCS"
#define	HxCMD_TYPE_LOGBACKUP		"LCB"
#define	HxCMD_TYPE_LOGFILE			"LCL"

#define	HxCMD_TYPE_PUTCHAR			"CHR"

#define C_CODE_RESET				"[00;39;49m"
#define C_CODE_F_BLACK				"[01;30m"		// Fg:Black
#define C_CODE_F_RED				"[01;31m"		// Fg:Red
#define C_CODE_F_GREEN				"[01;32m"		// Fg:Green
#define C_CODE_F_YELLOW 			"[01;33m"		// Fg:Yellow
#define C_CODE_F_BLUE				"[01;34m"		// Fg:Blue
#define C_CODE_F_PURPLE 			"[01;35m"		// Fg:Purple
#define C_CODE_F_CYAN				"[01;36m"		// Fg:Cyan
#define C_CODE_F_YELLOW_B_BLACK 	"[01;33;40m"	// Fg:Yellow, Bg:Black
#define C_CODE_F_WHITE_B_RED		"[01;37;41m"	// Fg:White, Bg:Red
#define C_CODE_F_BLACK_B_GREEN		"[01;30;42m"	// Fg:Black, Bg:Green
#define C_CODE_F_BLACK_B_YELLOW 	"[01;30;43m"	// Fg:black, Bg:Yellow
#define C_CODE_F_WHITE_B_BLUE		"[01;37;44m"	// Fg:White, Bg:Blue
#define C_CODE_F_WHITE_B_PURPLE 	"[01;37;45m"	// Fg:White, Bg:Purple
#define C_CODE_F_BLACK_B_CYAN		"[01;30;46m"	// Fg:Black, Bg:Cyan
#define C_CODE_F_RED_B_BLACK		"[01;31;40m"	// Fg:Red, 	Bg:Black
#define C_CODE_F_CYAN_B_BLACK		"[01;36;40m"	// Fg:cyan, Bg:Black


//	register group and word client to server
typedef	struct
{
	HCHAR	szCmdType[HxCMD_MAX_CMDTYPE_LEN];

	HCHAR	szProcName [HxCMD_MAX_WORD_LEN];
	HCHAR	szGroupName[HxCMD_MAX_WORD_LEN];
	HCHAR	szWordName [HxCMD_MAX_WORD_LEN];
	HCHAR	szHelpName [HxCMD_MAX_HELP_LEN];
} HxCMD_Register_t;

typedef	struct
{
	HCHAR	szCmdType[HxCMD_MAX_CMDTYPE_LEN];

	HCHAR	szGroupName[HxCMD_MAX_WORD_LEN];
	HCHAR	szWordName [HxCMD_MAX_WORD_LEN];
	HCHAR	szArgument [HxCMD_MAX_ARGU_LEN];
} HxCMD_Request_t;

typedef struct
{
	HCHAR	szCmdType[HxCMD_MAX_CMDTYPE_LEN];
	HCHAR	cChar;
} HxCMD_PutChar_t;

typedef	struct
{
	HCHAR	szCmdType[HxCMD_MAX_CMDTYPE_LEN];

	HCHAR	szProcName[HxCMD_MAX_WORD_LEN];
	HUINT32	ulLocalTime;
	HUINT32	ulSystemRunning;
	HCHAR	szLogCapture[HxCMD_MAX_LOG_LEN];
} HxCMD_LogCapture_t;

typedef	struct
{
	HCHAR	szCmdType[HxCMD_MAX_CMDTYPE_LEN];
	HCHAR	szFileName[HxCMD_MAX_FILE_LEN];
	HUINT32	ulSystemTime;
	HCHAR	szLogData[HxCMD_MAX_LOG_LEN];
} HxCMD_LogFile_t;

extern HERROR	HLIB_CMD_RegisterWord(const HCHAR *pszCmdGroup, HINT32 (*fnToDo)(void *arg), const HCHAR *pszCmdWord, const HCHAR *pszCmdHelp, const HCHAR *pszCmdUsage);
extern HCHAR*	HLIB_CMD_GetNextArg(HCHAR **pszLine);
extern HERROR	DBG_HLIB_CMD_Printf(const HCHAR *format, ...);
extern HERROR	DBG_HLIB_CMD_CaptureLog(const HCHAR *format, ...);
#if defined(CONFIG_DEBUG)
#define HLIB_CMD_Printf			DBG_HLIB_CMD_Printf
#define HLIB_CMD_CaptureLog		DBG_HLIB_CMD_CaptureLog
#else
#define HLIB_CMD_Printf			while (0) ((int (*)(const char *, ...)) 0)
#define HLIB_CMD_CaptureLog		while (0) ((int (*)(const char *, ...)) 0)
#endif
extern HERROR	HLIB_CMD_SyncLog(void);
extern HERROR	HLIB_CMD_BackupLog(const HCHAR *pszFilePath);
extern HERROR	HLIB_CMD_FileLog(const HCHAR * file, const HCHAR *format, ...);

extern HERROR	HLIB_CMD_InitGetChar(void);
extern HERROR	HLIB_CMD_RecvGetChar(HCHAR *pChar);

//	below definition will be removed... just for compiling project
//	°¢°¢ÀÇ HxCMD_LOG_XX´Â ÇÊ¿äÇÑ ¸ðµâ¿¡¼­ ¼³Á¤ÀÌ µÇ¾î¾ß ÇÕ´Ï´Ù.
//	¾Æ·¡ ºÎºÐÀº Á¤¸®Àü¿¡¼­¸¸ Çã¿ëÇÕ´Ï´Ù.
typedef HCHAR*	HxCMD_Handle_t;

#endif
