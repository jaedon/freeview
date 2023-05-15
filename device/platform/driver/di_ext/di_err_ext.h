/*********************************************************************

DI Extension module

Module : DI Extension
File name : di_err_ext.h

**********************************************************************/

#ifndef __DI_EXT_ERR_H__
#define __DI_EXT_ERR_H__

typedef enum DI_EXT_Error
{
	DI_EXT_OK= 0,
	DI_EXT_ERR,
	DI_EXT_ERR_INVALID_PARAMETER,
	DI_EXT_ERR_INITIALIZATION,
	DI_EXT_ERR_EVENT_INITIALIZATION,
	DI_EXT_ERR_OUTOFMEMORY,
	DI_EXT_ERR_EXTERNAL_ERROR
}DI_EXT_Error;

#define TRACE_FG_LIGHT_RED			"\033[1;31;40m"		/* ERROR 		: Red	*/
#define TRACE_FG_GREEN				"\033[1;32;40m"		/* VALUE 		: Green	*/
#define TRACE_FG_YELLOW				"\033[1;33;40m"		/* WARNING 		: Yellow	*/
#define TRACE_FG_BLUE				"\033[1;34;40m"		/* NONE & ALL 	: Blue	*/
#define TRACE_FG_MAGENTA			"\033[1;35;40m"		/* INFO 			: Pink 	*/
#define TRACE_FG_CYAN				"\033[1;36;40m"		/* ALL 			: Cyan	*/
#define TRACE_FG_WHITE				"\033[1;37;40m"
#define TRACE_FG_NORMAL				"\033[00m"

#if defined(CONFIG_DEBUG)
	//#define DI_EXT_DEBUG
	#if defined(DI_EXT_DEBUG)
	#define PrintEnter(...) {	printf(TRACE_FG_CYAN "+++[%s] ", __FUNCTION__); \
								printf(TRACE_FG_NORMAL __VA_ARGS__); \
								printf("\n"); }

	#define PrintExit(...)  {	printf(TRACE_FG_CYAN "---[%s] ", __FUNCTION__); \
								printf(TRACE_FG_NORMAL __VA_ARGS__); \
								printf("\n"); }

	#define PrintDebug(...)	{	printf(TRACE_FG_GREEN "[%s].%d : ", __FUNCTION__, __LINE__); \
								printf(__VA_ARGS__); \
								printf(TRACE_FG_NORMAL); }

	#define PrintError(...)	{	printf(TRACE_FG_LIGHT_RED "[%s].%d : ", __FUNCTION__, __LINE__); \
								printf(__VA_ARGS__); \
								printf(TRACE_FG_NORMAL); }
	#else
	#define PrintDebug(...)		while (0) ((int (*)(char *, ...)) 0)
	#define PrintError(...)	{	printf(TRACE_FG_LIGHT_RED "[%s].%d : ", __FUNCTION__, __LINE__); \
								printf(__VA_ARGS__); \
								printf(TRACE_FG_NORMAL); }
	#define PrintEnter(...)		while (0) ((int (*)(char *, ...)) 0)
	#define PrintExit(...)		while (0) ((int (*)(char *, ...)) 0)
	#endif /* DI_EXT_DEBUG */
#else
	#define PrintDebug(...)		while (0) ((int (*)(char *, ...)) 0)
	#define PrintError(...)		while (0) ((int (*)(char *, ...)) 0)
	#define PrintEnter(...)		while (0) ((int (*)(char *, ...)) 0)
	#define PrintExit(...)		while (0) ((int (*)(char *, ...)) 0)
#endif

#endif /* __DI_EXT_ERR_H__ */

