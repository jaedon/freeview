
/**
 * linden_trace.c
*/

/**
 * @defgroup		TRACE TRACE : Trace module
 * @author		Jin-hong Kim
 * @note			Trace APIs
 * @brief			Define TRACE APIs
 * @file 			linden_trace.c
*/


#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <stdarg.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/*#include "b_os_lib.h" */

#include "linden_trace.h"
#include "linden_monitor.h"
#include "linden_net.h"

#include "vkernel.h"
#include "taskdef.h"


#ifdef __LINDEN_TRACE_ON__

#define MAX_TRACE_MODULE_BASE	16

#define MAX_MODULE_BASE			16
#define MAX_CATEGORY				(TRACE_MODULE_MAX_CATEGORY-1)

#define MAX_CATEGORY_NAME_LENGTH				16

//#define __USE_TRACE_CS__


#ifdef __USE_TRACE_CS__
#define CS_START(x)		VK_SEM_Get(x)
#define CS_END(x)		VK_SEM_Release(x)
#else
#define CS_START(x)
#define CS_END(x)
#endif

#if 0
B_MutexHandle	g_MutexHandle;
#endif

unsigned long g_TraceSem = 0;

/*	2006_09_07 linden trace redefined
*/
#define TST_PRINT(...)	/* VK_printf (__VAR_ARGS__) */
#define DBG_TRACE_PRINT(...) /* VK_printf (__VAR_ARGS__) */

#if defined(CONFIG_DEBUG)
#define INFO_PRINT	VK_printf
#else
#define INFO_PRINT(...)	((void)0)
#endif

#define CONST_LINDEN_TRACE_THREAD_NAME_ROOT			"ROOT"
#define CONST_LINDEN_TRACE_THREAD_NAME_UNKNOWN		"UKN"
#define CONST_LINDEN_TRACE_THREAD_NAME_INVALID		"INV"

typedef struct
{
	char				*m_szName;
	unsigned long		m_ulValue;
} stTraceInfo;

typedef struct
{
	unsigned long		m_ulTraceModuleBase;
	stTraceInfo		m_sBaseModuleInfo;		/* module base Info */
	unsigned long		m_ulTraceModules;		/* module base별 trace대상 category들. */
	stTraceInfo		m_saCategoryInfo[TRACE_MODULE_MAX_CATEGORY];	/* category별 level 설정 가능 */
} stTraceInfoEx;

/********************************************************************************
 * external function prototypes
 ********************************************************************************/

/********************************************************************************
 * static function prototypes
 ********************************************************************************/

/********************************************************************************
 * external variables and structures
 ********************************************************************************/

/********************************************************************************
 * global variables and structures
 ********************************************************************************/

/********************************************************************************
 * static variables and structures
 ********************************************************************************/

static int				sTraceInitialized		= FALSE;
static HUINT32			sTraceLevel			= TRACE_LEVEL_NONE;

#if 0
static stTraceInfo		sModuleBaseInfo[MAX_MODULE_BASE];
static stTraceInfo		sModuleInfo[MAX_MODULE_BASE][MAX_CATEGORY];
static int				sModuleCnt = 0;
#endif

static stTraceInfoEx sTraceModuleInfoEx[] =
{
	{ 	TRACE_MODULE_VK_BASE,
		{ "VKBASE", TRACE_LEVEL_NONE },
		TRACE_MODULE_NONE,
		{  { "VK", TRACE_LEVEL_NONE }, { "VKCLOCK", TRACE_LEVEL_NONE }, { "VKEVENT", TRACE_LEVEL_NONE }, { "VKMEM", TRACE_LEVEL_NONE },
		   { "VKMSG", TRACE_LEVEL_NONE }, { "VKSEM", TRACE_LEVEL_NONE }, { "VKTASK", TRACE_LEVEL_NONE }, { "VKTIMER", TRACE_LEVEL_NONE },
		   { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE },
		   { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }
		}
	},
	{ 	TRACE_MODULE_DI_BASE,
		{ "DIBASE", TRACE_LEVEL_NONE },
		TRACE_MODULE_NONE,
		{ { "DIAUDIO", TRACE_LEVEL_NONE }, { "DICHAN", TRACE_LEVEL_NONE }, { "DICI", TRACE_LEVEL_NONE }, { "DIDEMUX", TRACE_LEVEL_NONE },
		   { "DIEPRM", TRACE_LEVEL_NONE }, { "DIFAN", TRACE_LEVEL_NONE }, { "DIFLASH", TRACE_LEVEL_NONE }, { "DIHDMI", TRACE_LEVEL_NONE },
		   { "DIKEY", TRACE_LEVEL_NONE }, { "DILED", TRACE_LEVEL_NONE }, { "DIOSD", TRACE_LEVEL_NONE }, { "DIPVR", TRACE_LEVEL_NONE },
		   { "DISCART", TRACE_LEVEL_NONE }, { "DISC", TRACE_LEVEL_NONE }, { "DIVIDEO", TRACE_LEVEL_NONE }, { "DIINIT", TRACE_LEVEL_NONE }
		}
	},
	{ 	TRACE_MODULE_DRV_BASE,
		{ "DRV", TRACE_LEVEL_NONE },
		TRACE_MODULE_NONE,
		{ { "DRVGPIO", TRACE_LEVEL_NONE }, { "DRVHDMI", TRACE_LEVEL_NONE }, { "DRVI2C", TRACE_LEVEL_NONE }, { "DRVKPD", TRACE_LEVEL_NONE },
		   { "DRVLNB", TRACE_LEVEL_NONE }, { "DRVMICOM", TRACE_LEVEL_NONE }, { "DRVPWM", TRACE_LEVEL_NONE }, { "DRVRC", TRACE_LEVEL_NONE },
		   { "DRVSCRT", TRACE_LEVEL_NONE }, { "DRVUART", TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE },
		   { "DRVSYNC", TRACE_LEVEL_NONE }, { "DRVDRM", TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }
		}
	},
	{ 	TRACE_MODULE_MON_BASE,
		{ "MONBASE", TRACE_LEVEL_NONE },
		TRACE_MODULE_NONE,
		{ { "MON", TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE },
		   { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE },
		   { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE },
		   { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }
		}
	},
	{ 	TRACE_MODULE_NONE,
		{ NULL, TRACE_LEVEL_NONE },
		TRACE_MODULE_NONE,
		{ { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE },
		   { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE },
		   { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE },
		   { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }, { NULL, TRACE_LEVEL_NONE }
		}
	}

};

static stTraceInfo sTraceLevelInfo[] =
{
	{ "ALL",			TRACE_LEVEL_ALL },
	{ "ERROR",		TRACE_LEVEL_ERROR },
	{ "WARNING",		TRACE_LEVEL_WARNING },
	{ "FUNCTION",	TRACE_LEVEL_FUNCTION },
	{ "INFO",			TRACE_LEVEL_INFO },
	{ "VALUE",		TRACE_LEVEL_VALUE },
	{ "NO",			TRACE_LEVEL_NONE },
	{ NULL,			TRACE_LEVEL_NONE }
};

#if 0
static stMenu register_monitor_menu[] =
{
	{ eMenuTypeLeaf, "Display Register Value",			{ mon_display_register_value } },
	{ eMenuTypeLeaf, "Change Register Value",		{ mon_change_register_value } },
	{ eMenuTypeNull,  "",							{ NULL } }
};
#endif

