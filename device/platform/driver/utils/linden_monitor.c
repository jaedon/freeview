/* $Header$ */
/********************************************************************************
 * $Workfile$
 * $Modtime$
 * Project 	: OC-2000 ( two-way cable, OOB only )
 * Author 	: Jinny Youn
 *
 * Desc 	: for target shell & test
 *
 *
 * Copyright (c) 2002 by HUMAX Co., Ltd.
 * All right reserved.
 ********************************************************************************/

 /**
  * linden_monitor.c
 */

 /**
  * @defgroup		 MONITOR MONITOR : menu_monitor module
  * @author 	 	Jinny Youn
  * @note			 MenuMonitor APIs
  * @brief			 Define MenuMonitor APIs
  * @file			 linden_monitor.c
 */



/********************************************************************************
 * include
 ********************************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "linden_monitor.h"
#include "linden_net.h"
#include "vkernel.h"

/********************************************************************************
 * include ( platform specific )
 ********************************************************************************/

/********************************************************************************
 * definition
 ********************************************************************************/
#define MAX_MONITOR_MODULE		20

#define MENU_NUM_QUIT			-1
#define MENU_NUM_HELP			-2
#define MENU_NUM_RESET			-3
#define MENU_NUM_FACTORY_RESET	-4
#define MENU_NUM_RETRY			-5

#define MAX_PATH_LEN			150

#if defined(CONFIG_DEBUG)
#define INFO_PRINT	VK_printf
#else
#define INFO_PRINT(...)	((void)0)
#endif

/********************************************************************************
 * typedef
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
static int		sLMStatus = 0;
static int		sNumRegisteredModule = 0;

static stMenu	saLMSubMenu[ MAX_MONITOR_MODULE ] =
{
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} },
	{ eMenuTypeNull, NULL, {NULL} }
};

static stMenu	sLMMainMenu =
{
	.m_nCategory = eMenuTypeNode,
	.m_szDesc = "LINDEN MONITOR",
};

static char		sCurrentPath[MAX_PATH_LEN];
static char		*spCurrentPath;

/********************************************************************************
 * external function prototypes
 ********************************************************************************/

/********************************************************************************
 * global function prototypes
 ********************************************************************************/

/********************************************************************************
 * static function prototypes
 ********************************************************************************/

/********************************************************************************
 * function bodies (static)
 ********************************************************************************/

/*SFUNC**************************************************************************
 *	FUNCTION NAME	: lmShow_NodeMenu
 *	DESCRIPTION   	: to show menu list
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static void lmShow_NodeMenu
(
	const char*	szDesc,
	stMenu*		pxMenu
)
/**************************************************************************EFUNC*/
{
	stMenu	*pMenu;
	int		 menuNum;

	(void)szDesc;

	INFO_PRINT( "\n" );
	INFO_PRINT( "+------------------------------------------------------------------------------\n" );
	INFO_PRINT( "| %s\n", szDesc );
	INFO_PRINT( "+------------------------------------------------------------------------------" );

	/* select submenu */
	pMenu 		= pxMenu;
	menuNum		= 1;
	while( pMenu && ( pMenu->m_nCategory != eMenuTypeNull ) )
	{
		switch( pMenu->m_nCategory )
		{
			case eMenuTypeNode:
				INFO_PRINT( "\n| %2d. %s+", menuNum, pMenu->m_szDesc );
				menuNum++;
				break;
			case eMenuTypeLeaf:
				INFO_PRINT( "\n| %2d. %s", menuNum, pMenu->m_szDesc );
				menuNum++;
				break;
			default:
				break;
		}
		pMenu++;
	}
	INFO_PRINT( "\n+------------------------------------------------------------------------------" );
	INFO_PRINT( "\n|  q. QUIT" );
	INFO_PRINT( "\n|  h. HELP" );
	/* INFO_PRINT( "\n|  r. RESET STB" ); */
	/* INFO_PRINT( "\n|  f. FACTORY RESET" ); */

	INFO_PRINT( "\n+------------------------------------------------------------------------------\n\n" );
}

