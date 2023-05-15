/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
#include <osd_font.h>
#include <osd_gfx.h>
#include <gwm_objdef.h>
#include <gwm_appobj.h>

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */
#define	MAX_MLIST_OBJECT		4

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */
struct tagMListObject
{
	GUIOBJ_BASIC_PROPERTY

	int item_count;
	int current_index;
	int num_rows, num_cols;
	HBOOL wrap_up, wrap_down, wrap_left, wrap_right;
};

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
static struct tagMListObject sMListArray[MAX_MLIST_OBJECT];
static char isAllocated[MAX_MLIST_OBJECT];

/* ======================================================================== */
/* Private Methods															*/
/* ======================================================================== */

static MLIST_OBJECT local_gwm_mlist_AllocObject(void)
{
	int i;

	for (i = 0; i < MAX_MLIST_OBJECT; i++)
	{
		if (!isAllocated[i])
		{
			GWM_assert(sMListArray[i].ID == -1);
			isAllocated[i] = 1;
			return &sMListArray[i];
		}
	}

	return NULL;
}


static void local_gwm_mlist_FreeObject(MLIST_OBJECT mlist)
{
	int i;

	GWM_assert(mlist);

	for (i = 0; i < MAX_MLIST_OBJECT; i++)
	{
		if (mlist == &sMListArray[i])
		{
			GWM_assert(isAllocated[i] == 1);
			sMListArray[i].ID = -1;
			isAllocated[i] = 0;
			return;
		}
	}

	GWM_assert(0);
}


static MLIST_OBJECT local_gwm_mlist_GetObject(int id)
{
	ONDK_GWM_GuiObject_t objlist;
	MLIST_OBJECT mlist;

	GWM_assert(g_pstCurrentApp);

	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	mlist = (MLIST_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);

	return mlist;
}


#define ROUND_DN(a, b) (((a) / (b)) * (b))

static int local_gwm_mlist_GetFirstVisibleIndex(MLIST_OBJECT mlist)
{
	if (mlist->item_count > 0 && mlist->num_rows > 0 && mlist->num_cols > 0)
	{
		return ROUND_DN(mlist->current_index, mlist->num_rows * mlist->num_cols);
	}
	else
	{
		return -1;
	}
}


static int local_gwm_mlist_GetLastVisibleIndex(MLIST_OBJECT mlist)
{
	if (mlist->item_count > 0 && mlist->num_rows > 0 && mlist->num_cols > 0)
	{
		int val = ROUND_DN(mlist->current_index, mlist->num_rows * mlist->num_cols) + mlist->num_rows * mlist->num_cols - 1;

		if (val >= mlist->item_count)
		{
			val = mlist->item_count - 1;
		}

		return val;
	}
	else
	{
		return -1;
	}
}


static ONDK_Result_t local_gwm_mlist_SetListData(MLIST_OBJECT mlist, int item_count, int num_rows, int num_cols, int current_index)
{
	if (item_count < 0 || num_rows < 0 || num_cols < 0)
	{
		return GWM_RET_OUT_OF_RANGE;
	}

	if (item_count > 0 && (current_index < 0 || item_count <= current_index))
	{
		return GWM_RET_OUT_OF_RANGE;
	}

	mlist->item_count = item_count;
	mlist->num_rows = num_rows;
	mlist->num_cols = num_cols;
	mlist->current_index = current_index;

	return GWM_RET_OK;
}