/*	2006_09_07 pthread.h에서 이미 정의되어 있음
typedef unsigned int pthread_t;
*/
/*static pthread_t	s_xRootThreadId;*/
#ifdef __TRACE_WITH_TASK_NAME__
static unsigned long s_xRootThreadId;
#endif

/********************************************************************************
 * external function prototypes
 ********************************************************************************/

/********************************************************************************
 * function bodies (static)
 ********************************************************************************/

/*SFUNC**************************************************************************
 *	FUNCTION NAME	: _test_search_trace_module_by_name
 *	DESCRIPTION   	: to show trace module status
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static unsigned long _test_search_trace_module_base_by_name
(
	char	*pszName
)
/**************************************************************************EFUNC*/
{
#if 0
	int				moduleInx;

	for( moduleInx=0; moduleInx<sModuleCnt; moduleInx++ )
	{
		if( VK_strcmp( sModuleBaseInfo[moduleInx].m_szName, pszName ) == 0 )
		{
			return sModuleBaseInfo[moduleInx].m_ulValue;
		}
	}
	return TRACE_MODULE_NONE;
#else
	stTraceInfoEx*	pTraceInfoEx = sTraceModuleInfoEx;

	while( pTraceInfoEx->m_ulTraceModuleBase != TRACE_MODULE_NONE )
	{
		/* found !! */
		if( VK_strcmp( pTraceInfoEx->m_sBaseModuleInfo.m_szName, pszName ) == 0 )
		{
			return pTraceInfoEx->m_ulTraceModuleBase;
		}
		pTraceInfoEx++;
	}

	return TRACE_MODULE_NONE;
#endif
}


/*SFUNC**************************************************************************
 *	FUNCTION NAME	: _test_search_trace_module_by_name
 *	DESCRIPTION   	: to show trace module status
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static unsigned long _test_search_trace_module_by_name
(
	char	*pszName
)
/**************************************************************************EFUNC*/
{
#if 0
	int				moduleInx, categoryInx;
	unsigned long 	moduleBase;

	for( moduleInx=0; moduleInx<sModuleCnt; moduleInx++ )
	{
		moduleBase = sModuleBaseInfo[moduleInx].m_ulValue;
		for( categoryInx=0; categoryInx<MAX_CATEGORY; categoryInx++ )
		{
			// save as not packed
			if( VK_strcmp( sModuleInfo[moduleInx][categoryInx].m_szName, pszName ) == 0 )
			{
				return sModuleInfo[moduleInx][categoryInx].m_ulValue;
			}
		}
	}
	return TRACE_MODULE_NONE;
#else
	stTraceInfoEx*	pTraceInfoEx = sTraceModuleInfoEx;
	int				categoryInx = 0;
	unsigned long		module = 0;

	while( pTraceInfoEx->m_ulTraceModuleBase != TRACE_MODULE_NONE )
	{
		for( categoryInx=0; categoryInx<TRACE_MODULE_MAX_CATEGORY; categoryInx++ )
		{
			if( pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName != NULL )
			{
				if( VK_strcmp( pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName, pszName ) == 0 )
				{
					module = pTraceInfoEx->m_ulTraceModuleBase; 	/* set base first */
					module |= ( 0x00000001 << categoryInx );
//INFO_PRINT( "module name %s return %d\n", pszName, module );
					return module;
				}
			}
		}
		pTraceInfoEx++;
	}

//INFO_PRINT( "module name %s return %d\n", pszName, TRACE_MODULE_NONE );
	return TRACE_MODULE_NONE;

#endif
}

/*SFUNC**************************************************************************
 *	FUNCTION NAME	: _test_search_trace_level_by_name
 *	DESCRIPTION   	: to show trace module status
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static unsigned long _test_search_trace_level_by_name
(
	char	*pszName
)
/**************************************************************************EFUNC*/
{
	stTraceInfo	*pTrcInfo = sTraceLevelInfo;

	while( pTrcInfo && pTrcInfo->m_szName )
	{
		if( VK_strcmp( pTrcInfo->m_szName, pszName) == 0 )
		{
			return pTrcInfo->m_ulValue;
		}
		pTrcInfo++;
	}

	return -1;
}

/*SFUNC**************************************************************************
 *	FUNCTION NAME	: _test_search_trace_level_name_by_value
 *	DESCRIPTION   	: to show trace module status
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static const char *_test_search_trace_level_name_by_value
(
	unsigned long	level
)
/**************************************************************************EFUNC*/
{
	stTraceInfo	*pTrcInfo = sTraceLevelInfo;

//INFO_PRINT( "_test_search_trace_level_name_by_value level %d\n", level );
	while( pTrcInfo && pTrcInfo->m_szName )
	{
		if( pTrcInfo->m_ulValue == level )
		{
//INFO_PRINT( "\t_test_search_trace_level_name_by_value pTrcInfo->m_szName %s  pTrcInfo->m_ulValue %d\n", pTrcInfo->m_szName, pTrcInfo->m_ulValue );
			return pTrcInfo->m_szName;
		}
		pTrcInfo++;
	}

	return NULL;
}

#if 0
/*SFUNC**************************************************************************
 *	FUNCTION NAME	: _test_show_trace_level
 *	DESCRIPTION   	: to show trace module status
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static void _test_show_trace_level
(
	stTraceInfo		*pxTraceLevelInfo,
	unsigned long	ulLevel
)
/**************************************************************************EFUNC*/
{
	stTraceInfo	*ptxTraceLevelInfo;

	ptxTraceLevelInfo = pxTraceLevelInfo;

	while( ptxTraceLevelInfo && ptxTraceLevelInfo->m_szName )
	{
		if( ptxTraceLevelInfo->m_ulValue == ulLevel ) {
			INFO_PRINT( "\n\t------] TRACE LEVEL %s\t\t", ptxTraceLevelInfo->m_szName );
			break;
		}
		ptxTraceLevelInfo++;
	}

	return;
}

static void _test_set_trace_data
(
	void
)
{
	int				moduleInx, categoryInx;
	unsigned long	moduleBase;
	const char		*pModuleName;

	VK_memset( sModuleBaseInfo, 0x00, sizeof(sModuleBaseInfo) );

	sModuleCnt = 0;
	for( moduleInx=0; moduleInx<MAX_MODULE_BASE; moduleInx++ )
	{
		moduleBase = 0x00010000<<moduleInx;
		if( ( pModuleName = TraceGetModuleBaseName( moduleBase ) ) != NULL )
		{
			// save as packed
			sModuleBaseInfo[sModuleCnt].m_ulValue	= moduleBase;
			sModuleBaseInfo[sModuleCnt].m_szName	= pModuleName;
			sModuleCnt++;
		}
	}

	VK_memset( sModuleInfo, 0x00, sizeof(sModuleInfo) );

	for( moduleInx=0; moduleInx<sModuleCnt; moduleInx++ )
	{
		moduleBase = sModuleBaseInfo[moduleInx].m_ulValue;
		for( categoryInx=0; categoryInx<MAX_CATEGORY; categoryInx++ )
		{
			// save as not packed
			sModuleInfo[moduleInx][categoryInx].m_szName = TraceGetModuleName( moduleBase | ( 0x00000001 << categoryInx ) );
			if( sModuleInfo[moduleInx][categoryInx].m_szName )
			{
				sModuleInfo[moduleInx][categoryInx].m_ulValue = ( moduleBase | ( 0x00000001 << categoryInx ) );
			}
			else
			{
				sModuleInfo[moduleInx][categoryInx].m_ulValue = TRACE_MODULE_NONE;
			}
		}
	}
}
#endif

