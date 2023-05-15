/* $Header:   //BASE/archives/STING_T/include/cmdtool.h-arc   1.0   17 Dec 2004 10:31:46   jhkim1  $ */
/********************************************************************
 * $Workfile:   cmdtool.h  $
 * Project    : HEPVR
 * Author     :
 * Description: Header file for Command Tool (cmd_tool) Environment
 *
 *                              Copyright (c) 2003 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef _CMD_TOOL_H_
#define _CMD_TOOL_H_

/********************************************************************
 Header Files
 ********************************************************************/
#include <htype.h>
#include <vkernel.h>
#include "di_uart.h"

/********************************************************************
 Constant/Macro Definition
 ********************************************************************/
#define	CMD_Printf				DI_UART_Print
/* prompt */
#define	CMD_PROMPT		"[01;36mCMD >[00m "
#define CMD_UART            DI_UART_0
#define CMD_UART_SW_ID      UART_SW_ID_2

/*
ANSI Color Codes

[0m  : ¸ðµç »ö°ú ½ºÅ¸ÀÏ ÃÊ±âÈ­
[1m  : ±½°Ô(bold) / ¹à°Ô
[3m  : ÀÌÅÅ¸¯Ã¼(italic)
[4m  : ¹ØÁÙ(underline)
[7m  : ¹ÝÀü(±ÛÀÚ»ö/¹è°æ»öÀ» °Å²Ù·Î)
[9m  : °¡·ÎÁÙ Ä¡±â
[22m : ±½°Ô(bold) Á¦°Å
[23m : ÀÌÅÅ¸¯Ã¼(italic)Á¦°Å
[24m : ¹ØÁÙ(underline)Á¦°Å
[27m : ¹ÝÀü Á¦°Å
[29m : °¡·ÎÁÙ Á¦°Å
[30m : ±ÛÀÚ»ö:°ËÁ¤
[31m : ±ÛÀÚ»ö:»¡°­
[32m : ±ÛÀÚ»ö:ÃÊ·Ï
[33m : ±ÛÀÚ»ö:³ë¶û
[34m : ±ÛÀÚ»ö:ÆÄ¶û
[35m : ±ÛÀÚ»ö:¸¶Á¨Æ®(ºÐÈ«)
[36m : ±ÛÀÚ»ö:½Ã¾È(Ã»·Ï)
[37m : ±ÛÀÚ»ö:¹é»ö
[39m : ±ÛÀÚ»öÀ¸·Î ±âº»°ªÀ¸·Î
[40m : ¹ÙÅÁ»ö:Èæ»ö
[41m : ¹ÙÅÁ»ö:Àû»ö
[42m : ¹ÙÅÁ»ö:³ì»ö
[43m : ¹ÙÅÁ»ö:È²»ö
[44m : ¹ÙÅÁ»ö:Ã»»ö
[45m : ¹ÙÅÁ»ö:ºÐÈ«»ö
[46m : ¹ÙÅÁ»ö:Ã»·Ï»ö
[47m : ¹ÙÅÁ»ö:Èò»ö
[49m :¹ÙÅÁ»öÀ» ±âº»°ªÀ¸·Î
*/

/* prompt */
#define CMD_PROMPT_DEFAULT				"[01;36mCommand >[00m "


/* for keyword structure */
#define	MAX_CMD_GROUP			64
#define MAX_CMD_WORD			500
#define MAX_WORD_LEN			16
#define MAX_HELP_LEN			500
#define MAX_USAGE_LEN			500
#define MAX_CMD_LINE			256

/* for key-press structure */
#define MAX_CMD_KEY				32

/* for RCU-press structure */
#define MAX_CMD_RCU				32
#define MAX_RCU_PRESS			5

#define MAX_CMD_SUBMODULE		20

/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1

//	Added by iwnam, 2002/5/15, define UART special keys, when use tera term.
#define UART_UP					0x81		// Decimal 128
#define UART_DOWN				0x82
#define UART_RIGHT				0x83
#define UART_LEFT				0x84
#define UART_INS				0x90
#define UART_HOME				0x91
#define UART_PGUP				0x92
#define UART_DEL				0x93
#define UART_END				0x94
#define UART_PGDN				0x95
#define UART_F1					0xa0
#define UART_F2					0xa1
#define UART_F3					0xa2
#define UART_F4					0xa3
#define UART_F5					0xa4
#define UART_F6					0xa6
#define UART_F7					0xa7
#define UART_F8					0xa8
#define UART_F9					0xa9
#define UART_F10				0xaa
#define UART_F11				0xac
#define UART_F12				0xad


#define IS_TABLEITEM(a,  b)		(!VK_strcmp(a, "ALL") || !VK_strcmp(a, b))

#define MAX_INPUT_LENGTH 		2048 /* sql»ç¿ëÀ¸·Î ±ä buf°¡ ÇÊ¿ä. ºÎÁ·ÇÏ¸é ´õ ´Ã¸± °Í */


/********************************************************************
 Structure and Type Definition
 ********************************************************************/
typedef enum
{
	CMDPROC_PRINT_STATE			= 0,
	CMDPROC_SET_STATE,
	CMDPROC_SET_DBGCOLOR,

	CMDPROC_MAX
}CMDPROC_TYPE;


/* command category structure */
typedef	struct tagCmdGroup
{
	int		nGroupId;
	char	szGroupName[MAX_WORD_LEN];
	char	szPromptName[64];
	char	szHelp[MAX_HELP_LEN];

	struct tagCmdGroup	*pstParentGroup;
} CMDGROUP_WORD;