static void local_gwm_mlist_ArrowUp(MLIST_OBJECT mlist)
{
	int current, first, last, col;

	if (mlist->item_count <= 0 || mlist->num_rows <= 0 || mlist->num_cols <= 0)
	{
		return;
	}

	current = mlist->current_index;
	first = local_gwm_mlist_GetFirstVisibleIndex(mlist);
	last = local_gwm_mlist_GetLastVisibleIndex(mlist);
	col = (mlist->current_index - first) % mlist->num_cols;

	if ((current - first) < mlist->num_cols)
	{
		if (mlist->wrap_up)
		{
			if (col == 0)
			{
				if (current == 0)
				{
					current = mlist->item_count - 1;
					if ((mlist->item_count % (mlist->num_rows * mlist->num_cols)) > mlist->num_cols)
					{
						while ((current % mlist->num_cols) != (mlist->num_cols - 1))
						{
							--current;
						}
					}
				}
				else
				{
					--current;
				}
			}
			else
			{
				current = last;
				while ((current % mlist->num_cols) != (col - 1))
				{
					--current;
				}
			}
		}
		else
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_UP_NOMORE, HANDLE_NULL, mlist->ID, mlist->current_index, 0, NULL);
			return;
		}
	}
	else
	{
		current -= mlist->num_cols;
	}

	if (mlist->current_index != current)
	{
		mlist->current_index = current;
		GWM_assert(0 <= mlist->current_index && mlist->current_index < mlist->item_count);

		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, HANDLE_NULL, mlist->ID, mlist->current_index, 0, NULL);
	}
}

static void local_gwm_mlist_ArrowDown(MLIST_OBJECT mlist)
{
	int current, first, last, col;

	if (mlist->item_count <= 0 || mlist->num_rows <= 0 || mlist->num_cols <= 0)
	{
		return;
	}

	current = mlist->current_index;
	first = local_gwm_mlist_GetFirstVisibleIndex(mlist);
	last = local_gwm_mlist_GetLastVisibleIndex(mlist);
	col = (mlist->current_index - first) % mlist->num_cols;

	if ((last - current) < mlist->num_cols)
	{
		if (mlist->wrap_down)
		{
			if (col == (mlist->num_cols - 1))
			{
				current = first + mlist->num_rows * mlist->num_cols;
				if (current >= mlist->item_count)
				{
					current = 0;
				}
			}
			else
			{
				current = first + col + 1;
				if (current >= mlist->item_count)
				{
					current = 0;
				}
			}
		}
		else
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_DOWN_NOMORE, HANDLE_NULL, mlist->ID, mlist->current_index, 0, NULL);
			return;
		}
	}
	else
	{
		current += mlist->num_cols;
	}

	if (mlist->current_index != current)
	{
		mlist->current_index = current;
		GWM_assert(0 <= mlist->current_index && mlist->current_index < mlist->item_count);

		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, HANDLE_NULL, mlist->ID, mlist->current_index, 0, NULL);
	}
}


static void local_gwm_mlist_ArrowLeft(MLIST_OBJECT mlist)
{
	int current, first, last, row, col;

	if (mlist->item_count <= 0 || mlist->num_rows <= 0 || mlist->num_cols <= 0)
	{
		return;
	}

	current = mlist->current_index;
	first = local_gwm_mlist_GetFirstVisibleIndex(mlist);
	last = local_gwm_mlist_GetLastVisibleIndex(mlist);
	row = (mlist->current_index - first) / mlist->num_cols;
	col = (mlist->current_index - first) % mlist->num_cols;

	if (col == 0)
	{
		first -= mlist->num_rows * mlist->num_cols;
		if (first < 0)
		{
			if (mlist->wrap_left)
			{
				current = ROUND_DN(mlist->item_count - 1, mlist->num_rows * mlist->num_cols)
						+ row * mlist->num_cols + mlist->num_cols - 1;
				if (current >= mlist->item_count)
				{
					current = mlist->item_count - 1;
				}
			}
			else
			{
				//GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_LEFT_NOMORE, HANDLE_NULL, mlist->ID, mlist->current_index, 0);
				return;
			}
		}
		else
		{
			current = first + (row + 1) * mlist->num_cols - 1;
		}
	}
	else
	{
		--current;
	}

	if (mlist->current_index != current)
	{
		mlist->current_index = current;
		GWM_assert(0 <= mlist->current_index && mlist->current_index < mlist->item_count);

		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, HANDLE_NULL, mlist->ID, mlist->current_index, 0, NULL);
	}
}


