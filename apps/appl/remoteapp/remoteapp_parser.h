/**************************************************************
 *	@file		remoteapp_parser.h
 *	RemoteApp Server UDP Socket API for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/17
 *	@author			humax
 *	@brief			Remote App Parser API Header
 ***************************************************************/

#ifndef __REMOTEAPP_PARSER_H__

#define __REMOTEAPP_PARSER_H__

#define RCU_DATA_KEYCODE_LEN	8
#define TOUCH_PAD_DATA_LEN	16
#define REMOTE_RCU_ID_LEN	8
#define REMOTE_RCU_KEY_TYPE_LEN	8
#define REMOTE_RCU_KEY_DATA_LEN	1024 //24

#define REMOTE_RCU_ID	"hmxra"
#define KEY_TYPE_RCU	"rcu"
#define KEY_TYPE_TOUCH	"touch"
#define KEY_TYPE_KEY	"key"

#define KEY_NAME_ENTER		"ENTER"
#define KEY_VALUE_ENTER		66
#define KEY_NAME_TAB		"TAB"
#define KEY_VALUE_TAB		61
#define KEY_NAME_DEL		"DEL"
#define KEY_VALUE_DEL		67

typedef enum
{
	eRemote_RCU					= 0x00000001,
	eRemote_Keyboard_Char 		= 0x00000002,
	eRemote_Keyboard_Code 		= 0x00000004,
	eRemote_Keyboard_String		= 0x00000008,
	eRemote_Max					= 0x00000010,
} RemoteData_e;

typedef struct {
	char keycode[RCU_DATA_KEYCODE_LEN]; 	/* humax rcu code like "0020" -> eRcuKeyCode_RED (0x0020) */
} rcu_data;

typedef struct {
	char x_comma_y[TOUCH_PAD_DATA_LEN];	/* relative x and y coordinate like "1020,710" */
} touchpad_data;

typedef struct {
	char inputkey[REMOTE_RCU_KEY_DATA_LEN];		/* input key ascii value : A=65 */
} keyboard_data;

typedef struct TAG_RemoteRCU_Rcu {
	char id[REMOTE_RCU_ID_LEN];		/* id : hmxra */
	char key_type[REMOTE_RCU_KEY_TYPE_LEN];	/* string : keyboard-"key", touch-"touch", rcu-"rcu" */

	union {
		rcu_data	stRcuData;
		touchpad_data	stTouchPadData;
		keyboard_data	stKeyboardData;
		char		reserved[24];
	} data;
} RemoteRCU_Rcu_t;

HERROR	remoteapp_parser(RemoteRCU_Rcu_t *RecvData, void** pvResult, RemoteData_e *peResultType);

#endif
