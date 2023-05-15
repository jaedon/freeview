/****************************************************************************                                               
 * ProjectName :    Electronic Program Guide                                                                                
 *                                                                                                                          
 * ModuleDescription : ��ũ�� ����Ʈ                                                                                        
 *                                                                                                                          
 * FileName : list.h                                                                                                        
 *                                                                                                                          
 *     Date           Programmer               Work                                                                         
 * 2007. 12. 09      �� �� ��           ��ũ�� ����Ʈ ����, ����, ����                                                      
 ***************************************************************************/
#ifndef _LIST_H_
#define _LIST_H_

#include <stdbool.h>
#include <stddef.h>  /* offsetof ��ũ�� ��� */

/* ����Ʈ�� ��� ����ü */
typedef struct list_node {
  struct list_node *ptPrev;
  struct list_node *ptNext;
} LIST_NODE_T;

/* ����Ʈ ����ü */
typedef struct list {
  LIST_NODE_T tHead;   /* ����Ʈ�� ���� */
  LIST_NODE_T tTail;   /* ����Ʈ�� �� */
} LIST_T;

/* list_node�� �����ͷ� list_node�� �����ϰ� �ִ� 
   ����ü�� �����ͷκ�ȯ�Ѵ�. 
*/
#define LIST_Entry(LIST_NODE, STRUCT, MEMBER) \
  ((STRUCT *) ((unsigned char *) &(LIST_NODE)->ptNext    \
	       - offsetof (STRUCT, MEMBER.ptNext)))

/* ����Ʈ �ʱ�ȭ */
void LIST_Init(LIST_T *ptList);

/* ����Ʈ ��� ��� */
LIST_NODE_T *LIST_Begin(LIST_T *ptList);
LIST_NODE_T *LIST_Next(LIST_NODE_T *ptNode);
LIST_NODE_T *LIST_Prev(LIST_NODE_T *ptNode);
LIST_NODE_T *LIST_End(LIST_T *ptList);

LIST_NODE_T *LIST_Head(LIST_T *ptList);
LIST_NODE_T *LIST_Tail(LIST_T *ptList);

/* ����Ʈ ���� ���� */
void LIST_InsertBefore(LIST_NODE_T *ptAfter, LIST_NODE_T *ptNode);
void LIST_InsertAfter(LIST_NODE_T *ptBefore, LIST_NODE_T *ptNode);
void LIST_InsertFront(LIST_T *ptList, LIST_NODE_T *ptNode);
void LIST_InsertBack(LIST_T *ptList, LIST_NODE_T *ptNode);

/* ����Ʈ ���� ���� */
LIST_NODE_T *LIST_Delete(LIST_NODE_T *ptNode);
LIST_NODE_T *LIST_DeleteFront(LIST_T *ptList);
LIST_NODE_T *LIST_DeleteBack(LIST_T *ptList);

/* ����Ʈ �Ӽ� �˻� */
bool LIST_IsEmpty(LIST_T *ptList);
bool LIST_IsHead(LIST_NODE_T *ptNode);
bool LIST_IsInterior(LIST_NODE_T *ptNode);
bool LIST_IsTail(LIST_NODE_T *ptNode);

#endif
