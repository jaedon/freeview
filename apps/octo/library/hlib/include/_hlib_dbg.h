
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


#ifndef	__HLIB_DEBUG__
#define	__HLIB_DEBUG__

#include <signal.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#ifdef	CONFIG_DEBUG
	#define	ENABLE_DEBUG
#endif

#define	USE_VK_SIG
/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct
{
   HUINT32  tid;
   HUINT32  timer;
   HUINT32  starttime; //msec
   HUINT32  settime;    //sec
   HCHAR    caller[128];
   HCHAR    message[1024];
} HLIB_DBG_INFO_t;

typedef void	(*HxDBGFunc_t)		(HLIB_DBG_INFO_t *pinfo, HBOOL bPass);
/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */
#ifdef	ENABLE_DEBUG
#ifdef	USE_VK_SIG
	extern void		HLIB_DBG_Init(void);
#else
	extern void		HLIB_DBG_Init(void (*pfnSignalUserHandler)(int signal, siginfo_t *siginfo, void *context));		//not support sig_handler,use vk_sig.
	extern void		HLIB_DBG_InitSignalHandler(void);
#endif
extern void		HLIB_DBG_InitCoreDump(void);
extern void		HLIB_DBG_InitTrace(void);
extern void		HLIB_DBG_SetupTraceEx(HCHAR *module, HINT32 nTraceNum, HINT32 n, ...);
extern void*	HLIB_DBG_BeginTrace(HCHAR *module, ...);
extern void		HLIB_DBG_EndTrace(HCHAR *module, void *pvTrace);

#define	HLIB_DBG_SetupTrace(module, n, ...)		HLIB_DBG_SetupTraceEx(module, -1, n, __VA_ARGS__)
//	trace function - calc time before and after expr.
#define	HLIB_DBG_Trace(module, expr, ...)				\
	do {												\
		void	*_pv_;									\
		_pv_ = HLIB_DBG_BeginTrace(module, __VA_ARGS__);	\
		expr;											\
		HLIB_DBG_EndTrace(module, _pv_);					\
	} while (0)

#else
	#define		HLIB_DBG_NullFunction					while (0) ((int (*)(const char *, ...)) 0)
	#define		HLIB_DBG_Init(...)						HLIB_DBG_NullFunction
	#define		HLIB_DBG_InitSignalHandler(...)			HLIB_DBG_NullFunction
	#define		HLIB_DBG_InitCoreDump(...)				HLIB_DBG_NullFunction
	#define		HLIB_DBG_InitTrace(...)					HLIB_DBG_NullFunction
	#define		HLIB_DBG_SetupTrace(...)				HLIB_DBG_NullFunction
	#define		HLIB_DBG_SetupTraceEx(...)				HLIB_DBG_NullFunction
	#define		HLIB_DBG_BeginTrace(...)				HLIB_DBG_NullFunction
	#define		HLIB_DBG_EndTrace(...)					HLIB_DBG_NullFunction
	#define		HLIB_DBG_Trace(module, expr, ...)		expr
#endif

/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

#endif /* __HLIB_DEBUG__ */


extern HULONG HLIB_DBG_SetTimeLimitEx(const HCHAR *caller,HUINT32 starttime ,HUINT16 settime, const HCHAR *format, ...);
extern HERROR  HLIB_DBG_CancelTimeLimitEx(HULONG ulTimerId);

/**************************************************************************
* HLIB_DBG_SetupTimeLimit(char,unsigned int , ...)
* @desc
*   Hang-up 상태를 감지하기 위한 타이머 설치, 충분한 긴 시간으로 타이머를 설치해야한다.
*   설치이후 해재 전까지의 루틴 실행 시간에 제한을 건다.
* @param
*   1. expr : 실행 내역
*   2. caller : __FUNCTION__ 이 대부분이겠지만, 예로 obama bus 와 같이 proc 이름을 넣으면 디버깅이 편함
*   3. settime  :  실행 제한 시간.. 충훈하고도 남을 시간
*   4. ...     : 가변인자,  해당 context 에서 저장하고자할 내용을 기록한다. 예로obama bus 의 message 를 적으면 편함
* @return
*
**************************************************************************/
#define HLIB_DBG_SetupTimeLimit(expr,caller,settime,...)                                            \
	do {												                                            \
		HULONG _timerId =0;							                                                \
		_timerId = HLIB_DBG_SetTimeLimitEx(caller,HLIB_STD_GetSystemTick(),settime,__VA_ARGS__);    \
		expr;	                                                                                    \
	    if(_timerId != 0){                                                                          \
    		HLIB_DBG_CancelTimeLimitEx(_timerId);                                                   \
	    }                                                                                           \
	} while(0)

/**************************************************************************
* HLIB_DBG_GetTimerList(HxDBGFunc_t)
* @desc
* 수행된 타이머(10회)들의 기록분과, 아직 수행하지 못한 타이머를 callback 을 통해 반환한다.
* @param
*  1.  함수포인터.
****************************************************************************/
extern HERROR              HLIB_DBG_GetTimerList(HxDBGFunc_t pFunc);

/**************************************************************************
* HLIB_DBG_GetExceedTimer(HxDBGFunc_t)
* @desc
* 수행하지 못한 타이머의 정보를 리턴한다.
* @param
* @return
*  수행하지 못한 등록된 타이머의 정보
****************************************************************************/
extern HLIB_DBG_INFO_t*    HLIB_DBG_GetExceedTimer(void);



