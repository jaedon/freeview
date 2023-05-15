/*
* rxlist.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rxstdlib.h"
#include "rxlist.h"



STATIC void* _rxlist_Malloc();
STATIC void* _rxlist_Realloc(void* oldData,HUINT32 size);
STATIC RxList_t* _rxlist_Dtar(RxList_t* list , HUINT32 pos);
RxList_t* RxList_Prepend(RxList_t* list , void* data);


RxList_t* RxList_Insert(RxList_t *list , void* data , HINT32 position)
{
	RxList_t *new_list;
	RxList_t *tmp_list ;

	if(position < 0)
		return RxList_Append(list,data);
	else if(position == 0)
		return RxList_Prepend(list,data);

	tmp_list = _rxlist_Dtar(list , position);

	if(!tmp_list)
		return RxList_Append(list,data);

	new_list = _rxlist_Malloc();
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


RxList_t* RxList_Append(RxList_t *list , void* data)
{
	RxList_t *new_list;
	RxList_t *last;

	new_list = _rxlist_Malloc();
	new_list->data = data;
	new_list->next = NULL;
	if(list)
	{
		last = RxList_Last(list);
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


RxList_t* RxList_AppendSortEx(RxList_t *list , void* data, HINT32 (*cbcmp)(void *,void *))
{
	RxList_t 	*new_list;
	RxList_t 	*node;
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

		list = RxList_Insert(list, data, nCnt);
		return list;
	}
	else
	{
		new_list = _rxlist_Malloc();
		new_list->data = data;
		new_list->next = NULL;
		new_list->prev = NULL;
		return new_list;
	}
}



RxList_t* RxList_GetListItem(RxList_t* list , HUINT32 position)
{
	while((position-- > 0) && list)
		list = list->next;
	return list;
}


RxList_t* RxList_First(RxList_t *list)
{
	if(list)
	{
		while(list->prev)
			list = list->prev;
	}
	return list;
}


RxList_t* RxList_Last(RxList_t *list)
{
	if(list)
	{
		while(list->next)
			list = list->next;
	}
	return list;
}


void* RxList_Data(RxList_t *list)
{
	if(list)
	{
		return list->data;
	}
	return NULL;
}


RxList_t* RxList_Find(RxList_t *list , const void* data)
{
	while(list)
	{

		if(list->data == data)
			break;
		list = list->next;
	}
	return list;
}


RxList_t* RxList_FindEx		(RxList_t *list, const void* data, HBOOL (*cbcmp)(void *,void *))
{
	HBOOL bCmp = FALSE;

	if(cbcmp == NULL || cbcmp == NULL)
	{
		return NULL;
	}

	while(list)
	{
		bCmp = cbcmp((void *)data, (void *)list->data);
		if(bCmp == TRUE)
			break;
		list = list->next;
	}
	return list;
}


HUINT32 RxList_Length(RxList_t *list)
{
	HUINT32 length;

	length = 0;
	while(list)
	{
		// printf("root value : %x\n",(HUINT32)list);
		length++;
		list = list->next;
	}
	return length;
}



RxList_t* RxList_Remove(RxList_t *list , const void* data)
{
	RxList_t *tmp;

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

			RxSTD_Free(tmp);
			break;
		}
	}
	return list;
}


RxList_t* RxList_RemoveAll(RxList_t *list)
{
	RxList_t *tmp = list;
	while(tmp)
	{
		RxList_t *next = tmp->next;
		RxSTD_Free(tmp);
		tmp = next;
	}
	return tmp;
}


STATIC void* _rxlist_Malloc()
{
	HUINT32 size = sizeof(RxList_t);
	void* mem = RxSTD_Alloc(size);
	if(mem == NULL)
	{
		assert(mem != NULL);
		return NULL;
	}
	if(mem)
		memset(mem,0,size);
	return mem;
}

STATIC void* _rxlist_Realloc(void* oldData,HUINT32 size)
{
	void* mem = RxSTD_Realloc(oldData,size);
	if(mem == NULL)
	{
		assert(mem != NULL);
		return NULL;
	}
	if(mem)
		memset(mem,0,size);
	return mem;

}



RxList_t* RxList_Prepend(RxList_t* list , void* data)
{
	RxList_t* new_list;

	new_list = _rxlist_Malloc();
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

void RxList_Foreach(RxList_t *list , void (*func)(void *))
{
	while(list)
	{
		func(list->data);
		list = list->next;
	}
}



STATIC RxList_t* _rxlist_Dtar(RxList_t* list , HUINT32 pos)
{
	while((pos-- > 0) && list)
		list = list->next;
	return list;
}
