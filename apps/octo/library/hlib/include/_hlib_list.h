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
 * HxList_insert : list �� �������� �߰�
 * list : ó������ HxList_t �� pointer �� �׻� ���� �Ǿ�� �Ѵ�.
 * data: list �� �߰��Ǵ� ������
 * position : ������ ��ġ��.
 * Returns : ���� HxList_t �� ����. �̰��� �׻� �����ϴ°��� �߿�.
 */
HxList_t* HLIB_LIST_Insert			(HxList_t *list , void* data , HINT32 position);




/**
 * HLIB_LIST_Append : list �� �������� list �߰�
 * list: ���� list �� pointer
 * data: list �� �߰��Ǵ� ������
 * Returns: ���� HxList_t �� ����. �̰��� �׻� �����ϴ°��� �߿�.
 */
HxList_t* HLIB_LIST_Append			(HxList_t *list , void* data);



/**
 * HLIB_LIST_AppendSortEx : list �� cmp���� 1�̻��� ���� return �ϴ� ��ġ�� �߰�
 * list: ���� list �� pointer
 * data: list �� �߰��Ǵ� ������
 * cbcmp: �񱳵Ǵ� �Լ� ������
 * Returns: ���� HxList_t �� ����. �̰��� �׻� �����ϴ°��� �߿�.
 */
HxList_t* HLIB_LIST_AppendSortEx(HxList_t *list , void* data, HINT32 (*cbcmp)(void *,void *));



/**
 * HLIB_LIST_GetListItem: position �� ��ġ�� �ش�Ǵ� �������� ���´�.
 * list: ���� list �� pointer
 * position : ���� list ���� ��ġ
 * Returns: list ���� position ��ġ�� HxList_t �� ����
 */
HxList_t* HLIB_LIST_GetListItem	(HxList_t* list , HUINT32 position);





/**
 * HLIB_LIST_Remove: list ���� data �����
 * list: ���� list �� pointer
 * Returns: ���� HxList_t �� ����.
 */
HxList_t* HLIB_LIST_Remove			(HxList_t *list , const void* data);





/**
 * HLIB_LIST_First: list ��� ������ �����
 * list: ���� list �� pointer
 * Returns: �׻� NULL ����.
 */
HxList_t* HLIB_LIST_RemoveAll		(HxList_t *list);




/**
 * HLIB_LIST_First: list ��� ������ ����� (����� ���� function�� data�� ���쵵�� callback ��� ����)
 * list: ���� list �� pointer
 * free_func: data�� ����� ���� func callback
 * Returns: �׻� NULL ����.
 */
HxList_t* HLIB_LIST_RemoveAllFunc(HxList_t *list,  void (*free_func)(void *data));


/**
 * �־��� List�� ó�� ���� condition_func�� ��� ���� ���� �ϳ��� ���� ����.
 * Result List�� ���� ���ο� list�� ������ ������, coding���� ����.
 */
HxList_t*	HLIB_LIST_RemoveIf(HxList_t *list, void *userData, HBOOL (*condition_func)(void *userdata, void *data), void (*free_func)(void *data));

/**
 * HLIB_LIST_First: list �� ���� HxList_t �� ���.
 * list: ���� list �� pointer
 * Returns: first HxList_t ����.
 */
HxList_t* HLIB_LIST_First			(HxList_t *list);



/**
 * list �� prev HxList_t �� ���.
 * list: list �� pointer
 * Returns: prev HxList_t ����.
 */
HxList_t* HLIB_LIST_Prev			(HxList_t *list);



/**
 * list �� next HxList_t �� ���.
 * list: list �� pointer
 * Returns: next HxList_t ����.
 */
HxList_t* HLIB_LIST_Next			(HxList_t *list);



/**
 * HLIB_LIST_First: list �� last HxList_t �� ���.
 * list: ���� list �� pointer
 * Returns: last HxList_t ����.
 */
HxList_t* HLIB_LIST_Last			(HxList_t *list);


/**
 * HLIB_LIST_First: list �� data �� ���.
 * list: ���� list �� pointer
 * Returns: data void* ����.
 */
void* HLIB_LIST_Data(HxList_t *list);


/**
 * HLIB_LIST_First: list ���� data �� ��ġ�ϴ� HxList_t ã��
 * list: ���� list �� pointer
 * Returns: ã�Ƴ� HxList_t ����.
 */
HxList_t* HLIB_LIST_Find			(HxList_t *list , const void* data);


/**
 * HLIB_LIST_First:
 * list: ���� list �� pointer / data
 * Returns:  HxList_t ����.
 */

HxList_t* HLIB_LIST_Prepend		(HxList_t* list , void* data);



/**
 * HLIB_LIST_First: list �� length count ���.
 * list: ���� list �� pointer
 * Returns: list �� length ����.
 */
HUINT32 HLIB_LIST_Length			(HxList_t *list);


/**
 * HLIB_LIST_FindEx: list�� call back�� �̿���, list ã��
 * list: ���� list �� pointer
 * Returns: ã�Ƴ� HxList_t ����.
 */
HxList_t* HLIB_LIST_FindEx 	(HxList_t *list, const void* user_data, HBOOL (*cbcmp)(void *user_data,void *list_data));


/**
 * HLIB_LIST_Foreach: list�� call back�� �̿��Ͽ� foreach��.
 * list: ���� list �� pointer
 * func: foreach�ϸ鼭 ������ function pointer
 * Returns: void
 */
void HLIB_LIST_Foreach(HxList_t *list , void (*func)(void *));

/**
 * HLIB_LIST_Foreach: list�� call back�� �̿��Ͽ� foreach��.
 * list: ���� list �� pointer
 * func: foreach�ϸ鼭 ������ function pointer
 * userdata : ���ڷ� �� userdata
 * Returns: return TRUE�� ������ �� list
 */
HxList_t* HLIB_LIST_ForeachEx(HxList_t *list , HBOOL (*func)(void *data, void *userdata), void *userdata);

#endif	/* !__HLIB_LIST_H__ */