/*SFUNC**************************************************************************
 *	FUNCTION NAME	: lmGet_Command
 *	DESCRIPTION   	: to get user input
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static void lmGet_Command
(
	int		callDepth,
	char*	pPath,
	int*	nNum
)
/**************************************************************************EFUNC*/
{
	char	caCommand[100+1];
	int		inx;

	(void)callDepth;
	(void)pPath;

	while(1)
	{
		inx = 0;

		INFO_PRINT( "%d:%s> ", callDepth, pPath );

		VK_fflush(VK_STDOUT);

		while( inx<100 )
		{
			caCommand[inx] = (char)GetChar();

			PutChar(caCommand[inx]);

			if( ( caCommand[inx] == 0x0d ) || ( caCommand[inx] == 0x0a ) )
				break;
			if( caCommand[inx] != 0x08 )	/* delete	*/
				inx++;
		}

		if( ( caCommand[0] == 0x0d ) || ( caCommand[0] == 0x0a ) )
		{
			*nNum = MENU_NUM_RETRY;
			break;
		}
		else if( ( caCommand[0] == 'q' ) || ( caCommand[0] == 'Q' ) )
		{
			*nNum = MENU_NUM_QUIT;
			break;
		}
		else if( ( caCommand[0] == 'h' ) || ( caCommand[0] == 'H' ) )
		{
			*nNum = MENU_NUM_HELP;
			break;
		}
		else if( ( caCommand[0] == 'r' ) || ( caCommand[0] == 'R' ) )
		{
			*nNum = MENU_NUM_RESET;
			break;
		}
		else if( ( caCommand[0] == 'f' ) || ( caCommand[0] == 'F' ) )
		{
			*nNum = MENU_NUM_FACTORY_RESET;
			break;
		}

		else
		{
			caCommand[inx] = 0x00;	/* null */
			*nNum = VK_atoi( caCommand );

			break;
		}

	}

	return;
}

/*SFUNC**************************************************************************
 *	FUNCTION NAME	: lmGet_MenuFunction
 *	DESCRIPTION   	: to get menu function
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
static stMenu* lmFind_Submenu
(
	int			xIndex,
	stMenu*		pxMenu
)
/**************************************************************************EFUNC*/
{
	stMenu	*pMenu;
	int		 menuNum;

	/* select submenu */
	pMenu 		= pxMenu;
	menuNum		= 0;
	while( pMenu && ( pMenu->m_nCategory != eMenuTypeNull ) )
	{
		switch( pMenu->m_nCategory )
		{
			case eMenuTypeNode:
			case eMenuTypeLeaf:
				menuNum++;
				break;
			default:
				break;
		}

		if( xIndex == menuNum )
		{
			return pMenu;
		}

		pMenu++;
	}

	return (stMenu *)NULL;
}

static void lmPrint( char *ptrFormat, ... )
{
	va_list varg;

	va_start( varg, ptrFormat );
	VK_vprintf( ptrFormat, varg );
	va_end( varg );
}

/*SFUNC************************************************************************
 *	FUNCTION NAME		lmHandle_NodeMenu
 *	DESCRIPTION
 * 	INPUT         		NONE
 *	OUTPUT        		NONE
 *	RETURN        		NONE
 *	REMARK        		NONE
 *******************************************************************************/
static void lmHandle_NodeMenu
(
	int					callDepth,
	char*				pPath,
	const stMenu*		pMenus,
	int					ableQuit
)
/*************************************************************************EFUNC*/
{
	int					nNum;
	stMenu*				pMenu;
	char				szPath[MAX_PATH_LEN];

	if( pMenus->m_nCategory != eMenuTypeNode )
		return;

	while(1)
	{
		lmShow_NodeMenu( pPath, pMenus->m_uContents.m_pstSubMenu );

		lmGet_Command( callDepth, pPath, &nNum );

		/* find submenu function */
		if( (nNum == MENU_NUM_QUIT) && (ableQuit!=0) )
		{
			/* QUIT */
			return;
		}
		else if( nNum == MENU_NUM_HELP )
		{
			continue;
		}

		else if( nNum == MENU_NUM_RETRY)
		{
			continue;
		}
/*
		else if( nNum == MENU_NUM_RESET )
		{
			nvRam_Close();
			pwrReBoot();
			// now, STB will be rebooted
		}
		else if( nNum == MENU_NUM_FACTORY_RESET )
		{
			__simo_Factory_Reset();
			//pwrReBoot();

			// now, STB will be rebooted
		}
*/
		else
		{
			if( ( pMenu = lmFind_Submenu( nNum, pMenus->m_uContents.m_pstSubMenu ) ) != (stMenu *)NULL )
			{
				if( pMenu->m_nCategory == eMenuTypeNode )
				{
					VK_snprintf( szPath, sizeof(szPath), "%s%s/", pPath, pMenu->m_szDesc );
					lmHandle_NodeMenu( callDepth+1, szPath, pMenu, 1 );
				}
				else
				{
					(pMenu->m_uContents.m_pxMonitorFunction)();
				}
			}
			else
			{
				lmPrint( "no function or submenus\n" );
			}
		}
	}
}