static void local_gwm_mlist_ArrowRight(MLIST_OBJECT mlist)
{
	int current, first, last, row, col;

	if (mlist->item_count <= 0 || mlist->num_rows <= 0 || mlist->num_cols <= 0)
	{
		return;
	}

	current = mlist->current_index;
	first = local_gwm_mlist_GetFirstVisibleIndex(mlist);
	last = local_gwm_mlist_GetLastVisibleIndex(mlist);
	row = (mlist->current_index - first) / mlist->num_cols;
	col = (mlist->current_index - first) % mlist->num_cols;

	if (col == (mlist->num_cols - 1))
	{
		first += mlist->num_rows * mlist->num_cols;
		if (first >= mlist->item_count)
		{
			if (mlist->wrap_right)
			{
				current = row * mlist->num_cols;
			}
			else
			{
				//GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_RIGHT_NOMORE, HANDLE_NULL, mlist->ID, mlist->current_index, 0);
				return;
			}
		}
		else
		{
			current = first + row * mlist->num_cols;
			if (current >= mlist->item_count)
			{
				current = mlist->item_count - 1;
				while (current % mlist->num_cols)
				{
					--current;
				}
			}
		}
	}
	else if (current == (mlist->item_count - 1))
	{
		if (mlist->wrap_right)
		{
			current = row * mlist->num_cols;
		}
		else
		{
			//GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_RIGHT_NOMORE, HANDLE_NULL, mlist->ID, mlist->current_index, 0);
			return;
		}
	}
	else
	{
		++current;
	}

	if (mlist->current_index != current)
	{
		mlist->current_index = current;
		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, HANDLE_NULL, mlist->ID, mlist->current_index, 0, NULL);
	}

	GWM_assert(mlist->current_index < mlist->item_count);
	if (mlist->item_count > 0)
	{
		GWM_assert(mlist->current_index >= 0);
	}
}


static void local_gwm_mlist_PagePlus(MLIST_OBJECT mlist)
{
	int current, first, npagenum;

	npagenum = mlist->num_rows*mlist->num_cols;
	if (mlist->item_count <= npagenum || mlist->num_rows <= 0 || mlist->num_cols <= 0)
	{
		return;
	}

	current = mlist->current_index;
	first = local_gwm_mlist_GetFirstVisibleIndex(mlist);

	if(first + npagenum >= mlist->item_count)
	{
		if(!mlist->wrap_right)
			return;

		current = current - first;
	}
	else
	{
		current += npagenum;
		if(current >= mlist->item_count)
			current = mlist->item_count - 1;
	}

	if (mlist->current_index != current)
	{
		mlist->current_index = current;
		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, HANDLE_NULL, mlist->ID, mlist->current_index, 0, NULL);
	}

	GWM_assert(mlist->current_index < mlist->item_count);
	if (mlist->item_count > 0)
	{
		GWM_assert(mlist->current_index >= 0);
	}
}

static void local_gwm_mlist_PageMinus(MLIST_OBJECT mlist)
{
	int current, first, npagenum, newfirst;

	npagenum = mlist->num_rows*mlist->num_cols;
	if (mlist->item_count <= npagenum || mlist->num_rows <= 0 || mlist->num_cols <= 0)
	{
		return;
	}

	current = mlist->current_index;
	first = local_gwm_mlist_GetFirstVisibleIndex(mlist);

	if(first < npagenum)
	{
		if(!mlist->wrap_left)
			return;

		newfirst = ROUND_DN(mlist->item_count - 1, mlist->num_rows * mlist->num_cols);
		current = newfirst + (current - first);
		if(current >= mlist->item_count)
			current = mlist->item_count - 1;
	}
	else
	{
		current -= npagenum;
		if(current < 0)
			current = 0;
	}

	if (mlist->current_index != current)
	{
		mlist->current_index = current;
		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, HANDLE_NULL, mlist->ID, mlist->current_index, 0, NULL);
	}

	GWM_assert(mlist->current_index < mlist->item_count);
	if (mlist->item_count > 0)
	{
		GWM_assert(mlist->current_index >= 0);
	}
}

