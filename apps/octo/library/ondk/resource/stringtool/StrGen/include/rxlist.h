/*************************************************************
 * @file		rxlist.h
 * @date		2011/01/20
 * @author	Moon-Young Kim (mykim@humaxdigital.com)
 * @brief		HUMAX common library header, related with list functon.

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef HXLIST_H_
#define HXLIST_H_

#include "rxtype.h"
typedef struct _RxList RxList_t;

struct _RxList
{
	void* data;
	RxList_t*next;
	RxList_t *prev;
};




/**
 * MHList_insert : list 에 아이템을 추가
 * list : 처음시작 MHList_t 의 pointer 로 항상 유지 되어야 한다.
 * data: list 에 추가되는 아이템
 * position : 삽입할 위치값.
 * Returns : 시작 MHList_t 를 리턴. 이값을 항상 유지하는것이 중요.
 */
RxList_t* RxList_Insert			(RxList_t *list , void* data , HINT32 position);




/**
 * MHList_Append : list 의 마지막에 list 추가
 * list: 시작 list 의 pointer
 * data: list 에 추가되는 아이템
 * Returns: 시작 MHList_t 를 리턴. 이값을 항상 유지하는것이 중요.
 */
RxList_t* RxList_Append			(RxList_t *list , void* data);



/**
 * MHList_AppendSortEx : list 중 cmp에서 1이상의 값을 return 하는 위치에 추가
 * list: 시작 list 의 pointer
 * data: list 에 추가되는 아이템
 * cbcmp: 비교되는 함수 포인터
 * Returns: 시작 MHList_t 를 리턴. 이값을 항상 유지하는것이 중요.
 */
RxList_t* RxList_AppendSortEx(RxList_t *list , void* data, HINT32 (*cbcmp)(void *,void *));



/**
 * MHList_GetListItem: position 의 위치에 해당되는 아이템을 얻어온다.
 * list: 시작 list 의 pointer
 * position : 얻어올 list 내의 위치
 * Returns: list 내의 position 위치의 MHList_t 를 리턴
 */
RxList_t* RxList_GetListItem	(RxList_t* list , HUINT32 position);





/**
 * MHList_Remove: list 에서 data 지우기
 * list: 시작 list 의 pointer
 * Returns: 시작 MHList_t 를 리턴.
 */
RxList_t* RxList_Remove			(RxList_t *list , const void* data);





/**
 * MHList_First: list 모든 아이템 지우기
 * list: 시작 list 의 pointer
 * Returns: 항상 NULL 리턴.
 */
RxList_t* RxList_RemoveAll		(RxList_t *list);





/**
 * MHList_First: list 의 시작 MHList_t 를 얻기.
 * list: 시작 list 의 pointer
 * Returns: first MHList_t 리턴.
 */
RxList_t* RxList_First			(RxList_t *list);




/**
 * MHList_First: list 의 last MHList_t 를 얻기.
 * list: 시작 list 의 pointer
 * Returns: last MHList_t 리턴.
 */
RxList_t* RxList_Last			(RxList_t *list);


/**
 * MHList_First: list 의 data 를 얻기.
 * list: 시작 list 의 pointer
 * Returns: data void* 리턴.
 */
void* RxList_Data(RxList_t *list);


/**
 * MHList_First: list 에서 data 와 일치하는 MHList_t 찾기
 * list: 시작 list 의 pointer
 * Returns: 찾아낸 MHList_t 리턴.
 */
RxList_t* RxList_Find			(RxList_t *list , const void* data);


/**
 * MHList_First:
 * list: 시작 list 의 pointer / data
 * Returns:  MHList_t 리턴.
 */

RxList_t* RxList_Prepend		(RxList_t* list , void* data);



/**
 * MHList_First: list 의 length count 얻기.
 * list: 시작 list 의 pointer
 * Returns: list 의 length 리턴.
 */
HUINT32 RxList_Length			(RxList_t *list);


/**
 * MHList_FindEx: list에 call back을 이용한, list 찾기
 * list: 시작 list 의 pointer
 * Returns: 찾아낸 MHList_t 리턴.
 */
RxList_t*	RxList_FindEx		(RxList_t *list, const void* data, HBOOL (*cbcmp)(void *,void *));


/**
 * MHList_Foreach: list에 call back을 이용하여 foreach함.
 * list: 시작 list 의 pointer
 * func: foreach하면서 수행할 function pointer
 * Returns: void
 */
void RxList_Foreach(RxList_t *list , void (*func)(void *));

#endif