static void _trc_show_all
(
	void
)
{
//	unsigned int		moduleBase;
	unsigned int		moduleTracedOn;
	unsigned int		traceLevel;
//	int				moduleInx = 0;
	int				categoryInx = 0;
//	char				tmpName[20];
//	HUINT32				category;
	stTraceInfoEx*	pTraceInfoEx = sTraceModuleInfoEx;
	const char		*pLevelName;

	TraceGetLevel( TRACE_MODULE_NONE, &traceLevel );

	INFO_PRINT( "level %d\n", traceLevel );
	INFO_PRINT( LINDEN_TRACE_FG_NORMAL "\n\nGLOBAL TRACE STATUS ( %s )\n", _test_search_trace_level_name_by_value( traceLevel ) );

	INFO_PRINT( "--------------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------|\n" );
	INFO_PRINT( "     BASE     |     1     |     2     |     3     |     4     |     5     |     6     |     7     |     8     |\n" );
	INFO_PRINT( "              |     9     |     A     |     B     |     C     |     D     |     E     |     F     |     0     |\n" );
	INFO_PRINT( "--------------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------|" );

	while( pTraceInfoEx->m_ulTraceModuleBase != TRACE_MODULE_NONE )
	{
		INFO_PRINT( "\n %9s(%lu) |", pTraceInfoEx->m_sBaseModuleInfo.m_szName,
							 pTraceInfoEx->m_sBaseModuleInfo.m_ulValue );

		// get traced module in each module base
		TraceGetModule( pTraceInfoEx->m_ulTraceModuleBase, &moduleTracedOn );
		for( categoryInx=0; categoryInx<MAX_CATEGORY; categoryInx++ )
		{

			if(categoryInx == 8)
			{
				INFO_PRINT( "\n              |");
			}

			pLevelName = _test_search_trace_level_name_by_value( pTraceInfoEx->m_saCategoryInfo[categoryInx].m_ulValue );

			if( ( ( moduleTracedOn >> categoryInx ) & 0x00000001 ) == 0x00000001 )
			{
				if( pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName )
				{
					switch( pTraceInfoEx->m_saCategoryInfo[categoryInx].m_ulValue )
					{
						case TRACE_LEVEL_ERROR:
							INFO_PRINT( LINDEN_TRACE_FG_LIGHT_RED "%8s(%c)" LINDEN_TRACE_FG_NORMAL,
								pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName, ( ( pLevelName == NULL ) ? '?' : *pLevelName ) );
							break;

						case TRACE_LEVEL_WARNING:
							INFO_PRINT( LINDEN_TRACE_FG_YELLOW "%8s(%c)" LINDEN_TRACE_FG_NORMAL,
								pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName, ( ( pLevelName == NULL ) ? '?' : *pLevelName) );
							break;

						case TRACE_LEVEL_INFO:
							INFO_PRINT( LINDEN_TRACE_FG_MAGENTA "%8s(%c)" LINDEN_TRACE_FG_NORMAL,
								pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName, ( ( pLevelName == NULL ) ? '?' : *pLevelName ) );
							break;

						case TRACE_LEVEL_FUNCTION:
							INFO_PRINT( LINDEN_TRACE_FG_CYAN "%8s(%c)" LINDEN_TRACE_FG_NORMAL,
								pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName, ( ( pLevelName == NULL ) ? '?' : *pLevelName ) );
							break;

						case TRACE_LEVEL_VALUE:
							INFO_PRINT( LINDEN_TRACE_FG_GREEN "%8s(%c)" LINDEN_TRACE_FG_NORMAL,
								pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName, ( ( pLevelName == NULL ) ? '?' : *pLevelName ) );
							break;

						case TRACE_LEVEL_ALL:
							INFO_PRINT( LINDEN_TRACE_FG_BLUE "%8s(%c)" LINDEN_TRACE_FG_NORMAL,
								pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName, ( ( pLevelName == NULL ) ? '?' : *pLevelName ) );
							break;

						/* inherited *pLevelName */
						case TRACE_LEVEL_NONE:
							INFO_PRINT( LINDEN_TRACE_FG_NORMAL "%8s<%c>" LINDEN_TRACE_FG_NORMAL,
								pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName, ( ( pLevelName == NULL ) ? '?' : *pLevelName ) );
							break;

						default:
							INFO_PRINT( LINDEN_TRACE_FG_NORMAL "%8s<%c>" LINDEN_TRACE_FG_NORMAL,
								pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName, ( ( pLevelName == NULL ) ? '?' : *pLevelName ) );
							break;

					}
					INFO_PRINT( "|" );
				}
				else
				{
					INFO_PRINT( "     *     |" );
				}
			}
			else
			{
				if( pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName )
					INFO_PRINT( "%8s(-)|", pTraceInfoEx->m_saCategoryInfo[categoryInx].m_szName );
				else
					INFO_PRINT( "     -     |" );
			}
		}

		INFO_PRINT( "\n--------------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------|" );
		pTraceInfoEx++;
	}
	INFO_PRINT( "\n--------------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------|\n" );
	return;
}

/*SFUNC**************************************************************************
 *	FUNCTION NAME	: _trc_set_trace_on
 *	DESCRIPTION   	: to set trace module
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static void _trc_set_trace_on
(
	void
)
/**************************************************************************EFUNC*/
{
	unsigned long	ulModule;
	char		str[200];
	char		*moduleName;
	unsigned int				i;
	int				strInx = 0;
	int				isAll = 0;
	char		*lastPoint = NULL;

//	_test_set_trace_data();
	int 		ch;

	INFO_PRINT( "\n\t----------------------- SET TRACE ON --------------------------------" );

	INFO_PRINT( "\n\tType [ALL] MODULE1 MODULE2 ... to trace on" );
	INFO_PRINT( "\n\tMODULEx can be module base ..." );
	INFO_PRINT( "\n\tType : " );

	VK_fflush(VK_STDOUT);

	ch = GetChar();

	PutChar(ch);
	while( ch != 0x0a && ch != 0x0d )
	{
		str[strInx++] = (char)ch;
		ch = GetChar();
		PutChar(ch);
	}
	str[strInx] = 0x00;

	for( i=0; i<VK_strlen(str); i++ )
	{
		str[i] = VK_toupper( str[i] );
	}

	moduleName = VK_strtok_r( str, " ", &lastPoint );
	if( ( moduleName != NULL ) && VK_strcmp( moduleName, "ALL" ) == 0 )
	{
		isAll = 1;
		moduleName = VK_strtok_r((char *)NULL, " ", &lastPoint );
	}

	while( moduleName != NULL )
	{
		if( isAll )
		{
			if( ( ulModule = _test_search_trace_module_base_by_name( moduleName ) ) != TRACE_MODULE_NONE )
			{
				TraceSetModule( ulModule | 0x0000FFFF );
			}
			else
			{
				INFO_PRINT( "\ninvalid module base name %s\n", moduleName );
			}

		}
		else
		{
			if( ( ulModule = _test_search_trace_module_by_name( moduleName ) ) != TRACE_MODULE_NONE )
			{
				TraceSetModule( ulModule );
			}
			else
			{
				INFO_PRINT( "\ninvalid module name %s\n", moduleName );
			}
		}
		moduleName = VK_strtok_r((char *)NULL, " ", &lastPoint );
	}

	INFO_PRINT( "\n\t-----------------------------------------------------------------------\n" );

	_trc_show_all();

	return;
}