static ONDK_Result_t local_gwm_mlist_ProcessKey(MLIST_OBJECT mlist, KEY_Code_t arg)
{
	ONDK_Result_t result = GWM_MESSAGE_BREAK;

	switch (arg) {
	case KEY_ARROWUP:
		local_gwm_mlist_ArrowUp(mlist);
		break;

	case KEY_ARROWDOWN:
		local_gwm_mlist_ArrowDown(mlist);
		break;

	case KEY_ARROWLEFT:
		local_gwm_mlist_ArrowLeft(mlist);
		break;

	case KEY_ARROWRIGHT:
		local_gwm_mlist_ArrowRight(mlist);
		break;

	case KEY_CH_PLUS:
		local_gwm_mlist_PagePlus(mlist);
		break;

	case KEY_CH_MINUS:
		local_gwm_mlist_PageMinus(mlist);
		break;

	case KEY_OK:
		if (mlist->item_count > 0 && mlist->num_rows > 0 && mlist->num_cols > 0)
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_SELECTED, HANDLE_NULL, mlist->ID, mlist->current_index, 0, NULL);
		}
		break;

	default:
		result = GWM_MESSAGE_PASS;
		break;
	}

	return result;
}


static ONDK_Result_t	local_gwm_mlist_Proc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	MLIST_OBJECT mlist;
	ONDK_Region_t area;
	ONDK_Rect_t rect;

	GWM_assert(object->type == GUIOBJ_MLIST);
	mlist = (MLIST_OBJECT)object;

	switch (proc_msg) {
	case MT_DESTROY:
		INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
		ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
		local_gwm_mlist_FreeObject(mlist);
		return GWM_MESSAGE_BREAK;

	case MT_DRAW:
		ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
		INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
		if (object->objDrawMethod != NULL)
		{
			object->objDrawMethod(ONDK_GWM_GetSurface(), &area, &rect, object, NULL, arg);
		}
		return GWM_MESSAGE_BREAK;

	case MT_RELOCATE:
		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		mlist->rect = *((ONDK_Rect_t *)arg);
		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		return GWM_MESSAGE_BREAK;

	case MT_CHANGE_TEXT:
	case MT_SET_VALUE:
		/* do nothing */
		return GWM_MESSAGE_BREAK;

	case MT_KEYDOWN:
		return local_gwm_mlist_ProcessKey(mlist, arg);

	case MT_KEYUP:
		return GWM_MESSAGE_BREAK;

	case MT_GET_FOCUS:
		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		return GWM_MESSAGE_BREAK;

	case MT_LOST_FOCUS:
		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
		return GWM_MESSAGE_BREAK;

	default:
		break;
	}

	return GWM_MESSAGE_PASS;
}


/* ======================================================================== */
/* Public Methods															*/
/* ======================================================================== */

void gwm_mlist_InitClass(void)
{
	int		i;

	for (i = 0; i < MAX_MLIST_OBJECT; i++)
	{
		sMListArray[i].ID = -1;
		isAllocated[i] = 0;
	}
}


