/**
 * unittest.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "htype.h"
#include "vkernel.h"

#include "unittest.h"
#include "linden_trace.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define MAX_CMD_LEN	128

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
static	UT_MENU_ITEM_T		*s_headMenuItem;

static void		P_UT_MENU_Display(UT_MENU_ITEM_T *pItem)
{
	UT_MENU_ITEM_T *pMenuItem;
	HINT32			i;

	if (pItem == NULL)
		return;

	i = 0;
	pMenuItem = pItem;
	while (pMenuItem != NULL)
	{
		switch (pMenuItem->menuType)
		{
		case MENU_ITEM_START :
			VK_printf("*============================================================*\n");
			VK_printf("* %3d. %s\n", i, "exit");
			break;
		case MENU_ITEM_FUNC :
			VK_printf("* %3d. %-32s (%3d) seconds\n", i, pMenuItem->pszFuncName ? pMenuItem->pszFuncName : "NULL", pMenuItem->expected_time < 0 ? -1 : pMenuItem->expected_time);
			break;
		case MENU_ITEM_SUBMENU :
			VK_printf("* %3d. %-32s\n", i, pMenuItem->pszFuncName ? pMenuItem->pszFuncName : "NULL");
			break;
		case MENU_ITEM_END :
			VK_printf("*  100. %s\n", "test all functions");
			VK_printf("*------------------------------------------------------------*\n");
			VK_printf("*    Select Number & Press OK                                *\n");
			VK_printf("*============================================================*\n");
			return;
		default :
			break;
		}
		pMenuItem = pMenuItem->pNextItem;
		i++;
	}
	return;
}

static HINT32		P_UT_MENU_GetItemCount(UT_MENU_ITEM_T *pItem)
{
	UT_MENU_ITEM_T *pMenuItem;
	HINT32			i;

	if (pItem == NULL)
		return 0;

	i = 0;
	pMenuItem = pItem;
	while (pMenuItem != NULL)
	{
		switch (pMenuItem->menuType)
		{
		case MENU_ITEM_START :
			break;
		case MENU_ITEM_FUNC :
		case MENU_ITEM_SUBMENU :
			i++;
			break;
		case MENU_ITEM_END :
			return i;
		default :
			break;
		}
		pMenuItem = pMenuItem->pNextItem;
	}
	return i;
}

static HINT32		P_UT_MENU_AutoSelectItem(UT_MENU_ITEM_T *pItem)
{
	UT_MENU_ITEM_T *pMenuItem;
	UT_RESULT		result;
	HINT32			i;

	if (pItem == NULL)
		return 0;

	i = 0;
	pMenuItem = pItem;
	while (pMenuItem != NULL)
	{
		switch (pMenuItem->menuType)
		{
		case MENU_ITEM_START :
			break;
		case MENU_ITEM_FUNC :
			if (pMenuItem->pfnActionFunc != NULL)
			{
				VK_printf("enter %d. %s\n", i, pMenuItem->pszFuncName);
				result = pMenuItem->pfnActionFunc();
				UT_PrintResult(pMenuItem->pszFuncName, result == UT_OK);
			}
			break;
		case MENU_ITEM_SUBMENU :
			i++;
			break;
		case MENU_ITEM_END :
			goto submenu_routine;
		default :
			break;
		}
		pMenuItem = pMenuItem->pNextItem;
	}

submenu_routine:
	pMenuItem = pItem;
	while (pMenuItem != NULL)
	{
		switch (pMenuItem->menuType)
		{
		case MENU_ITEM_START :
			break;
		case MENU_ITEM_FUNC :
			break;
		case MENU_ITEM_SUBMENU :
			P_UT_MENU_AutoSelectItem(pMenuItem->pSubItem);
			i++;
			break;
		case MENU_ITEM_END :
			return i;
		default :
			break;
		}
		pMenuItem = pMenuItem->pNextItem;
	}

	return i;
}


static UT_RESULT P_UT_GetNumber(HINT32 *ulTestNumber)
{
	char	szCmd[MAX_CMD_LEN+1];

	*ulTestNumber = -1;
	do
	{
		szCmd[0] = 0;
		VK_printf(">> ");
		gets(szCmd);
	} while (!VK_strlen(szCmd));
	VK_sscanf(szCmd, "%d", ulTestNumber);

	return UT_OK;
}

static UT_MENU_ITEM_T	*P_UT_MENU_SelectItem(UT_MENU_ITEM_T *pItem, HINT32 ulTestNumber)
{
	HINT32				i;
	UT_RESULT			result;
	UT_MENU_ITEM_T		*pMenuItem;
	UT_MENU_ITEM_T		*pReturnItem;

	i=0;
	if (pItem == NULL)
		return NULL;

	pMenuItem = pItem;
	while (pMenuItem != NULL)
	{
		if (i>=ulTestNumber)
			break;
		pMenuItem = pMenuItem->pNextItem;
		i++;
	}
	if (pMenuItem == NULL)
		return NULL;

	VK_printf("enter %d. %s\n", i, pMenuItem->pszFuncName);

	switch (pMenuItem->menuType)
	{
	case MENU_ITEM_START:
		pReturnItem = NULL;
		break;
	case MENU_ITEM_SUBMENU:
		pReturnItem = (UT_MENU_ITEM_T*)pMenuItem->pSubItem;
		break;
	case MENU_ITEM_FUNC:
		if (pMenuItem->pfnActionFunc != NULL)
		{
			result = pMenuItem->pfnActionFunc();
			UT_PrintResult(pMenuItem->pszFuncName, result == UT_OK);
		}
		pReturnItem = pItem;
		break;
	default :
		break;
	}
	return pReturnItem;
}

DI_ERR_CODE UT_GetNumber(HUINT32 *ulTestNumber)
{
	char	szCmd[MAX_CMD_LEN+1];

	*ulTestNumber = -1;
	do
	{
		szCmd[0] = 0;
		VK_printf(">> ");
		gets(szCmd);
	} while (!VK_strlen(szCmd));
	VK_sscanf(szCmd, "%d", (int *)ulTestNumber);

	return UT_OK;
}
void UT_MENU_AddMainMenu(UT_MENU_ITEM_T *pItem)
{
	UT_MENU_ITEM_T *pNextItem;
	UT_MENU_ITEM_T *pNewItem;

	pNewItem = pItem;
	if (s_headMenuItem == NULL)
	{
		s_headMenuItem=pNewItem;
		goto end;
	}
	pNextItem = s_headMenuItem;
	while(pNextItem->pNextItem != NULL)
	{
		if (pNextItem->pNextItem->menuType == MENU_ITEM_END)
		{
			pItem->pNextItem = pNextItem->pNextItem;
			pNextItem->pNextItem = pItem;
			return;
		}
		pNextItem = pNextItem->pNextItem;
	}
	pNextItem->pNextItem = pNewItem;
end :
	return;
}

void UT_MENU_AddMenu(UT_MENU_ITEM_T *pItem)
{
	UT_MENU_ITEM_T *pNextItem;
	UT_MENU_ITEM_T *pNewItem;

	pNewItem = pItem;
	if (s_headMenuItem == NULL)
	{
		s_headMenuItem=pNewItem;
		goto end;
	}
	pNextItem = s_headMenuItem;
	while(pNextItem->pNextItem != NULL)
	{
		pNextItem = pNextItem->pNextItem;
	}
	pNextItem->pNextItem = pNewItem;
end :
	return;
}


int UT_Loop(void)
{
	HINT32 nTestNumber = 0;
	HINT32			nItemCount;
	HINT32			menuDepth;
	UT_MENU_ITEM_T	*pMenuItem;
	UT_MENU_ITEM_T	*pMenuDepth[8];

	VK_memset(pMenuDepth, 0, sizeof(UT_MENU_ITEM_T*)*8);
	menuDepth = 1;
	pMenuItem = s_headMenuItem;
	do
	{
		P_UT_MENU_Display(pMenuItem);
		nItemCount = P_UT_MENU_GetItemCount(pMenuItem);
		P_UT_GetNumber(&nTestNumber);
		if (nTestNumber == 100)
		{
			P_UT_MENU_AutoSelectItem(pMenuItem);
			continue;
		}
		if (nItemCount < nTestNumber)
			continue;
		pMenuDepth[menuDepth] = pMenuItem;
		pMenuItem = P_UT_MENU_SelectItem(pMenuItem, nTestNumber);
		if (pMenuItem == NULL)
			pMenuItem = pMenuDepth[--menuDepth];
		else if (pMenuDepth[menuDepth] != pMenuItem)
			menuDepth++;

		if (pMenuItem == NULL)
			break;
	} while (1);

	return 0;
}


void UT_PrintResult(char *func, HBOOL isPass)
{
	if (isPass)
		VK_printf("[RESULT] %s%s%s : %s\n", LINDEN_TRACE_FG_GREEN, "PASS", LINDEN_TRACE_FG_NORMAL, func ? func : "");
	else
		VK_printf("[RESULT] %s%s%s : %s\n", LINDEN_TRACE_FG_LIGHT_RED, "FAIL", LINDEN_TRACE_FG_NORMAL, func ? func : "");
}