/*SFUNC**************************************************************************
 *	FUNCTION NAME	: _trc_set_trace_off
 *	DESCRIPTION   	: to unset trace module
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static void _trc_set_trace_off
(
	void
)
/**************************************************************************EFUNC*/
{
	unsigned long	ulModule;
	char		str[200];
	char		*moduleName;
	unsigned int				i;
	int				strInx = 0;
	int				isAll = 0;
	char		*lastPoint = NULL;

//	_test_set_trace_data();
	int 		ch;

	INFO_PRINT( "\n\t----------------------- SET TRACE OFF --------------------------------" );

	INFO_PRINT( "\n\tType [ALL] MODULE1 MODULE2 ... to trace off" );
	INFO_PRINT( "\n\tMODULEx can be module base ..." );
	INFO_PRINT( "\n\tType : " );

	VK_fflush(VK_STDOUT);

	ch = GetChar();
	PutChar(ch);

	while( ch != 0x0a && ch != 0x0d )
	{
		str[strInx++] = (char)ch;
		ch = GetChar();
		PutChar(ch);

	}
	str[strInx] = 0x00;

	for( i=0; i<VK_strlen(str); i++ )
	{
		str[i] = VK_toupper( str[i] );
	}

	moduleName = VK_strtok_r( str, " ", &lastPoint );
	if( ( moduleName != NULL ) && VK_strcmp( moduleName, "ALL" ) == 0 )
	{
		isAll = 1;
		moduleName = VK_strtok_r((char *)NULL, " ", &lastPoint );
	}

	while( moduleName != NULL )
	{
		if( isAll )
		{
			if( ( ulModule = _test_search_trace_module_base_by_name( moduleName ) ) != TRACE_MODULE_NONE )
			{
				TraceUnsetModule( ulModule | 0x0000FFFF );
			}
			else
			{
				INFO_PRINT( "\ninvalid module base name %s\n", moduleName );
			}
		}
		else
		{
			if( ( ulModule = _test_search_trace_module_by_name( moduleName ) ) != TRACE_MODULE_NONE )
			{
				TraceUnsetModule( ulModule );
			}
			else
			{
				INFO_PRINT( "\ninvalid module name %s\n", moduleName );
			}
		}
		moduleName = VK_strtok_r((char *)NULL, " ", &lastPoint );
	}

	INFO_PRINT( "\n\t-----------------------------------------------------------------------\n" );

	_trc_show_all();

	return;
}

/*SFUNC**************************************************************************
 *	FUNCTION NAME	: _trc_set_level
 *	DESCRIPTION   	: to unset trace module
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static void _trc_set_level
(
	void
)
/**************************************************************************EFUNC*/
{
	unsigned long	ulModule;
	char	str[200];
	char	*moduleName;
	int			strInx = 0;
	int			isAll = 0;

	unsigned long	ulLevel=0;
	char		*level;
	unsigned int			i;
	int 		ch;
	char		*lastPoint = NULL;

	INFO_PRINT( "\n\t---------------------- SET TRACE LEVEL -------------------------------" );

	INFO_PRINT( "\n\tType LEVEL [ALL] MODULE1 MODULE2 ... " );
	INFO_PRINT( "\n\tLEVEL = { no, error, warning, info, function, value, all }" );
	INFO_PRINT( "\n\tType : ");

	VK_fflush(VK_STDOUT);

	ch = GetChar();
	PutChar(ch);

	while( ch != 0x0a && ch != 0x0d )
	{
		str[strInx++] = (char)ch;
		ch = GetChar();
		PutChar(ch);

	}
	str[strInx] = 0x00;

	for( i=0; i<VK_strlen(str); i++ )
	{
		str[i] = VK_toupper( str[i] );
	}

	level = VK_strtok_r(str, " ", &lastPoint );

	/* first str is level name */
	if( level != NULL )
	{
		if( ( ulLevel = _test_search_trace_level_by_name( level ) ) == (unsigned long)(-1) )
		{
			INFO_PRINT( "\ninvalid level %s\n", level );
			INFO_PRINT( "\n\t-----------------------------------------------------------------------\n" );
			return;
		}
	}

	moduleName = VK_strtok_r((char *)NULL, " ", &lastPoint );
	if( ( moduleName != NULL ) && VK_strcmp( moduleName, "ALL" ) == 0 )
	{
		isAll = 1;
		moduleName = VK_strtok_r((char *)NULL, " ", &lastPoint );
	}

	while( moduleName != NULL )
	{
		if( isAll )
		{
			if( ( ulModule = _test_search_trace_module_base_by_name( moduleName ) ) != TRACE_MODULE_NONE )
			{
				TraceSetLevel( ulModule, ulLevel );
			}
			else
			{
				INFO_PRINT( "\ninvalid module base name %s\n", moduleName );
			}
		}
		else
		{
			if( ( ulModule = _test_search_trace_module_by_name( moduleName ) ) != TRACE_MODULE_NONE )
			{
				TraceSetLevel( ulModule, ulLevel );
			}
			else
			{
				INFO_PRINT( "\ninvalid module name \"%s\"\n", moduleName );
			}
		}
		moduleName = VK_strtok_r((char *)NULL, " ", &lastPoint );
	}

	INFO_PRINT( "\n\t-----------------------------------------------------------------------\n" );

	_trc_show_all();

	return;
}

stMenu	gaTraceMonitorMenu[] =
{
	{ eMenuTypeLeaf, "SHOW ALL",			{ _trc_show_all } },
	{ eMenuTypeLeaf, "SET MODULE ON", 	{ _trc_set_trace_on } },
	{ eMenuTypeLeaf, "SET MODULE OFF",	{ _trc_set_trace_off } },
	{ eMenuTypeLeaf, "SET LEVEL",			{ _trc_set_level } },
	{ eMenuTypeNull,  "",					{ NULL } }
};

static int _TraceGetModuleIndexFromModuleBase( HUINT32 moduleBase )
{
#if 0
	int	inx;
	int	tmpModuleBase = ( (moduleBase & 0xFFFF0000 ) >> 16 );

	for( inx=0; inx<16; inx++ )
	{
		if( ( tmpModuleBase & 0x00000001 ) == tmpModuleBase )
		{
			return inx;
		}
		tmpModuleBase = ( tmpModuleBase >> 1 );
	}

	INFO_PRINT( " TRC] invalid module base 0x%08x\n", (int)moduleBase );
	return -1;
#else
	stTraceInfoEx*	pTraceInfoEx = sTraceModuleInfoEx;
	int				moduleInx = 0;

	while( pTraceInfoEx->m_ulTraceModuleBase != TRACE_MODULE_NONE )
	{
		/* found !! */
		if( moduleBase == pTraceInfoEx->m_ulTraceModuleBase )
		{
			return moduleInx;
		}
		pTraceInfoEx++;
		moduleInx++;
	}

//	INFO_PRINT( " TRC] invalid module base 0x%08x\n", (int)moduleBase );
	return -1;
#endif

}

static int _TraceGetCategoryIndexFromModule( HUINT32 module )
{
	int	inx;
	int	tmpModule = ( (module & 0x0000FFFF ) );

	for( inx=0; inx<16; inx++ )
	{
		if( ( tmpModule & 0x00000001 ) == tmpModule )
		{
			return inx;
		}
		tmpModule = ( tmpModule >> 1 );
	}

	INFO_PRINT( " TRC] invalid module 0x%08x\n", (int)module );
	return -1;
}