ONDK_Result_t GWM_MList_Create(int id, ONDK_Rect_t *rect, int item_count,
		int num_rows, int num_cols, int current_index)
{
	MLIST_OBJECT mlist;
	ONDK_Result_t hresult;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist != NULL)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("Alread exists!\n"));
		return GWM_RET_LINK_CONFLICT;
	}

	mlist = local_gwm_mlist_AllocObject();
	if (mlist == NULL)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("Out of memeory\n"));
		return GWM_RET_OUT_OF_MEMORY;
	}

	hresult = local_gwm_mlist_SetListData(mlist, item_count, num_rows, num_cols, current_index);
	if (hresult != GWM_RET_OK)
	{
		local_gwm_mlist_FreeObject(mlist);
		return hresult;
	}

	mlist->wrap_up = mlist->wrap_down = mlist->wrap_left = mlist->wrap_right = FALSE;

	mlist->link = NULL;
	mlist->rect = *rect;
	mlist->ID = id;
	mlist->type = GUIOBJ_MLIST;
	mlist->focus = GUIOBJ_NORMAL;
	mlist->Proc = local_gwm_mlist_Proc;
	mlist->objDrawMethod = NULL;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)mlist, 0);
	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)mlist);
	ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);

	return hresult;
}


int GWM_MList_GetCurrentIndex(int id)
{
	MLIST_OBJECT mlist;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist == NULL)
	{
		return -1;
	}

	GWM_assert(mlist->type == GUIOBJ_MLIST);

	return mlist->current_index;
}

ONDK_Result_t GWM_MList_SetCurrentIndex(int id, int val)
{
	MLIST_OBJECT mlist;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	GWM_assert(mlist->type == GUIOBJ_MLIST);

	if(mlist->item_count > val && val >= 0)
	{
		mlist->current_index = val;
		ONDK_GWM_APP_InvalidateRelativeRect(mlist->rect.x, mlist->rect.y, mlist->rect.w, mlist->rect.h);
	}
	else
	{
		return GWM_RET_OUT_OF_RANGE;
	}

	return GWM_RET_OK;
}

int GWM_MList_GetFirstVisibleIndex(int id)
{
	MLIST_OBJECT mlist;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist == NULL)
	{
		return -1;
	}

	GWM_assert(mlist->type == GUIOBJ_MLIST);

	return local_gwm_mlist_GetFirstVisibleIndex(mlist);
}


int GWM_MList_GetLastVisibleIndex(int id)
{
	MLIST_OBJECT mlist;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist == NULL)
	{
		return -1;
	}

	GWM_assert(mlist->type == GUIOBJ_MLIST);

	return local_gwm_mlist_GetLastVisibleIndex(mlist);
}


int GWM_MList_GetItemCount(int id)
{
	MLIST_OBJECT mlist;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist == NULL)
	{
		return -1;
	}

	GWM_assert(mlist->type == GUIOBJ_MLIST);

	return mlist->item_count;
}


int GWM_MList_GetRowCount(int id)
{
	MLIST_OBJECT mlist;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist == NULL)
	{
		return -1;
	}

	GWM_assert(mlist->type == GUIOBJ_MLIST);

	return mlist->num_rows;
}


int GWM_MList_GetColumnCount(int id)
{
	MLIST_OBJECT mlist;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist == NULL)
	{
		return -1;
	}

	GWM_assert(mlist->type == GUIOBJ_MLIST);

	return mlist->num_cols;
}


ONDK_Result_t GWM_MList_SetListData(int id, int item_count, int num_rows, int num_cols, int current_index)
{
	MLIST_OBJECT mlist;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	GWM_assert(mlist->type == GUIOBJ_MLIST);

	return local_gwm_mlist_SetListData(mlist, item_count, num_rows, num_cols, current_index);
}


ONDK_Result_t GWM_MList_SetWrapAroundMode(int id, HBOOL up, HBOOL down, HBOOL left, HBOOL right)
{
	MLIST_OBJECT mlist;

	mlist = local_gwm_mlist_GetObject(id);
	if (mlist == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	GWM_assert(mlist->type == GUIOBJ_MLIST);

	mlist->wrap_up = up;
	mlist->wrap_down = down;
	mlist->wrap_left = left;
	mlist->wrap_right = right;
}



void GWM_MList_FreeList(int id)
{
	MLIST_OBJECT mlist;

	mlist = (MLIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (mlist)
	{
		local_gwm_mlist_FreeObject(mlist);
	}
}


