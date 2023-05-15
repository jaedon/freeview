/****************************************************************************                                               
 * ProjectName :    Electronic Program Guide                                                                                
 *                                                                                                                          
 * ModuleDescription : 링크드 리스트                                                                                        
 *                                                                                                                          
 * FileName : list.h                                                                                                        
 *                                                                                                                          
 *     Date           Programmer               Work                                                                         
 * 2007. 12. 09      신 민 수           링크드 리스트 구현, 수정, 보완                                                      
 ***************************************************************************/
#ifndef _LIST_H_
#define _LIST_H_

#include <stdbool.h>
#include <stddef.h>  /* offsetof 매크로 사용 */

/* 리스트의 노드 구조체 */
typedef struct list_node {
  struct list_node *ptPrev;
  struct list_node *ptNext;
} LIST_NODE_T;

/* 리스트 구조체 */
typedef struct list {
  LIST_NODE_T tHead;   /* 리스트의 시작 */
  LIST_NODE_T tTail;   /* 리스트의 끝 */
} LIST_T;

/* list_node의 포인터로 list_node를 포함하고 있는 
   구조체의 포인터로변환한다. 
*/
#define LIST_Entry(LIST_NODE, STRUCT, MEMBER) \
  ((STRUCT *) ((unsigned char *) &(LIST_NODE)->ptNext    \
	       - offsetof (STRUCT, MEMBER.ptNext)))

/* 리스트 초기화 */
void LIST_Init(LIST_T *ptList);

/* 리스트 노드 얻기 */
LIST_NODE_T *LIST_Begin(LIST_T *ptList);
LIST_NODE_T *LIST_Next(LIST_NODE_T *ptNode);
LIST_NODE_T *LIST_Prev(LIST_NODE_T *ptNode);
LIST_NODE_T *LIST_End(LIST_T *ptList);

LIST_NODE_T *LIST_Head(LIST_T *ptList);
LIST_NODE_T *LIST_Tail(LIST_T *ptList);

/* 리스트 삽입 연산 */
void LIST_InsertBefore(LIST_NODE_T *ptAfter, LIST_NODE_T *ptNode);
void LIST_InsertAfter(LIST_NODE_T *ptBefore, LIST_NODE_T *ptNode);
void LIST_InsertFront(LIST_T *ptList, LIST_NODE_T *ptNode);
void LIST_InsertBack(LIST_T *ptList, LIST_NODE_T *ptNode);

/* 리스트 삭제 연산 */
LIST_NODE_T *LIST_Delete(LIST_NODE_T *ptNode);
LIST_NODE_T *LIST_DeleteFront(LIST_T *ptList);
LIST_NODE_T *LIST_DeleteBack(LIST_T *ptList);

/* 리스트 속성 검사 */
bool LIST_IsEmpty(LIST_T *ptList);
bool LIST_IsHead(LIST_NODE_T *ptNode);
bool LIST_IsInterior(LIST_NODE_T *ptNode);
bool LIST_IsTail(LIST_NODE_T *ptNode);

#endif