static const char	*_TraceGetLevel( HUINT32 level )
{
	switch( level ) {
		case TRACE_LEVEL_NONE			: return "N";
		case TRACE_LEVEL_ERROR			: return "E";
		case TRACE_LEVEL_WARNING		: return "W";
		case TRACE_LEVEL_FUNCTION		: return "F";
		case TRACE_LEVEL_INFO			: return "I";
		case TRACE_LEVEL_VALUE			: return "V";
		case TRACE_LEVEL_FUNCTION_R		: return "X";
		case TRACE_LEVEL_FUNCTION_X		: return "X";
		default							: return "U";
	}
}

static void _TraceInner( HUINT32 module, HUINT32 level, char *format, va_list arg)
{
#ifdef __TRACE_WITH_TASK_NAME__
	pthread_t			pthreadSelf = pthread_self();
	char				*pThreadName;
#endif
//	char				acTimeStamp[20];

	(void)module;
	(void)level;

	DBG_TRACE_PRINT("\n_TraceInner Enter\n");

#ifdef __TRACE_WITH_TASK_NAME__

	if( VKI_TASK_GetCurrName( &pThreadName ) == VKI_Err_eERR )
	{
		DBG_TRACE_PRINT("\n_TraceInner VKI_TASK_GetCurrentId VKI_TASK_GetTaskName return VKI_Err_eERR\n");
		if( pthreadSelf == s_xRootThreadId )
		{
			pThreadName	= CONST_LINDEN_TRACE_THREAD_NAME_ROOT;
		}
		else
		{
			pThreadName	= CONST_LINDEN_TRACE_THREAD_NAME_INVALID;
		}
	}
	else
	{
		DBG_TRACE_PRINT("\n_TraceInner VKI_TASK_GetCurrentId VKI_TASK_GetTaskName return %s\n", pThreadName);
	}

	DBG_TRACE_PRINT("\n_TraceInner pThreadName %s\n", pThreadName );

#endif
/*
	if( level == TRACE_LEVEL_FUNCTION_R )
	{
		INFO_PRINT( "\n" );
	}
*/
/*
#ifdef __TRACE_WITH_TASK_NAME__
	INFO_PRINT( "*@%s-%s-%s>", TraceGetModuleName( module ), _TraceGetLevel( level ), pThreadName );
#else
	INFO_PRINT( "*@%s-%s-%d", TraceGetModuleName( module ), _TraceGetLevel( level ), (int)pthreadSelf );
#endif
*/
	VK_vprintf( format, arg );

/*
	if( level == TRACE_LEVEL_FUNCTION_X )
	{
		INFO_PRINT( "\n" );
	}
*/
	DBG_TRACE_PRINT("\n_TraceInner Exit\n" );
}


static void _TraceGetEffectiveLevel( int moduleInx, int categoryInx, unsigned long *pEffectiveLevel )
{
	if( pEffectiveLevel == (unsigned long *)NULL )
	{
		TST_PRINT( "_TraceGetEffectiveLevel null parameter\n" );
		return;
	}

	*pEffectiveLevel = sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue;

	/* if no level specified for that module -> assign module's base's level */
	if( *pEffectiveLevel == TRACE_LEVEL_NONE )
	{
		*pEffectiveLevel = sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue;
		/* if no level specified for that module's base -> assign global level */
		if( *pEffectiveLevel == TRACE_LEVEL_NONE )
		{
			*pEffectiveLevel = sTraceLevel;
		}
	}
	TST_PRINT( "_TraceGetEffectiveLevel %d\n", *pEffectiveLevel );
	return;
}

void TraceInit( void )
{
	static int		s_nTraceInitialized = 0;
	int iRet = 0;

	if( s_nTraceInitialized == 0 )
	{
		sTraceLevel			= TRACE_LEVEL_NONE;

		iRet = VK_SEM_Create(&g_TraceSem, "TRACE_SEM", DEFAULT_SUSPEND_TYPE);
		if(iRet != VK_OK)
		{
			TST_PRINT( "VKI_SEM_Crate ERROR !!\n" );
			return;
		}
		else
		{
			sTraceInitialized		= TRUE;
		}
#ifdef __TRACE_WITH_TASK_NAME__
		s_xRootThreadId = pthread_self();
		DBG_TRACE_PRINT("\nTraceInit s_xRootThreadId %d\n", s_xRootThreadId);
#endif

#if 0
		/* no linden monitor at the moment */
		lmRegister_Monitor( "TRACE", saTraceMonitorMenu );
		lmRegister_Monitor( "REGISTER", register_monitor_menu );
#endif
		s_nTraceInitialized = 1;
	}
	else
	{
		TST_PRINT( "\nTraceInit RE-ENTRANCE : already initialized\n" );
	}
	TST_PRINT( "\nTraceInit DONE!\n" );
}

void TraceSetTaskName( unsigned long linuxTaskId, unsigned char *pTaskName )
{
	(void)linuxTaskId;
	(void)pTaskName;
}

/* set "CATEGORY" name in a module */
void TraceSetModuleName( HUINT32 module, const char *name )
{
	HUINT32				moduleBase;
	HUINT32				category;
	stTraceInfoEx*	pTraceInfoEx = sTraceModuleInfoEx;
	int				categoryInx = 0;
	int				lenInputStr;

	if( !sTraceInitialized )
	{
		TST_PRINT( "\n[TraceSetModuleName] ERROR:TraceInit has not been succeeded!\n" );
		return;
	}

	moduleBase	= module & 0xFFFF0000;
	category		= module & 0x0000FFFF;

	TST_PRINT( "\n[TraceSetModuleName] moduleBase 0x%08x category 0x%08x\n", moduleBase, category );

	lenInputStr = VK_strlen(name);
	if (lenInputStr > MAX_CATEGORY_NAME_LENGTH)
		lenInputStr = MAX_CATEGORY_NAME_LENGTH;
	while( pTraceInfoEx->m_ulTraceModuleBase != TRACE_MODULE_NONE )
	{
		/* found !! */
		if( moduleBase == pTraceInfoEx->m_ulTraceModuleBase )
		{
			while( categoryInx<MAX_CATEGORY )
			{
				if( ( category >> categoryInx ) == 0x00000001 )
				{
					break;
				}
				categoryInx++;
			}

			TST_PRINT( "\n[TraceSetModuleName] categoryInx %d name %s\n", categoryInx, name );

			if( categoryInx<MAX_CATEGORY )
			{
				VK_strncpy(pTraceInfoEx->m_saCategoryInfo[ categoryInx ].m_szName, name, lenInputStr);
				pTraceInfoEx->m_saCategoryInfo[ categoryInx ].m_szName[lenInputStr-1] = 0;
			}
			break;
		}
		pTraceInfoEx++;
	}
}

