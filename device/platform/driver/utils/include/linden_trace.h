/**
 * linden_trace.h
*/

/**
 * @defgroup		TRACE TRACE : Trace module
 * @author		Jin-hong Kim
 * @note			Trace APIs
 * @brief			Define TRACE APIs
 * @file 			linden_trace.h
*/


#include "htype.h"

#ifndef __LINDEN_TRACE_H__
#define __LINDEN_TRACE_H__

#if defined(CONFIG_DEBUG)
#define DEBUG_MONITOR
#endif

#ifdef DEBUG_MONITOR
#define __LINDEN_TRACE_ON__
#else
#undef __LINDEN_TRACE_ON__
#endif

#define LINDEN_TRACE_FG_LIGHT_RED             	"\033[1;31;40m"		/* ERROR 		: »¡°­ */
#define LINDEN_TRACE_FG_GREEN             		"\033[1;32;40m"		/* VALUE 		: ÃÊ·Ï */
#define LINDEN_TRACE_FG_YELLOW             		"\033[1;33;40m"		/* WARNING 		: ³ë¶û */
#define LINDEN_TRACE_FG_BLUE             		"\033[1;34;40m"		/* NONE & ALL 	: ÆÄ¶û */
#define LINDEN_TRACE_FG_MAGENTA             	"\033[1;35;40m"		/* INFO 		: ºÐÈ« */
#define LINDEN_TRACE_FG_CYAN             		"\033[1;36;40m"		/* ALL 			: Ã»·Ï */
#define LINDEN_TRACE_FG_WHITE             		"\033[1;37;40m"
#define LINDEN_TRACE_FG_NORMAL                	"\033[00m"

/*
 *	Trace Module Definition
 */
#define TRACE_MODULE_ALL			0x00000000

#define TRACE_MODULE_MAX_CATEGORY		17

#define TRACE_CATEGORY_1			0x00000001
#define TRACE_CATEGORY_2			0x00000002
#define TRACE_CATEGORY_3			0x00000004
#define TRACE_CATEGORY_4			0x00000008
#define TRACE_CATEGORY_5			0x00000010
#define TRACE_CATEGORY_6			0x00000020
#define TRACE_CATEGORY_7			0x00000040
#define TRACE_CATEGORY_8			0x00000080
#define TRACE_CATEGORY_9			0x00000100
#define TRACE_CATEGORY_10			0x00000200
#define TRACE_CATEGORY_11			0x00000400
#define TRACE_CATEGORY_12			0x00000800
#define TRACE_CATEGORY_13			0x00001000
#define TRACE_CATEGORY_14			0x00002000
#define TRACE_CATEGORY_15			0x00004000
#define TRACE_CATEGORY_16			0x00008000
#define TRACE_CATEGORY_17			0x00010000



/* Virtual Kernel (VK) */
#define TRACE_MODULE_VK_BASE			0x00010000
#define 	TRACE_MODULE_VK				( TRACE_MODULE_VK_BASE + TRACE_CATEGORY_1 )
#define 	TRACE_MODULE_VK_CLOCK		( TRACE_MODULE_VK_BASE + TRACE_CATEGORY_2 )
#define 	TRACE_MODULE_VK_EVENT		( TRACE_MODULE_VK_BASE + TRACE_CATEGORY_3 )
#define 	TRACE_MODULE_VK_MEM			( TRACE_MODULE_VK_BASE + TRACE_CATEGORY_4 )
#define 	TRACE_MODULE_VK_MSG			( TRACE_MODULE_VK_BASE + TRACE_CATEGORY_5 )
#define 	TRACE_MODULE_VK_SEM			( TRACE_MODULE_VK_BASE + TRACE_CATEGORY_6 )
#define 	TRACE_MODULE_VK_TASK		( TRACE_MODULE_VK_BASE + TRACE_CATEGORY_7 )
#define 	TRACE_MODULE_VK_TIMER		( TRACE_MODULE_VK_BASE + TRACE_CATEGORY_8 )


