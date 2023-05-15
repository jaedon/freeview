/*************************************************************
 * @file		_hlib_list.h
 * @date		2011/01/20
 * @author	Moon-Young Kim (mykim@humaxdigital.com)
 * @brief		HUMAX common library header, related with list functon.

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef __HLIB_LIST_H__
#define __HLIB_LIST_H__

#include "htype.h"
typedef struct _HxList HxList_t;

struct _HxList
{
	void* data;
	HxList_t*next;
	HxList_t *prev;
};



/**
 * HxList_insert : list 에 아이템을 추가
 * list : 처음시작 HxList_t 의 pointer 로 항상 유지 되어야 한다.
 * data: list 에 추가되는 아이템
 * position : 삽입할 위치값.
 * Returns : 시작 HxList_t 를 리턴. 이값을 항상 유지하는것이 중요.
 */
HxList_t* HLIB_LIST_Insert			(HxList_t *list , void* data , HINT32 position);




/**
 * HLIB_LIST_Append : list 의 마지막에 list 추가
 * list: 시작 list 의 pointer
 * data: list 에 추가되는 아이템
 * Returns: 시작 HxList_t 를 리턴. 이값을 항상 유지하는것이 중요.
 */
HxList_t* HLIB_LIST_Append			(HxList_t *list , void* data);



/**
 * HLIB_LIST_AppendSortEx : list 중 cmp에서 1이상의 값을 return 하는 위치에 추가
 * list: 시작 list 의 pointer
 * data: list 에 추가되는 아이템
 * cbcmp: 비교되는 함수 포인터
 * Returns: 시작 HxList_t 를 리턴. 이값을 항상 유지하는것이 중요.
 */
HxList_t* HLIB_LIST_AppendSortEx(HxList_t *list , void* data, HINT32 (*cbcmp)(void *,void *));



/**
 * HLIB_LIST_GetListItem: position 의 위치에 해당되는 아이템을 얻어온다.
 * list: 시작 list 의 pointer
 * position : 얻어올 list 내의 위치
 * Returns: list 내의 position 위치의 HxList_t 를 리턴
 */
HxList_t* HLIB_LIST_GetListItem	(HxList_t* list , HUINT32 position);





/**
 * HLIB_LIST_Remove: list 에서 data 지우기
 * list: 시작 list 의 pointer
 * Returns: 시작 HxList_t 를 리턴.
 */
HxList_t* HLIB_LIST_Remove			(HxList_t *list , const void* data);





/**
 * HLIB_LIST_First: list 모든 아이템 지우기
 * list: 시작 list 의 pointer
 * Returns: 항상 NULL 리턴.
 */
HxList_t* HLIB_LIST_RemoveAll		(HxList_t *list);




/**
 * HLIB_LIST_First: list 모든 아이템 지우기 (지우기 전에 function에 data를 지우도록 callback 등록 가능)
 * list: 시작 list 의 pointer
 * free_func: data를 지우기 위한 func callback
 * Returns: 항상 NULL 리턴.
 */
HxList_t* HLIB_LIST_RemoveAllFunc(HxList_t *list,  void (*free_func)(void *data));


/**
 * 주어진 List의 처음 부터 condition_func의 결과 값을 토대로 하나씩 지워 나감.
 * Result List는 완전 새로운 list로 구성이 됨으로, coding상의 주의.
 */
HxList_t*	HLIB_LIST_RemoveIf(HxList_t *list, void *userData, HBOOL (*condition_func)(void *userdata, void *data), void (*free_func)(void *data));

/**
 * HLIB_LIST_First: list 의 시작 HxList_t 를 얻기.
 * list: 시작 list 의 pointer
 * Returns: first HxList_t 리턴.
 */
HxList_t* HLIB_LIST_First			(HxList_t *list);



/**
 * list 의 prev HxList_t 를 얻기.
 * list: list 의 pointer
 * Returns: prev HxList_t 리턴.
 */
HxList_t* HLIB_LIST_Prev			(HxList_t *list);



/**
 * list 의 next HxList_t 를 얻기.
 * list: list 의 pointer
 * Returns: next HxList_t 리턴.
 */
HxList_t* HLIB_LIST_Next			(HxList_t *list);



/**
 * HLIB_LIST_First: list 의 last HxList_t 를 얻기.
 * list: 시작 list 의 pointer
 * Returns: last HxList_t 리턴.
 */
HxList_t* HLIB_LIST_Last			(HxList_t *list);


/**
 * HLIB_LIST_First: list 의 data 를 얻기.
 * list: 시작 list 의 pointer
 * Returns: data void* 리턴.
 */
void* HLIB_LIST_Data(HxList_t *list);


/**
 * HLIB_LIST_First: list 에서 data 와 일치하는 HxList_t 찾기
 * list: 시작 list 의 pointer
 * Returns: 찾아낸 HxList_t 리턴.
 */
HxList_t* HLIB_LIST_Find			(HxList_t *list , const void* data);


/**
 * HLIB_LIST_First:
 * list: 시작 list 의 pointer / data
 * Returns:  HxList_t 리턴.
 */

HxList_t* HLIB_LIST_Prepend		(HxList_t* list , void* data);



/**
 * HLIB_LIST_First: list 의 length count 얻기.
 * list: 시작 list 의 pointer
 * Returns: list 의 length 리턴.
 */
HUINT32 HLIB_LIST_Length			(HxList_t *list);


/**
 * HLIB_LIST_FindEx: list에 call back을 이용한, list 찾기
 * list: 시작 list 의 pointer
 * Returns: 찾아낸 HxList_t 리턴.
 */
HxList_t* HLIB_LIST_FindEx 	(HxList_t *list, const void* user_data, HBOOL (*cbcmp)(void *user_data,void *list_data));


/**
 * HLIB_LIST_Foreach: list에 call back을 이용하여 foreach함.
 * list: 시작 list 의 pointer
 * func: foreach하면서 수행할 function pointer
 * Returns: void
 */
void HLIB_LIST_Foreach(HxList_t *list , void (*func)(void *));

/**
 * HLIB_LIST_Foreach: list에 call back을 이용하여 foreach함.
 * list: 시작 list 의 pointer
 * func: foreach하면서 수행할 function pointer
 * userdata : 인자로 들어갈 userdata
 * Returns: return TRUE로 끝났을 때 list
 */
HxList_t* HLIB_LIST_ForeachEx(HxList_t *list , HBOOL (*func)(void *data, void *userdata), void *userdata);

#endif	/* !__HLIB_LIST_H__ */