const char	*TraceGetModuleName( HUINT32 module )
{
	HUINT32				moduleBase;
	HUINT32				category;
	stTraceInfoEx*	pTraceInfoEx = sTraceModuleInfoEx;
	int				categoryInx = 0;

	if( !sTraceInitialized )
		return "INV";

	moduleBase	= module & 0xFFFF0000;
	category		= module & 0x0000FFFF;

	TST_PRINT( "\n[TraceGetModuleName] moduleBase 0x%08x category 0x%08x\n", moduleBase, category );
	while( pTraceInfoEx->m_ulTraceModuleBase != TRACE_MODULE_NONE )
	{
		/* found !! */
		if( moduleBase == pTraceInfoEx->m_ulTraceModuleBase )
		{
			while( categoryInx<MAX_CATEGORY )
			{
				if( ( category >> categoryInx ) == 0x00000001 )
				{
					break;
				}
				categoryInx++;
			}

			if( categoryInx<MAX_CATEGORY )
			{
				if( pTraceInfoEx->m_saCategoryInfo[ categoryInx ].m_szName )
					return pTraceInfoEx->m_saCategoryInfo[ categoryInx ].m_szName;
				else
					return ".";
			}
			break;
		}
		pTraceInfoEx++;
	}

	return ".";
}

/*
*	Return all modules whose trace flag is on in the base module
*/
void TraceGetModule( HUINT32 moduleBase, HUINT32 *module )
{
	int		moduleInx;

	if( !sTraceInitialized )
		return;

	*module	 = moduleBase;

	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) < 0 )
	{
		return;
	}

	*module		= sTraceModuleInfoEx[moduleInx].m_ulTraceModules;
	return;
}

void TraceSetModule( HUINT32 module )
{
	HUINT32		moduleBase;
	int		moduleInx;

	if( !sTraceInitialized )
		return;

	moduleBase	= module & 0xFFFF0000;
	TST_PRINT( "\n[TraceSetModule] module 0x%08x moduleBase 0x%08x\n", module, moduleBase );

	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) < 0 )
	{
		TST_PRINT( "\n[TraceSetModule] ERROR : INVALID INDEX\n" );
		return;
	}

	TST_PRINT( "\n[TraceSetModule] moduleIx %d\n", moduleInx );
	sTraceModuleInfoEx[moduleInx].m_ulTraceModules |= module;
	TST_PRINT( "\n[TraceSetModule] Modules trace on 0x%08x\n", sTraceModuleInfoEx[moduleInx].m_ulTraceModules );
	return;
}

void TraceUnsetModule( HUINT32 module )
{
	HUINT32		moduleBase;
	int		moduleInx;

	if( !sTraceInitialized )
		return;

	moduleBase	= module & 0xFFFF0000;
	TST_PRINT( "\n[TraceUnsetModule] module 0x%08x moduleBase 0x%08x\n", module, moduleBase );

	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) < 0 )
	{
		TST_PRINT( "\n[TraceUnsetModule] ERROR : INVALID INDEX\n" );
		return;
	}

	TST_PRINT( "\n[TraceUnsetModule] moduleIx %d\n", moduleInx );
	sTraceModuleInfoEx[moduleInx].m_ulTraceModules &= ~(module&0x0000FFFF);
	TST_PRINT( "\n[TraceUnsetModule] Modules trace on 0x%08x\n", sTraceModuleInfoEx[moduleInx].m_ulTraceModules );
	return;
}

void TraceGetLevel( HUINT32 module, HUINT32 *level )
{
	HUINT32		moduleBase;
	HUINT32		category;
	int		moduleInx, categoryInx;

	if( !sTraceInitialized )
		return;

	moduleBase 	= module & 0xFFFF0000;
	category 	= module & 0x0000FFFF;

	TST_PRINT( "\n[TraceGetLevel] moduleBase 0x%08x category 0x%08x\n", moduleBase, category );
	if( moduleBase == TRACE_MODULE_NONE )
	{
		/* all module level setting */
		*level = sTraceLevel;
	}
	else
	{
		if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) == -1 )
		{
			/* error while getting module index */
			*level= TRACE_LEVEL_NONE;
			return;
		}

		if( category == TRACE_MODULE_NONE )
		{
			/* all category level setting in a module */
			*level = sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue;
		}
		else
		{
			if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) == -1 )
			{
				/* error while getting module index */
				TST_PRINT( "\n[TraceGetLevel] categoryInx ERROR\n" );
				return;
			}

			/* a category level setting in a module */
			if( categoryInx < TRACE_MODULE_MAX_CATEGORY )
				*level = sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue;
		}

	}
	TST_PRINT( "\n[TraceGetLevel] level %d\n", *level );
	return;
}


/* Trace level priority : category -> module -> global */
void TraceSetLevel( HUINT32 module, HUINT32 level )
{
	HUINT32		moduleBase;
	HUINT32		category;
	int		moduleInx, categoryInx;

	if( !sTraceInitialized )
		return;

	moduleBase 	= module & 0xFFFF0000;
	category 	= module & 0x0000FFFF;

	TST_PRINT( "\n[TraceSetLevel] moduleBase 0x%08x category 0x%08x\n", moduleBase, category );
	if( moduleBase == TRACE_MODULE_NONE )
	{
		/* all module level setting */
		sTraceLevel	= level;
		TST_PRINT( "\n[TraceSetLevel] sTraceLevel %d\n", sTraceLevel );
	}
	else
	{
		if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) == -1 )
		{
			/* error while getting module index */
			TST_PRINT( "\n[TraceSetLevel] moduleInx ERROR\n" );
			return;
		}

		if( category == TRACE_MODULE_NONE )
		{
			/* all category level setting in a module */
			sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue = level;
			TST_PRINT( "\n[TraceSetLevel] sTraceModuleInfoEx[%d].m_sBaseModuleInfo.m_ulValue : %d\n",
				          moduleInx, sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue );
		}
		else
		{
			if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) == -1 )
			{
				/* error while getting module index */
				TST_PRINT( "\n[TraceSetLevel] categoryInx ERROR\n" );
				return;
			}

			/* a category level setting in a module */
			if( categoryInx < TRACE_MODULE_MAX_CATEGORY )
				sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue = level;

			TST_PRINT( "\n[TraceGetLevel] sTraceModuleInfoEx[%d].m_saCategoryInfo[%d].m_ulValue : %d\n",
				          moduleInx, categoryInx, sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue );
		}

	}
}


void Trace( HUINT32 module, HUINT32 level, char *ptrFormat, ... )
{
	va_list   varg;
	HUINT32		moduleBase;
	int		moduleInx, categoryInx;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	if( !sTraceInitialized )
		return;

	CS_START( sTraceSemId );

	moduleBase	= module & 0xFFFF0000;
	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) < 0 )
		{
			DBG_TRACE_PRINT("\n[Trace] NO CATEGORY in MODULE %d\n", moduleInx);
			return;
		}

		/* check if module is set to be traced */
		if( ( module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == module )
		{
#if 0
			/* check if level is set to be trace for the module */
			if( ( level <= sTraceLevel ) || 	/* if global level is set to be trace */
				( level <= sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue ) ||
				( level <= sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue ) )
			{
				va_start( varg, ptrFormat );
				_TraceInner( module, level, ptrFormat, varg );
				va_end( varg );
			}
#else
			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );

			/* check if level is set to be trace for the module */
			if( effectiveLevel >= sTraceLevel )
			{
				va_start( varg, ptrFormat );
				_TraceInner( module, level, ptrFormat, varg );
				va_end( varg );
			}
#endif
		}
	}

	CS_END( sTraceSemId );
}