/* Driver Interface (DI) */
#define TRACE_MODULE_DI_BASE			0x00020000
#define 	TRACE_MODULE_DI_AUDIO		( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_1 )
#define 	TRACE_MODULE_DI_CHANNEL		( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_2 )
#define 	TRACE_MODULE_DI_CI			( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_3 )
#define 	TRACE_MODULE_DI_DEMUX		( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_4 )
#define 	TRACE_MODULE_DI_EEPROM		( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_5 )
#define 	TRACE_MODULE_DI_FAN			( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_6 )
#define 	TRACE_MODULE_DI_FLASH		( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_7 )
#define 	TRACE_MODULE_DI_HDMI		( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_8 )
#define 	TRACE_MODULE_DI_KEY			( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_9 )
#define 	TRACE_MODULE_DI_LED			( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_10 )
#define 	TRACE_MODULE_DI_OSD			( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_11 )
#define 	TRACE_MODULE_DI_PVR			( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_12 )
#define 	TRACE_MODULE_DI_SCART		( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_13 )
#define 	TRACE_MODULE_DI_SC			( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_14 )
#define 	TRACE_MODULE_DI_VIDEO		( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_15 )
#define 	TRACE_MODULE_DI_INIT		( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_16 )
#define 	TRACE_MODULE_DI_FS			( TRACE_MODULE_DI_BASE + TRACE_CATEGORY_17 )

/* MAX 16 category~! */

/* DRV */
#define TRACE_MODULE_DRV_BASE			0x00040000
#define 	TRACE_MODULE_DRV_GPIO		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_1 )
#define 	TRACE_MODULE_DRV_HDMI		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_2 )
#define 	TRACE_MODULE_DRV_I2C		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_3 )
#define 	TRACE_MODULE_DRV_KEYPAD		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_4 )
#define 	TRACE_MODULE_DRV_LNB		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_5 )
#define 	TRACE_MODULE_DRV_MICOM		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_6 )
#define 	TRACE_MODULE_DRV_PWM		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_7 )
#define 	TRACE_MODULE_DRV_REMOCON	( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_8 )
#define 	TRACE_MODULE_DRV_SCART		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_9 )
#define 	TRACE_MODULE_DRV_UART		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_10 )
#define 	TRACE_MODULE_DRV_MISC		( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_11 )
#define 	TRACE_MODULE_DRV_HOTPLUG	( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_12 )
#define 	TRACE_MODULE_DRV_SYNC	( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_13 )
#define 	TRACE_MODULE_DRV_DRM	( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_14 )
#define 	TRACE_MODULE_DRV_CSD	( TRACE_MODULE_DRV_BASE + TRACE_CATEGORY_15 )


/* Monitor (MON)*/
#define TRACE_MODULE_MON_BASE			0x00080000
#define 	TRACE_MODULE_MON			( TRACE_MODULE_MON_BASE + TRACE_CATEGORY_1 )

/* undefined modules */
#define TRACE_MODULE_A_BASE			0x00100000
#define TRACE_MODULE_B_BASE			0x00200000
#define TRACE_MODULE_C_BASE			0x00400000
#define TRACE_MODULE_D_BASE			0x00800000
#define TRACE_MODULE_E_BASE			0x01000000
#define TRACE_MODULE_F_BASE			0x02000000
#define TRACE_MODULE_G_BASE			0x04000000
#define TRACE_MODULE_H_BASE			0x08000000
#define TRACE_MODULE_I_BASE			0x10000000
#define TRACE_MODULE_J_BASE			0x20000000
#define TRACE_MODULE_K_BASE			0x40000000
#define TRACE_MODULE_L_BASE			0x80000000


#define TRACE_MODULE_NONE				0x00000000


/*
 *	Trace Level
 */
#define TRACE_LEVEL_NONE				0x00
#define TRACE_LEVEL_ERROR				0x01
#define TRACE_LEVEL_FLOW				0x01
#define TRACE_LEVEL_WARNING				0x02
#define TRACE_LEVEL_INFO				0x03
#define TRACE_LEVEL_FUNCTION			0x04
#define TRACE_LEVEL_VALUE				0x05
#define TRACE_LEVEL_ALL					0x06
#define TRACE_LEVEL_FUNCTION_R			0x07	/* don't use this value for setting trace level	*/
#define TRACE_LEVEL_FUNCTION_X		0x08	/* don't use this value for setting trace level	*/

