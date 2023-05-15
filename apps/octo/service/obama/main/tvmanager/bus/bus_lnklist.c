/****************************************************************************
*
* File Name	: gwm_lnklist.c
*
* Description	: Linked List 관리 모듈.
*
* programed by Ahnsik, Choi.
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
* 2004/11/05	File Created.				AhnSik, Choi.
* 2008/01/10		revised					JK Baek
*
****************************************************************************/


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
#include <bus.h>
#include "_bus.h"

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */
/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */
/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */
/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */
/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */
/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */

INLINE	LINK_OBJECT	bus_lnklist_GetNext(LINK_OBJECT item)
{	HxLOG_Assert(item);
	return	item->nextLink;
}

/* 비교함수에서 큰 값이 앞에 존재한다. 내림차순으로 비교 */
BUS_Result_t	bus_lnklist_AddItemWithCompareDescend(LINK_OBJECT *root, LINK_OBJECT new_item, int compare_func(LINK_OBJECT, LINK_OBJECT) )
{
	LINK_OBJECT		temp;

	HxLOG_Assert(root);
	HxLOG_Assert(new_item);
	HxLOG_Assert(compare_func);

	if (new_item->nextLink != NULL)
	{
		HxLOG_Error("새로 추가하는 노드가 다른 링크에 연결되어 있을 수 있습니다.\n");
		return	ERR_BUS_LINK_CONFLICT;
	}

	temp = *root;
	if (temp==NULL)		// 아직 하나도 없다.
	{	*root = new_item;
		return	ERR_BUS_NO_ERROR;
	}

	if (compare_func(new_item, *root) <= 0)
	{	new_item->nextLink = *root;
		*root = new_item;
		return	ERR_BUS_NO_ERROR;
	}

	while(temp->nextLink != NULL)
	{
		if (compare_func(new_item, temp->nextLink) <= 0)
			break;
		temp = temp->nextLink;
	}
	new_item->nextLink = temp->nextLink;
	temp->nextLink = new_item;

	return	ERR_BUS_NO_ERROR;
}



BUS_Result_t	bus_lnklist_RemoveItemWithCompare(LINK_OBJECT *root, LINK_OBJECT item, int compare_func(LINK_OBJECT, LINK_OBJECT) )
{			// 비교함수에서 0을 리턴하는 조건의 Object를 삭제.
	LINK_OBJECT		temp, prev;

	HxLOG_Assert(root);
	HxLOG_Assert(item);
	HxLOG_Assert(compare_func);

	if (*root == NULL)		// 리스트에 하나도 없다.
	{	return	ERR_BUS_NO_OBJECT;
	}

	temp = *root;
	if (compare_func(item, *root)==0)	// 맨 첫번째 노드인 경우.
	{	*root = (*root)->nextLink;
		temp->nextLink = NULL;
		return	ERR_BUS_NO_ERROR;
	}

	do
	{	prev = temp;
		temp = temp->nextLink;
		if (compare_func(item, temp)==0)
			break;
	} while(temp != NULL);

	if (temp != NULL)			// 원하는 아이템을 찾았다면,
	{	prev->nextLink = temp->nextLink;
		temp->nextLink = NULL;
		return	ERR_BUS_NO_ERROR;
	}
								// 원하는 아이템을 찾지 못했다면,
	return	ERR_BUS_NO_OBJECT;
}

/* end of file */
