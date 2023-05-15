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
 * MHList_insert : list �� �������� �߰�
 * list : ó������ MHList_t �� pointer �� �׻� ���� �Ǿ�� �Ѵ�.
 * data: list �� �߰��Ǵ� ������
 * position : ������ ��ġ��.
 * Returns : ���� MHList_t �� ����. �̰��� �׻� �����ϴ°��� �߿�.
 */
RxList_t* RxList_Insert			(RxList_t *list , void* data , HINT32 position);




/**
 * MHList_Append : list �� �������� list �߰�
 * list: ���� list �� pointer
 * data: list �� �߰��Ǵ� ������
 * Returns: ���� MHList_t �� ����. �̰��� �׻� �����ϴ°��� �߿�.
 */
RxList_t* RxList_Append			(RxList_t *list , void* data);



/**
 * MHList_AppendSortEx : list �� cmp���� 1�̻��� ���� return �ϴ� ��ġ�� �߰�
 * list: ���� list �� pointer
 * data: list �� �߰��Ǵ� ������
 * cbcmp: �񱳵Ǵ� �Լ� ������
 * Returns: ���� MHList_t �� ����. �̰��� �׻� �����ϴ°��� �߿�.
 */
RxList_t* RxList_AppendSortEx(RxList_t *list , void* data, HINT32 (*cbcmp)(void *,void *));



/**
 * MHList_GetListItem: position �� ��ġ�� �ش�Ǵ� �������� ���´�.
 * list: ���� list �� pointer
 * position : ���� list ���� ��ġ
 * Returns: list ���� position ��ġ�� MHList_t �� ����
 */
RxList_t* RxList_GetListItem	(RxList_t* list , HUINT32 position);





/**
 * MHList_Remove: list ���� data �����
 * list: ���� list �� pointer
 * Returns: ���� MHList_t �� ����.
 */
RxList_t* RxList_Remove			(RxList_t *list , const void* data);





/**
 * MHList_First: list ��� ������ �����
 * list: ���� list �� pointer
 * Returns: �׻� NULL ����.
 */
RxList_t* RxList_RemoveAll		(RxList_t *list);





/**
 * MHList_First: list �� ���� MHList_t �� ���.
 * list: ���� list �� pointer
 * Returns: first MHList_t ����.
 */
RxList_t* RxList_First			(RxList_t *list);




/**
 * MHList_First: list �� last MHList_t �� ���.
 * list: ���� list �� pointer
 * Returns: last MHList_t ����.
 */
RxList_t* RxList_Last			(RxList_t *list);


/**
 * MHList_First: list �� data �� ���.
 * list: ���� list �� pointer
 * Returns: data void* ����.
 */
void* RxList_Data(RxList_t *list);


/**
 * MHList_First: list ���� data �� ��ġ�ϴ� MHList_t ã��
 * list: ���� list �� pointer
 * Returns: ã�Ƴ� MHList_t ����.
 */
RxList_t* RxList_Find			(RxList_t *list , const void* data);


/**
 * MHList_First:
 * list: ���� list �� pointer / data
 * Returns:  MHList_t ����.
 */

RxList_t* RxList_Prepend		(RxList_t* list , void* data);



/**
 * MHList_First: list �� length count ���.
 * list: ���� list �� pointer
 * Returns: list �� length ����.
 */
HUINT32 RxList_Length			(RxList_t *list);


/**
 * MHList_FindEx: list�� call back�� �̿���, list ã��
 * list: ���� list �� pointer
 * Returns: ã�Ƴ� MHList_t ����.
 */
RxList_t*	RxList_FindEx		(RxList_t *list, const void* data, HBOOL (*cbcmp)(void *,void *));


/**
 * MHList_Foreach: list�� call back�� �̿��Ͽ� foreach��.
 * list: ���� list �� pointer
 * func: foreach�ϸ鼭 ������ function pointer
 * Returns: void
 */
void RxList_Foreach(RxList_t *list , void (*func)(void *));

#endif