/* command structure using keyword typing */
typedef struct {
	char  	szWord[MAX_WORD_LEN];		/* keyword */
	int  		(*fnToDo)(void *arg);			/* function pointer */
	char  	szHelp[MAX_HELP_LEN];		/* help string */
	char  	szUsage[MAX_USAGE_LEN];	/* usage string */

	//	group
	CMDGROUP_WORD	*pstGroupPtr;
} CMD_WORD, *pCMD_WORD;

/* command structure using key press */
typedef struct {
	unsigned char  	ucKeyCode;					/* key code */
	void  			(*fnToDo)(void);				/* function pointer */
	char  			szHelp[MAX_HELP_LEN];		/* help string */
	char  			szUsage[MAX_USAGE_LEN];	/* usage string */
} CMD_KEY, *pCMD_KEY;

/* command structure using RCU */
typedef struct {
	char  	szRcuCode[MAX_RCU_PRESS];			/* RCU code array: must be null-terminated */
	void  	(*fnToDo)(void);						/* function pointer */
	char  	szHelp[MAX_HELP_LEN];				/* help string */
	char  	szUsage[MAX_USAGE_LEN];			/* usage string */
} CMD_RCU, *pCMD_RCU;


typedef 	void			(*SUB_PROC)(unsigned long, char*, unsigned long) ;

typedef struct CMD_TABLE_ITEM
{
	char							*szMainModule;
	SUB_PROC					pfnSubProc;
	char							*szSubModuleList[MAX_CMD_SUBMODULE];
	struct CMD_TABLE_ITEM		*pNext;
}CMD_TABLE_ITEM_t;



/********************************************************************
 Global Function Declaration
 ********************************************************************/
void 		CMD_Init(void);
void 		CMD_DI_Init(void);
void		CMD_SetCurrentGroup(char *szGroupName);
void		CMD_RegisterGroup(char *szGroupName, char *szPrompt, char *szHelp);
void		CMD_ReleaseRegisterGroup(void);
void		CMD_ListGroup(void);
void		CMD_ListAllCommandWithFullPath(void);

void 		CMD_OCTO_Init(void);
void 		CMD_RegisterWord(int (*fnToDo)(void *arg), char *szWord, char *szHelp, char *szUsage);
void 		CMD_ListWord(void);
void 		CMD_RegisterKey(void (*fnToDo)(void), unsigned char ucKeyCode, char *szHelp, char *szUsage);
void 		CMD_ListKey(void);
void 		CMD_RegisterRcu(void (*fnToDo)(void), char *szRcuCode, char *szHelp, char *szUsage);
void 		CMD_ListRCU(void);
void 		CMD_Parse(char *word);
char *		CMD_NextArg(char **szLine);
void 		CMD_OnKeyPress(const char key);
void 		CMD_OnRcuPress(const char key);
void 		CMD_KeyTask(void *pvParam);
HUINT8 *	CMD_StrToUpper(HUINT8 *str);
HUINT8 *	CMD_StringToUpper(HUINT8 *str, HUINT8 *pucTarget);
void		CMD_RegisterPromptString(void *szPrompt);
void		CMD_ShowPrompt(void);

void		CMD_AddCommandTableItem(CMD_TABLE_ITEM_t *pItem);
int CMD_GetInt(char **szArgList);
int CMD_ReadAsDec(char *pString, int *puiDec_value );
int CMD_ReadAsHex(char *pString, unsigned int *puiHex_value );
int CMD_ReadAsStr(char *pString, char **ppDestStr);
char *CMD_AddString(char *pA, char *pB, int lengthB, int mode);
HUINT8 GetNumberOfArgs(HUINT8* argBuffer);

typedef int (*pfn_cmdhwtest_ProgressbarUI)(char *pStr, unsigned int percent);
typedef int (*pfn_cmdhwtest_InfoUI)(char *pStr);
typedef int (*pfn_cmdhwtest_MsgUI)(char *pStr);
typedef int (*pfn_cmdhwtest_PVRGlobalCallback)(void);
typedef int (*pfn_cmdhwtest_demuxCallback)(void);
typedef int (*pfn_cmdhwtest_TimeoutInfoUI)(char *pStr, int strLength, int timeout_sec);


pfn_cmdhwtest_ProgressbarUI CMD_GetProgressbarUI(void);
pfn_cmdhwtest_InfoUI CMD_GetInfoUI(void);
pfn_cmdhwtest_MsgUI CMD_GetMsgUI(void);
pfn_cmdhwtest_PVRGlobalCallback CMD_GetPVRTestReadyCallback(void);
pfn_cmdhwtest_demuxCallback CMD_GetDemuxCallback(void);
pfn_cmdhwtest_TimeoutInfoUI CMD_GetTimeoutInfoUI(void);


void CMD_RegisterUIFunctions(pfn_cmdhwtest_ProgressbarUI pfnProgress, pfn_cmdhwtest_InfoUI pfnInfo, pfn_cmdhwtest_MsgUI pfnMsg, pfn_cmdhwtest_TimeoutInfoUI pTimeoutInfo);
void CMD_RegisterPVRTestReadyCallback(pfn_cmdhwtest_PVRGlobalCallback callback);
void CMD_RegisterDemuxTestCallback(pfn_cmdhwtest_PVRGlobalCallback callback);
void CMD_2HexChTo1Char(char *pGet, char upper, char lower);
int	CMD_SetKeyPressBootWait(int bSet, int bIsPress);

#endif /* _CMD_TOOL_H_ */