/*
*	빈도수가 높은 function에서 flow tracing을 하기 위해 만든 함수.
*/
void TraceFlow( HUINT32 module, char *ptrFormat, ... )
{
	va_list   varg;
	HUINT32		moduleBase;
	int		moduleInx, categoryInx;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	if( !sTraceInitialized )
		return;

	CS_START( sTraceSemId );

	moduleBase	= module & 0xFFFF0000;
	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) < 0 )
		{
			DBG_TRACE_PRINT("\n[TraceInfo] NO CATEGORY in MODULE %d\n", moduleInx);
			return;
		}

		/* check if module is set to be traced */
		if( ( module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == module )
		{
			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );

			/* check if level is set to be trace for the module */
			if( effectiveLevel >= TRACE_LEVEL_FLOW )
			{
				DBG_TRACE_PRINT("\n[TraceInfo] PRINT ++\n");
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_FLOW, ptrFormat, varg );
				va_end( varg );
				DBG_TRACE_PRINT("\n[TraceInfo] PRINT --\n");
			}
		}
	}

	CS_END( sTraceSemId );

}

void TraceError( HUINT32 module, char *ptrFormat, ... )
{
	va_list   varg;
	HUINT32		moduleBase;
	int		moduleInx, categoryInx;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	if( !sTraceInitialized )
		return;

	CS_START( sTraceSemId );

	moduleBase	= module & 0xFFFF0000;
	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) < 0 )
		{
			DBG_TRACE_PRINT("\n[TraceError] NO CATEGORY in MODULE %d\n", moduleInx);
			return;
		}

		/* check if module is set to be traced */
		if( ( module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == module )
		{
#if 0
			/* check if level is set to be trace for the module */
			if( ( sTraceLevel >= TRACE_LEVEL_ERROR ) || 	/* if global level is set to be trace */
				( sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue >= TRACE_LEVEL_ERROR ) ||
				( sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue >= TRACE_LEVEL_ERROR ) )
			{
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_ERROR, ptrFormat, varg );
				va_end( varg );
			}
#else
			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );

			/* check if level is set to be trace for the module */
			if( effectiveLevel >= TRACE_LEVEL_ERROR )
			{
				DBG_TRACE_PRINT("\n[TraceError] PRINT ++\n");
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_ERROR, ptrFormat, varg );
				va_end( varg );
				DBG_TRACE_PRINT("\n[TraceError] PRINT --\n");
			}
#endif
		}
	}

	CS_END( sTraceSemId );

}

void TraceWarning( HUINT32 module, char *ptrFormat, ... )
{
	va_list   varg;
	HUINT32		moduleBase;
	int		moduleInx, categoryInx;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	if( !sTraceInitialized )
		return;

	CS_START( sTraceSemId );

	moduleBase	= module & 0xFFFF0000;
	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) < 0 )
		{
			DBG_TRACE_PRINT("\n[TraceWarning] NO CATEGORY in MODULE %d\n", moduleInx);
			return;
		}

		/* check if module is set to be traced */
		if( ( module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == module )
		{
#if 0
			/* check if level is set to be trace for the module */
			if( ( sTraceLevel >= TRACE_LEVEL_WARNING ) || 	/* if global level is set to be trace */
				( sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue >= TRACE_LEVEL_WARNING ) ||
				( sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue >= TRACE_LEVEL_WARNING ) )
			{
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_WARNING, ptrFormat, varg );
				va_end( varg );
			}
#else
			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );

			/* check if level is set to be trace for the module */
			if( effectiveLevel >= TRACE_LEVEL_WARNING )
			{
				DBG_TRACE_PRINT("\n[TraceWarning] PRINT ++\n");
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_WARNING, ptrFormat, varg );
				va_end( varg );
				DBG_TRACE_PRINT("\n[TraceWarning] PRINT --\n");
			}
#endif
		}
	}

	CS_END( sTraceSemId );

}


void TraceEnter( HUINT32 module, char *ptrFormat, ... )
{
	va_list   varg;
	HUINT32		moduleBase;
	int		moduleInx, categoryInx;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	if( !sTraceInitialized )
		return;

	DBG_TRACE_PRINT("\n[TraceEnter] module 0x%08x\n", module);

	CS_START( sTraceSemId );

	moduleBase	= module & 0xFFFF0000;
	DBG_TRACE_PRINT("\n[TraceEnter] moduleBase 0x%08x\n", moduleBase);
	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) < 0 )
		{
			DBG_TRACE_PRINT("\n[TraceEnter] NO CATEGORY in MODULE %d\n", moduleInx);
			return;
		}

		/* check if module is set to be traced */
		if( ( module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == module )
		{
			DBG_TRACE_PRINT( "\n[TraceEnter] module & sTraceModuleInfoEx[%d].m_ulTraceModules : 0x%08x\n",
				           moduleInx, module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules );

			DBG_TRACE_PRINT("\n[TraceEnter] sTraceLevel %d module(%d) level %d category(%d) level %d\n",
				           sTraceLevel, moduleInx,
				           sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue,
				           categoryInx,
				           sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue );

#if 0
			/* check if level is set to be trace for the module */
			if( ( sTraceLevel >= TRACE_LEVEL_FUNCTION ) || 	/* if global level is set to be trace */
				( sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue >= TRACE_LEVEL_FUNCTION ) ||
				( sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue >= TRACE_LEVEL_FUNCTION ) )
			{
				DBG_TRACE_PRINT( VK_printf( "\n[TraceEnter] PRINT ++\n" ));
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_FUNCTION_R, ptrFormat, varg );
				va_end( varg );
				DBG_TRACE_PRINT( VK_printf( "\n[TraceEnter] PRINT --\n" ));
			}
#else
			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );

			/* check if level is set to be trace for the module */
			if( effectiveLevel >= TRACE_LEVEL_FUNCTION )
			{
				DBG_TRACE_PRINT("\n[TraceEnter] PRINT ++\n");
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_FUNCTION_R, ptrFormat, varg );
				va_end( varg );
				DBG_TRACE_PRINT("\n[TraceEnter] PRINT --\n" );
			}
#endif
		}
		else
		{
			DBG_TRACE_PRINT("[TraceEnter] not in trace ON\n");
		}
	}

	CS_END( sTraceSemId );
	DBG_TRACE_PRINT("\n[TraceEnter] RETURN\n");

}


void TraceExit( HUINT32 module, char *ptrFormat, ... )
{
	va_list   varg;
	HUINT32		moduleBase;
	int		moduleInx, categoryInx;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	if( !sTraceInitialized )
		return;

	CS_START( sTraceSemId );

	moduleBase	= module & 0xFFFF0000;
	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) < 0 )
		{
			DBG_TRACE_PRINT("\n[TraceExit] NO CATEGORY in MODULE %d\n", moduleInx);
			return;
		}

		/* check if module is set to be traced */
		if( ( module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == module )
		{
#if 0
			/* check if level is set to be trace for the module */
			if( ( sTraceLevel >= TRACE_LEVEL_FUNCTION ) || 	/* if global level is set to be trace */
				( sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue >= TRACE_LEVEL_FUNCTION ) ||
				( sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue >= TRACE_LEVEL_FUNCTION ) )
			{
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_FUNCTION_X, ptrFormat, varg );
				va_end( varg );
			}
#else
			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );

			/* check if level is set to be trace for the module */
			if( effectiveLevel >= TRACE_LEVEL_FUNCTION )
			{
				DBG_TRACE_PRINT("\n[TraceExit] PRINT ++\n");
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_FUNCTION_X, ptrFormat, varg );
				va_end( varg );
				DBG_TRACE_PRINT("\n[TraceExit] PRINT --\n");
			}
