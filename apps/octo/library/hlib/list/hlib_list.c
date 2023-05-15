/*
* hxlist.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "_hlib_stdlib.h"
#include "_hlib_list.h"
#include "_hlib_mem.h"


STATIC void* hlib_list_Malloc(void);
STATIC void* hlib_list_Realloc(void* oldData,HUINT32 size);
STATIC HxList_t* hlib_list_Dtar(HxList_t* list , HUINT32 pos);

HxList_t* HLIB_LIST_Insert(HxList_t *list , void* data , HINT32 position)
{
	HxList_t *new_list;
	HxList_t *tmp_list ;

	if(position < 0)
		return HLIB_LIST_Append(list, data);
	else if(position == 0)
		return HLIB_LIST_Prepend(list, data);

	tmp_list = hlib_list_Dtar(list, position);

	if(!tmp_list)
		return HLIB_LIST_Append(list,data);

	new_list = hlib_list_Malloc();
	new_list->data = data;
	new_list->next = NULL;
	new_list->prev = tmp_list->prev;

	if(tmp_list->prev)
		tmp_list->prev->next = new_list;

	new_list->next = tmp_list;
	tmp_list->prev = new_list;

	if(tmp_list == list)
		return new_list;
	else
		return list;
}


HxList_t* HLIB_LIST_Append(HxList_t *list , void* data)
{
	HxList_t *new_list;
	HxList_t *last;

	new_list = hlib_list_Malloc();
	new_list->data = data;
	new_list->next = NULL;
	if(list)
	{
		last = HLIB_LIST_Last(list);
		last->next = new_list;
		new_list ->prev = last;
		return list;
	}
	else
	{
		new_list->prev = NULL;
		return new_list;
	}
}


HxList_t* HLIB_LIST_AppendSortEx(HxList_t *list , void* data, HINT32 (*cbcmp)(void *,void *))
{
	HxList_t 	*new_list;
	HxList_t 	*node;
	HINT32		nCnt = 0;
	HINT32		nComp = 0;

	if(list)
	{
		node = list;
		while(node)
		{
			nComp = cbcmp(data, node->data);
			if(nComp > 0)
			{
				break;
			}
			node = node->next;
			nCnt++;
		}

		list = HLIB_LIST_Insert(list, data, nCnt);
		return list;
	}
	else
	{
		new_list = hlib_list_Malloc();
		new_list->data = data;
		new_list->next = NULL;
		new_list->prev = NULL;
		return new_list;
	}
}



HxList_t* HLIB_LIST_GetListItem(HxList_t* list , HUINT32 position)
{
	while((position-- > 0) && list)
		list = list->next;
	return list;
}


HxList_t* HLIB_LIST_First(HxList_t *list)
{
	if(list)
	{
		while(list->prev)
			list = list->prev;
	}
	return list;
}

HxList_t* HLIB_LIST_Prev(HxList_t *list)
{
	HxList_t *prev = NULL;
	if(list)
	{
		prev = list->prev;
	}
	return prev;
}


HxList_t* HLIB_LIST_Next(HxList_t *list)
{
	HxList_t *next = NULL;
	if(list)
	{
		next = list->next;
	}
	return next;
}



HxList_t* HLIB_LIST_Last(HxList_t *list)
{
	if(list)
	{
		while(list->next)
			list = list->next;
	}
	return list;
}


void* HLIB_LIST_Data(HxList_t *list)
{
	if(list)
	{
		return list->data;
	}
	return NULL;
}


HxList_t* HLIB_LIST_Find(HxList_t *list , const void* data)
{
	while(list)
	{

		if(list->data == data)
			break;
		list = list->next;
	}
	return list;
}


HxList_t* HLIB_LIST_FindEx		(HxList_t *list, const void* user_data, HBOOL (*cbcmp)(void *user_data,void *list_data))
{
	HBOOL bCmp = FALSE;

	if(cbcmp == NULL)
	{
		return NULL;
	}

	while(list)
	{
		bCmp = cbcmp((void *)user_data, (void *)list->data);
		if(bCmp == TRUE)
			break;
		list = list->next;
	}
	return list;
}


HUINT32 HLIB_LIST_Length(HxList_t *list)
{
	HUINT32 length;

	if ( list == NULL )
		return 0;

	length = 0;
	while(list)
	{
		// printf("root value : %x\n",(HUINT32)list);
		length++;
		list = list->next;
	}
	return length;
}



HxList_t* HLIB_LIST_Remove(HxList_t *list , const void* data)
{
	HxList_t *tmp;

	tmp = list;
	while(tmp)
	{
		if(tmp->data != data)
			tmp = tmp->next;
		else
		{
			if(tmp->prev)
				tmp->prev->next = tmp->next;
			if(tmp->next)
				tmp->next->prev = tmp->prev;

			if(list == tmp)
				list = list->next;

			HLIB_MEM_Free(tmp);
			break;
		}
	}
	return list;
}


HxList_t* HLIB_LIST_RemoveAll(HxList_t *list)
{
	HxList_t *tmp = HLIB_LIST_First(list);
	while(tmp)
	{
		HxList_t *next = tmp->next;
		HLIB_MEM_Free(tmp);
		tmp = next;
	}
	return tmp;
}



HxList_t* HLIB_LIST_RemoveAllFunc(HxList_t *list,  void (*free_func)(void *data))
{
	HxList_t *tmp = HLIB_LIST_First(list);
	HxList_t *next = NULL;
	while(tmp)
	{
		if(free_func)
		{
			free_func(tmp->data);
			tmp->data = NULL;
		}
		next = tmp->next;
		HLIB_MEM_Free(tmp);
		tmp = next;
	}
	return tmp;
}

HxList_t*	HLIB_LIST_RemoveIf(HxList_t *list, void *userData, HBOOL (*condition_func)(void *userdata, void *data), void (*free_func)(void *data))
{
	HBOOL		bCondition;
	HxList_t	*head = HLIB_LIST_First(list);
	HxList_t	*tmp;
	HxList_t	*next = NULL;

	if (condition_func == NULL)
	{
		return head;
	}

	tmp = head;
	while (tmp)
	{
		bCondition = condition_func(userData, tmp->data);
		if (bCondition)
		{
			if(tmp->prev)
				tmp->prev->next = tmp->next;
			if(tmp->next)
				tmp->next->prev = tmp->prev;

			if (tmp == head)
			{
				head = tmp->next;
			}

			if (free_func)
			{
				free_func(tmp->data);
				tmp->data = NULL;
			}

			next = tmp->next;
			HLIB_MEM_Free(tmp);
			tmp = next;
		} else
		{
			tmp = tmp->next;
		}
	}

	return head;
}

STATIC void* hlib_list_Malloc(void)
{
	HUINT32 size = sizeof(HxList_t);
	void* mem = HLIB_MEM_Malloc(size);
	if(mem == NULL)
	{
		assert(mem != NULL);
		return NULL;
	}
	if(mem)
		memset(mem,0,size);
	return mem;
}

STATIC void* hlib_list_Realloc(void* oldData,HUINT32 size)
{
	void* mem = HLIB_MEM_Realloc(oldData,size);
	if(mem == NULL)
	{
		assert(mem != NULL);
		return NULL;
	}
	if(mem)
		memset(mem,0,size);
	return mem;

}



HxList_t* HLIB_LIST_Prepend(HxList_t* list , void* data)
{
	HxList_t* new_list;

	new_list = hlib_list_Malloc();
	new_list->data = data;
	new_list->next = list;

	if(list)
	{
		new_list->prev = list->prev;
		if(list->prev)
			list->prev->next = new_list;

		list->prev = new_list;
	}
	else
		new_list->prev = NULL;

	return new_list;
}

void HLIB_LIST_Foreach(HxList_t *list , void (*func)(void *))
{
	while(list)
	{
		func(list->data);
		list = list->next;
	}
}

HxList_t*  HLIB_LIST_ForeachEx(HxList_t *list , HBOOL (*func)(void *data, void *userdata), void *userdata)
{
	while(list)
	{
		if(func(list->data, userdata) == TRUE)
		{
			break;
		}
		list = list->next;
	}

	return list;
}


STATIC HxList_t* hlib_list_Dtar(HxList_t* list , HUINT32 pos)
{
	while((pos-- > 0) && list)
		list = list->next;
	return list;
}

void	HxList_DEFINED_BUT_NOT_USED (void)
{
	(void)hlib_list_Realloc;
}


