#ifndef __UNITTEST_H___
#define __UNITTEST_H___

#include "htype.h"
#include "di_err.h"

typedef enum UT_RESULT_e
{
	UT_OK,
	UT_FAIL,
	UT_MAX	
} UT_RESULT;

typedef enum menu_type
{
	MENU_ITEM_START,
	MENU_ITEM_FUNC,
	MENU_ITEM_SUBMENU,
	MENU_ITEM_END,
	MENU_ITEM_MAX
} MENU_TYPE_E;

typedef UT_RESULT	(*pfn_ActionFunc)(void);

typedef struct UT_MENU_ITEM_t
{
	MENU_TYPE_E							menuType;
	pfn_ActionFunc								pfnActionFunc;
	struct UT_MENU_ITEM_t	*pSubItem;
	char										*pszFuncName;
	struct UT_MENU_ITEM_t	*pNextItem;
	int						expected_time;
} UT_MENU_ITEM_T;

#define MENU_START				{MENU_ITEM_START, NULL, NULL, NULL, NULL, 0}
#define MENU_END				{MENU_ITEM_END, NULL, NULL, NULL, NULL, 0}
#define MENU_FUNC(func, sec)	{MENU_ITEM_FUNC, func, NULL, #func, NULL, sec}
#define MENU_SUB(menu)			{MENU_ITEM_SUBMENU, NULL, menu, #menu, NULL, 0}


DI_ERR_CODE UT_GetNumber(HUINT32 *ulTestNumber);
void UT_MENU_AddMainMenu(UT_MENU_ITEM_T *pItem);
void UT_MENU_AddMenu(UT_MENU_ITEM_T *pItem);
void UT_PrintResult(char *func, HBOOL isPass);
int UT_Loop(void);
int UT_Init(void);

#define DONE_ERR(error) {						\
		if(error) {						\
			PrintError("ERROR %s,%d\n", __FUNCTION__, __LINE__); \
			goto done;				\
		}							\
	}

#endif /* __UNITTEST_H___ */

