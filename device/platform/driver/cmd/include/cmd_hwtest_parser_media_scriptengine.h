#if defined(CONFIG_VK_STDLIB)
#else
#include<stdio.h>
#include<malloc.h>
#include<string.h>
#endif

#define CMD_MEDIA_TAG_TEST "#test"
#define CMD_MEDIA_TAG_TESTEND "#test-end"
#define CMD_MEDIA_TAG_ASSET "#asset"
#define CMD_MEDIA_TAG_ASSETEND "#asset-end"
#define CMD_MEDIA_TAG_COMMAND "#command"
#define CMD_MEDIA_TAG_COMMANDEND "#command-end"
#define CMD_MEDIA_TAG_CREATE "#create"
#define CMD_MEDIA_TAG_STARTBUF "#startbuf"
#define CMD_MEDIA_TAG_START "#start"
#define CMD_MEDIA_TAG_STOP "#stop"
#define CMD_MEDIA_TAG_STOPBUF "#stopbuf"
#define CMD_MEDIA_TAG_DESTROY "#destroy"
#define CMD_MEDIA_TAG_SEEK "#seek"
#define CMD_MEDIA_TAG_TRICK "#trick"
#define CMD_MEDIA_TAG_DELAY "#delay"
#define CMD_MEDIA_TAG_LOOP "#loop"
#define CMD_MEDIA_TAG_LOOPEND "#loop-end"
#define CMD_MEDIA_TAG_WAIT "#wait"
#define CMD_MEDIA_TAG_LOG "#log"
#define CMD_MEDIA_TAG_SETAUDIO "#setaudio"

#define CMD_MEDIA_ATTR_ID "id"
#define CMD_MEDIA_ATTR_NUM "num"
#define CMD_MEDIA_ATTR_TIME "time"
#define CMD_MEDIA_ATTR_ORIGIN "origin"
#define CMD_MEDIA_ATTR_SPEED "speed"

#define CMD_MEDIA_VALUE_START "start"
#define CMD_MEDIA_VALUE_FWD "fwd"
#define CMD_MEDIA_VALUE_BWD "bwd"
#define CMD_MEDIA_VALUE_END "end"
#define CMD_MEDIA_VALUE_RAND "rand"

typedef enum
{
	CMD_MEDIA_ERR_OK=0,
	CMD_MEDIA_ERR_FAIL,
	CMD_MEDIA_ERR_EOF,
	CMD_MEDIA_ERR_PARSE,
	CMD_MEDIA_ERR_SYNTAX,
} CMD_MEDIA_ERR_e;
typedef enum
{
	CMD_MEDIA_SEEKDIR_START = 1,
	CMD_MEDIA_SEEKDIR_CURFW,
	CMD_MEDIA_SEEKDIR_CURBW,
	CMD_MEDIA_SEEKDIR_END,
	CMD_MEDIA_SEEKDIR_RAND,
} CMD_MEDIA_SEEKDIR_e;

typedef enum
{
	CMD_MEDIA_COM_TEST = 0,
	CMD_MEDIA_COM_TESTEND,
	CMD_MEDIA_COM_ASSET,
	CMD_MEDIA_COM_ASSETEND,
	CMD_MEDIA_COM_COMMAND,
	CMD_MEDIA_COM_COMMANDEND,
	CMD_MEDIA_COM_CREATE,
	CMD_MEDIA_COM_STARTBUF,
	CMD_MEDIA_COM_START,
	CMD_MEDIA_COM_STOP,
	CMD_MEDIA_COM_STOPBUF,
	CMD_MEDIA_COM_DESTROY,
	CMD_MEDIA_COM_SEEK,
	CMD_MEDIA_COM_TRICK,
	CMD_MEDIA_COM_DELAY,
	CMD_MEDIA_COM_LOOP,
	CMD_MEDIA_COM_LOOPEND,
	CMD_MEDIA_COM_WAIT,
	CMD_MEDIA_COM_LOG,
	CMD_MEDIA_COM_SETAUDIO,
	/* do not touch below */
	CMD_MEDIA_COM_URL,
	CMD_MEDIA_COM_MAX,
} CMD_MEDIA_COM_e;


typedef struct CMD_MEDIA_LINE_s
{
	CMD_MEDIA_COM_e eCmd;
	unsigned int id; /* for id */
	int param1; /* param */
	unsigned long long param2; /* param */
	unsigned long long param3; /* param */
	unsigned char* param4; /* url */
} CMD_MEDIA_LINE_t;

typedef struct CMD_MEDIA_TAG_TABLE_s
{
	unsigned char cTagName[32];
	unsigned int unValue;
} CMD_MEDIA_TAG_TABLE_t;

typedef struct CMD_MEDIA_ScriptParser_s
{
	short bInit;
	char *pBuf;
	char *p;
	char *pLoop;
	unsigned int unBufSize;
	unsigned int unLeftSize;
	unsigned int unLoopStartOffset;
	unsigned int unLoopCount;
	short bAssetOpened;
	short bCmdOpened;
	short bTestOpened;
	short bLoopOpened;
} CMD_MEDIA_ScriptParser_t;

CMD_MEDIA_ERR_e CMD_MEDIA_Script_Init(char *pBuf,unsigned int unSize);
CMD_MEDIA_ERR_e CMD_MEDIA_Script_Uninit();
CMD_MEDIA_ERR_e CMD_MEDIA_Script_GetLine(CMD_MEDIA_LINE_t *pData);

