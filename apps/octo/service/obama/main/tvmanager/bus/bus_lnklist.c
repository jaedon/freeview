/****************************************************************************
*
* File Name	: gwm_lnklist.c
*
* Description	: Linked List ���� ���.
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

/* ���Լ����� ū ���� �տ� �����Ѵ�. ������������ �� */
BUS_Result_t	bus_lnklist_AddItemWithCompareDescend(LINK_OBJECT *root, LINK_OBJECT new_item, int compare_func(LINK_OBJECT, LINK_OBJECT) )
{
	LINK_OBJECT		temp;

	HxLOG_Assert(root);
	HxLOG_Assert(new_item);
	HxLOG_Assert(compare_func);

	if (new_item->nextLink != NULL)
	{
		HxLOG_Error("���� �߰��ϴ� ��尡 �ٸ� ��ũ�� ����Ǿ� ���� �� �ֽ��ϴ�.\n");
		return	ERR_BUS_LINK_CONFLICT;
	}

	temp = *root;
	if (temp==NULL)		// ���� �ϳ��� ����.
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
{			// ���Լ����� 0�� �����ϴ� ������ Object�� ����.
	LINK_OBJECT		temp, prev;

	HxLOG_Assert(root);
	HxLOG_Assert(item);
	HxLOG_Assert(compare_func);

	if (*root == NULL)		// ����Ʈ�� �ϳ��� ����.
	{	return	ERR_BUS_NO_OBJECT;
	}

	temp = *root;
	if (compare_func(item, *root)==0)	// �� ù��° ����� ���.
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

	if (temp != NULL)			// ���ϴ� �������� ã�Ҵٸ�,
	{	prev->nextLink = temp->nextLink;
		temp->nextLink = NULL;
		return	ERR_BUS_NO_ERROR;
	}
								// ���ϴ� �������� ã�� ���ߴٸ�,
	return	ERR_BUS_NO_OBJECT;
}

/* end of file */