/********************************************************************************
 * function bodies (API)
 ********************************************************************************/

/*SFUNC**************************************************************************
 *	FUNCTION NAME		lmInit_Monitor
 *	DESCRIPTION
 * 	INPUT         		NONE
 *	OUTPUT        		NONE
 *	RETURN        		NONE
 *	REMARK        		NONE
 ********************************************************************************/
void lmInit_Monitor
(
	void
)
/*************************************************************************EFUNC*/
{
	if( sLMStatus != 0 )
	{
		lmPrint( "LindenMonitor already initialized !!\n" );
	}
	else
	{
		sLMStatus 				= 1;	/* initialized */
		sNumRegisteredModule	= 0;
		spCurrentPath			= sCurrentPath;
		*spCurrentPath			= 0x00;
	}

	if(sLMMainMenu.m_uContents.m_pstSubMenu == NULL)
	{
		sLMMainMenu.m_uContents.m_pstSubMenu = saLMSubMenu;
	}
	return;
}

/*SFUNC************************************************************************
 *	FUNCTION NAME		lmRegister_Monitor
 *	DESCRIPTION
 * 	INPUT         		NONE
 *	OUTPUT        		NONE
 *	RETURN        		NONE
 *	REMARK        		NONE
 *******************************************************************************/
void lmRegister_Monitor
(
	const char*			szDesc,
	stMenu*			pMenus
)
/*************************************************************************EFUNC*/
{
	stMenu				*pnewMenu;

	if( sLMStatus != 1 )
	{
#ifndef PRODUCT_TEST
		lmPrint( "\tLM] invalid status %d !\n", sLMStatus );
#endif
		return;
	}

	if( sNumRegisteredModule >= MAX_MONITOR_MODULE )
	{
		lmPrint( "\tLM] no more buffer !" );
		return;
	}

	if(sLMMainMenu.m_uContents.m_pstSubMenu == NULL)
	{
		sLMMainMenu.m_uContents.m_pstSubMenu = saLMSubMenu;
	}

	pnewMenu = &sLMMainMenu.m_uContents.m_pstSubMenu[sNumRegisteredModule];
	pnewMenu->m_nCategory				= eMenuTypeNode;
	pnewMenu->m_szDesc					= szDesc;
	pnewMenu->m_uContents.m_pstSubMenu	= (stMenu *)pMenus;
	sNumRegisteredModule++;

	return;
}

/*SFUNC**************************************************************************
 *	FUNCTION NAME		lmStart_Monitor
 *	DESCRIPTION
 * 	INPUT         		NONE
 *	OUTPUT        		NONE
 *	RETURN        		NONE
 *	REMARK        		NONE
 ********************************************************************************/
void lmStart_Monitor
(
	void
)
/*************************************************************************EFUNC*/
{
	char	szPath[MAX_PATH_LEN];

	if( sLMStatus != 1 )
	{
		lmPrint( "\tLM] invalid status %d\n", sLMStatus );
		return;
	}

	if( sNumRegisteredModule < 1 )
	{
		lmPrint( "\tLM] no registered module\n" );
		return;
	}

	sLMStatus = 2;	/* started */
	VK_snprintf( szPath, sizeof(szPath), "%s/", sLMMainMenu.m_szDesc );
	lmHandle_NodeMenu( 1, szPath, &sLMMainMenu, 1 );
	return;
}

/*SFUNC**************************************************************************
 *	FUNCTION NAME		lmEnd_Monitor
 *	DESCRIPTION
 * 	INPUT         		NONE
 *	OUTPUT        		NONE
 *	RETURN        		NONE
 *	REMARK        		NONE
 ********************************************************************************/
void lmEnd_Monitor
(
	void
)
/*************************************************************************EFUNC*/
{
	sLMStatus = 1;	/* end but initialized */
	return;
}


/*SFUNC**************************************************************************
 *	FUNCTION NAME	: lmFind_String
 *	DESCRIPTION   	: to find string to print from name table
 * 	INPUT         	: NONE
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 ********************************************************************************/
const char* lmFind_String
(
	stLMNameTable*		pxTable,
	int					nInt
)
/**************************************************************************EFUNC*/
{
	stLMNameTable		*ptTable;

	ptTable = pxTable;
	while( ptTable && ptTable->m_nInt != -1 )
	{
		if( ptTable->m_nInt == nInt )
			break;
		ptTable++;
	}

	return ptTable->m_szName;
}


/******************************* end of file **************************************/


