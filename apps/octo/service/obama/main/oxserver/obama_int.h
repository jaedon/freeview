
#ifndef	__OBAMA_INT_H__
#define	__OBAMA_INT_H__

#include "htype.h"

#ifdef  __cplusplus
extern "C" {
#endif


//#define	OBAMA_SOCKET		"/tmp/.gaka_signal.tmp"
#define	OBAMA_SIGNATURE		"sib"

#if defined(CONFIG_PROD_OS_EMULATOR)
#define	CRYPT_BUFFER_SIZE	(1*1024*1024)
#else
#define	CRYPT_BUFFER_SIZE	((getpagesize() * 3) * 12) // 12k 배수로 요청
#endif

typedef enum
{
	/**
	 * Request  --> [i:OBAMA_CMD_REQUEST, i:0, s:method, b:param] + request_id
	 * Call     --> [i:OBAMA_CMD_REQUEST, i:0, s:method, b:param]
	 */
	OBAMA_CMD_REQUEST,
	/**
	 * Response <-- [i:OBAMA_CMD_RESPONSE, i:err_code, a:[]] + request_id
	 * Callback <-- [i:OBAMA_CMD_RESPONSE, i:err_cdoe, s:name, a:[]]
	 * Error    <-- [i:OBAMA_CMD_RESPONSE, i:err_code, s:msg, s:detail]
	 */
	OBAMA_CMD_RESPONSE,
	/**
	 * Register --> [i:OBAMA_CMD_CALLBAC, i:+1, s:name]
	 * Unegister--> [i:OBAMA_CMD_CALLBAC, i:-1, s:name]
	 */
	OBAMA_CMD_CALLBACK
} OBAMA_Cmd_e;

#define	ERROR_IF(expr, err)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return (err);}}while(0)
#define	ERROR_IF_VOID(expr)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return;}}while(0)

#ifdef	CONFIG_DEBUG
#define	OAPI_LOGGING	1	// 0: oapi, 1: obama
#endif

#ifdef	OAPI_LOGGING
typedef struct
{
	HCHAR		cmd[4];
	HUINT32		time;
	HUINT32		requestId;
	HUINT32		size;
} OAPI_Log_t;

static void		oapi_Logging (FILE *fp, HCHAR cmd, const void *data, HUINT32 n, HUINT32 reqid)
{
	OAPI_Log_t	log;

	if (fp == NULL)
		return;

	memset(&log, 0, sizeof(OAPI_Log_t));
	log.cmd[0]    = cmd;
	log.time      = HLIB_STD_GetSystemTime();
	log.requestId = reqid;
	log.size      = n;

	fwrite(&log, sizeof(OAPI_Log_t), 1, fp);
	fwrite(data, 1, n, fp);
}
#endif

#ifdef  __cplusplus
}
#endif

#endif /* __OBAMA_INT_H__ */