#define CAS_TRACE_LEVEL_NONE			TRACE_LEVEL_NONE
#define CAS_TRACE_LEVEL_ERROR			TRACE_LEVEL_ERROR
#define CAS_TRACE_LEVEL_WARNING		TRACE_LEVEL_WARNING
#define CAS_TRACE_LEVEL_INFO			TRACE_LEVEL_INFO
#define CAS_TRACE_LEVEL_FUNCTION		TRACE_LEVEL_FUNCTION
#define CAS_TRACE_LEVEL_VALUE			TRACE_LEVEL_VALUE
#define CAS_TRACE_LEVEL_ALL			TRACE_LEVEL_ALL
#define CAS_TRACE_LEVEL_FUNCTION_R	TRACE_LEVEL_FUNCTION_R	/* don't use this value for setting trace level	*/
#define CAS_TRACE_LEVEL_FUNCTION_X	TRACE_LEVEL_FUNCTION_X	/* don't use this value for setting trace level	*/


#ifdef __LINDEN_TRACE_ON__
extern void TraceInit( void );
extern const char* TraceGetModuleBaseName( HUINT32 moduleBase );
extern const char* TraceGetModuleName( HUINT32 module );
extern void TraceSetModuleName( HUINT32 module, const char *name );
extern void TraceGetModule( HUINT32 moduleBase, HUINT32 *module );
extern void TraceSetModule( HUINT32 module );
extern void TraceUnsetModule( HUINT32 module );
extern void TraceGetLevel( HUINT32 module, HUINT32 *level );
extern void TraceSetLevel( HUINT32 module, HUINT32 level );
extern void Trace( HUINT32 module, HUINT32 level, char *string, ... );
extern void TraceError( HUINT32 module, char *string, ... );
extern void TraceWarning( HUINT32 module, char *string, ... );
extern void TraceEnter( HUINT32 module, char *string, ... );
extern void TraceExit( HUINT32 module, char *string, ... );
extern void TraceInfo( HUINT32 module, char *string, ... );
extern void TraceValue( HUINT32 module, char *string, ... );
extern void TraceValueArray( HUINT32 module, char *desc, int len, char *value );
extern void TraceFlow( HUINT32 module, char *string, ... );
#else
#define TraceInit()					((void)0)
#define TraceGetModuleBaseName(a)	((void)0)
#define TraceGetModuleName(a)		((void)0)
#define TraceSetModuleName(a,b)		((void)0)
#define TraceGetModule(a,b)			((void)0)
#define TraceSetModule(a)			((void)0)
#define TraceUnsetModule(a)			((void)0)
#define TraceGetLevel(a,b)			((void)0)
#define TraceSetLevel(a,b)			((void)0)
#define Trace(a,...)				((void)0)
#define TraceError(a,...)			((void)0)
#define TraceWarning(a,...)			((void)0)
#define TraceEnter(a,...)			((void)0)
#define TraceExit(a,...)			((void)0)
#define TraceInfo(a,...)			((void)0)
#define TraceValue(a,...)			((void)0)
#define TraceValueArray(a,b,c,d)	((void)0)
#define TraceFlow(a,...)			((void)0)
#endif


#define PrintDebug(...)		TraceInfo(DEBUG_MODULE, __VA_ARGS__)
#define PrintEnter(...) {	TraceEnter(DEBUG_MODULE, LINDEN_TRACE_FG_CYAN "+++[%s] ", __FUNCTION__); \
							TraceEnter(DEBUG_MODULE, LINDEN_TRACE_FG_NORMAL __VA_ARGS__); \
							TraceEnter(DEBUG_MODULE, "\n"); }
#define PrintExit(...)  {	TraceExit(DEBUG_MODULE, LINDEN_TRACE_FG_CYAN "---[%s] ", __FUNCTION__); \
							TraceExit(DEBUG_MODULE, LINDEN_TRACE_FG_NORMAL __VA_ARGS__); \
							TraceExit(DEBUG_MODULE, "\n"); }
#define PrintError(...)	{	TraceError(DEBUG_MODULE, LINDEN_TRACE_FG_LIGHT_RED "[%s].%d : ", __FUNCTION__, __LINE__); \
							TraceError(DEBUG_MODULE, __VA_ARGS__); \
							TraceError(DEBUG_MODULE, LINDEN_TRACE_FG_NORMAL); }


#endif	/* __LINDEN_TRACE_ON__	*/