#endif
		}
	}

	CS_END( sTraceSemId );

}

void TraceInfo( HUINT32 module, char *ptrFormat, ... )
{
	va_list   varg;
	HUINT32		moduleBase;
	int		moduleInx, categoryInx;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	if( !sTraceInitialized )
		return;

	CS_START( sTraceSemId );

	moduleBase	= module & 0xFFFF0000;

	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) < 0 )
		{
			DBG_TRACE_PRINT("\n[TraceInfo] NO CATEGORY in MODULE %d\n", moduleInx);
			return;
		}

		/* check if module is set to be traced */
		if( ( module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == module )
		{
#if 0
			/* check if level is set to be trace for the module */
			if( ( sTraceLevel >= TRACE_LEVEL_INFO ) || 	/* if global level is set to be trace */
				( sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue >= TRACE_LEVEL_INFO ) ||
				( sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue >= TRACE_LEVEL_INFO ) )
			{
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_INFO, ptrFormat, varg );
				va_end( varg );
			}
#else
			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );


			/* check if level is set to be trace for the module */
			if( effectiveLevel >= TRACE_LEVEL_INFO )
			{
				DBG_TRACE_PRINT("\n[TraceInfo] PRINT ++\n");
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_INFO, ptrFormat, varg );
				va_end( varg );
				DBG_TRACE_PRINT("\n[TraceInfo] PRINT --\n");
			}
#endif
		}
	}

	CS_END( sTraceSemId );

}


void TraceValue( HUINT32 module, char *ptrFormat, ... )
{
	va_list   varg;
	HUINT32		moduleBase;
	int		moduleInx, categoryInx;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	if( !sTraceInitialized )
		return;

	CS_START( sTraceSemId );

	moduleBase	= module & 0xFFFF0000;
	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) < 0 )
		{
			DBG_TRACE_PRINT("\n[TraceValue] NO CATEGORY in MODULE %d\n", moduleInx);
			return;
		}

		/* check if module is set to be traced */
		if( ( module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == module )
		{
#if 0
			/* check if level is set to be trace for the module */
			if( ( sTraceLevel >= TRACE_LEVEL_VALUE ) || 	/* if global level is set to be trace */
				( sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue >= TRACE_LEVEL_VALUE ) ||
				( sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue >= TRACE_LEVEL_VALUE ) )
			{
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_VALUE, ptrFormat, varg );
				va_end( varg );
			}
#else
			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );

			/* check if level is set to be trace for the module */
			if( effectiveLevel >= TRACE_LEVEL_FUNCTION )
			{
				DBG_TRACE_PRINT("\n[TraceValue] PRINT ++\n");
				va_start( varg, ptrFormat );
				_TraceInner( module, TRACE_LEVEL_VALUE, ptrFormat, varg );
				va_end( varg );
				DBG_TRACE_PRINT("\n[TraceValue] PRINT --\n");
			}
#endif
		}
	}

	CS_END( sTraceSemId );

}

void TraceValueArray( HUINT32 module, char *desc, int len, char *value )
{
	HUINT32				moduleBase;
	int				moduleInx, categoryInx, i;
	unsigned char*	ptr;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	if( !sTraceInitialized )
		return;

	CS_START( sTraceSemId );

	moduleBase	= module & 0xFFFF0000;
	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( module ) ) < 0 )
		{
			DBG_TRACE_PRINT("\n[TraceValueArray] NO CATEGORY in MODULE %d\n", moduleInx);
			return;
		}

		/* check if module is set to be traced */
		if( ( module & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == module )
		{
#if 0
			/* check if level is set to be trace for the module */
			if( ( sTraceLevel >= TRACE_LEVEL_VALUE ) || 	/* if global level is set to be trace */
				( sTraceModuleInfoEx[moduleInx].m_sBaseModuleInfo.m_ulValue >= TRACE_LEVEL_VALUE ) ||
				( sTraceModuleInfoEx[moduleInx].m_saCategoryInfo[categoryInx].m_ulValue >= TRACE_LEVEL_VALUE ) )
			{
				INFO_PRINT( "                     |%s|%s| :: %s(%d)", TraceGetModuleName( module ), _TraceGetLevel( TRACE_LEVEL_VALUE ), desc, len );
				ptr = (unsigned char *)value;
				for( i=0; i<len; i++ )
				{
					if( !(i%16) )
						INFO_PRINT( "\n                     |%s|%s| :: ", TraceGetModuleName( module ), _TraceGetLevel( TRACE_LEVEL_VALUE ) );
					INFO_PRINT( "%02x ", *ptr++ );
					if( !((i+1)%4) )
						INFO_PRINT( "  " );
				}
				INFO_PRINT( "\n" );
			}
#else
			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );

			/* check if level is set to be trace for the module */
			if( effectiveLevel >= TRACE_LEVEL_VALUE )
			{
				INFO_PRINT( "                     |%s|%s| :: %s(%d)", TraceGetModuleName( module ), _TraceGetLevel( TRACE_LEVEL_VALUE ), desc, len );
				ptr = (unsigned char *)value;
				for( i=0; i<len; i++ )
				{
					if( !(i%16) )
						INFO_PRINT( "\n                     |%s|%s| :: ", TraceGetModuleName( module ), _TraceGetLevel( TRACE_LEVEL_VALUE ) );
					INFO_PRINT( "%02x ", *ptr++ );
					if( !((i+1)%4) )
						INFO_PRINT( "  " );
				}
				INFO_PRINT( "\n" );
			}
#endif
		}
	}

	CS_END( sTraceSemId );

}

HUINT32 Trace_CheckModule( HUINT32 ulModule, HUINT32 ulLevel)
{
	HUINT32		moduleBase;
	int		moduleInx, categoryInx;
	unsigned long effectiveLevel = TRACE_LEVEL_NONE;

	moduleBase	= ulModule & 0xFFFF0000;
	if( ( moduleInx = _TraceGetModuleIndexFromModuleBase( moduleBase ) ) != -1 )
	{
		if( ( categoryInx = _TraceGetCategoryIndexFromModule( ulModule ) ) < 0 )
		{
			TST_PRINT( "\n[TraceValue] NO CATEGORY in MODULE %d\n", moduleInx );
			return 1;
		}

		/* check if module is set to be traced */
		if( ( ulModule & sTraceModuleInfoEx[moduleInx].m_ulTraceModules ) == ulModule )
		{

			_TraceGetEffectiveLevel( moduleInx, categoryInx, &effectiveLevel );

			/* check if level is set to be trace for the module */
			if( effectiveLevel >= ulLevel )
			{
				return 1;
			}
		}
	}
	return 0;
}



const char	*TraceGetModuleBaseName( HUINT32 moduleBase )
{
	stTraceInfoEx	*pTraceInfoEx = &sTraceModuleInfoEx[0];

	if( !sTraceInitialized )
		return "*";

	while( pTraceInfoEx->m_ulTraceModuleBase != TRACE_MODULE_NONE )
	{
		/* found !! */
		if( moduleBase == pTraceInfoEx->m_ulTraceModuleBase )
		{
			if( pTraceInfoEx->m_sBaseModuleInfo.m_szName )
				return pTraceInfoEx->m_sBaseModuleInfo.m_szName;
			else
				return "*";
		}
		pTraceInfoEx++;
	}

	return "*";
}



#endif /* __LINDEN_TRACE_ON__ */


/******************************* end of file ************************************/


