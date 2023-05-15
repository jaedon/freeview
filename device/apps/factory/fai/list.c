/****************************************************************************                                               
 * ProjectName :    Electronic Program Guide                                                                                
 *                                                                                                                          
 * ModuleDescription : 링크드 리스트                                                                                        
 *                                                                                                                          
 * FileName : list.c                                                                                                        
 *                                                                                                                          
 *     Date           Programmer               Work                                                                         
 * 2007. 12. 09       신 민 수          링크드 리스트 구현, 수정, 보완                                                      
 ****************************************************************************/

#include "list.h"

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          void LIST_Init(LIST_T *ptList)
 *
 * Description  : 링크드 리스트를 초기화한다.
 * Parameter    : ptList - 초기화할 링크드 리스트의 포인터
 * Return value : 없음
 ****************************************************************************/
void LIST_Init(LIST_T *ptList)
{
  ptList->tHead.ptPrev = NULL;
  ptList->tHead.ptNext = &ptList->tTail;
  ptList->tTail.ptPrev = &ptList->tHead;
  ptList->tTail.ptNext = NULL;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_Begin(LIST_T *ptList)
 *
 * Description  : 리스트의 처음 노드를 얻어온다.
 * Parameter    : ptList - 얻어올 리스트의 포인터
 * Return value : 리스트의 처음 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_Begin(LIST_T *ptList)
{
  return ptList->tHead.ptNext;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_Next(LIST_NODE_T *ptNode)
 *
 * Description  : 인자로 들어온 노드의 다음 노드를 얻어온다.
 * Parameter    : ptNode - 얻어올 노드의 이전 노드
 * Return value : 인자로 들어온 노드의 다음 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_Next(LIST_NODE_T *ptNode)
{
  return ptNode->ptNext;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_Prev(LIST_NODE_T *ptNode)
 *
 * Description  : 인자로 들어온 노드의 다음 노드를 얻어온다.
 * Parameter    : ptNode - 얻어올 노드의 이전 노드
 * Return value : 인자로 들어온 노드의 다음 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_Prev(LIST_NODE_T *ptNode)
{
  return ptNode->ptPrev;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_End(LIST_T *ptList)
 *
 * Description  : 리스트의 마지막 노드를 얻어온다. 
 * Parameter    : ptList - 얻어올 리스트의 포인터
 * Return value : 리스트의 마지막 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_End(LIST_T *ptList)
{
  return &ptList->tTail;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_Head(LIST_T *ptList)
 *
 * Description  : 리시트의 헤드 노드를 얻어온다. 
 * Parameter    : ptList - 얻어올 리스트의 포인터
 * Return value : 리스트의 헤드 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_Head(LIST_T *ptList)
{
  return &ptList->tHead;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_Tail(LIST_T *ptList)
 *
 * Description  : 리스트이 테일 노드를 얻어온다. 
 * Parameter    : ptList - 얻어올 리스트의 포인터
 * Return value : 리스트의 테일 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_Tail(LIST_T *ptList)
{
  return &ptList->tTail;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_Front(LIST_T *ptList)
 *
 * Description  : 리스트의 처음 노드를 얻어온다. 
 * Parameter    : ptList - 얻어올 리스트의 포인터
 * Return value : 리스트의 처음 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_Front(LIST_T *ptList)
{
  return ptList->tHead.ptNext;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_Back(LIST_T *ptList)
 *
 * Description  : 리스트의 마지막 노드를 얻어온다.
 * Parameter    : ptList - 얻어올 리스트의 포인터
 * Return value : 리스트의 마지막 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_Back(LIST_T *ptList)
{
  return ptList->tTail.ptPrev;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          bool LIST_IsEmpty(LIST_T *ptList)
 *
 * Description  : 비어있는 리스트인지 확인한다.
 * Parameter    : ptList - 확인할 리스트의 포인터
 * Return value : true - 비어있는 리스트
 *                false - 비어있지 않은 리스트
 ****************************************************************************/
bool LIST_IsEmpty(LIST_T *ptList)
{
  return LIST_Begin(ptList) == LIST_End(ptList);
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          void LIST_InsertAfter(LIST_NODE_T *before, LIST_NODE_T *ptNode)
 *
 * Description  : 인자로 주어진 before노드의 다음에 ptNode를 삽입한다.
 * Parameter    : before - 삽입할 노드의 이전 노드의 포인터
                  ptNode - 삽입할 노드
 * Return value : 없음
 ****************************************************************************/
void LIST_InsertAfter(LIST_NODE_T *ptBefore, LIST_NODE_T *ptNode)
{
  ptNode->ptPrev = ptBefore;
  ptNode->ptNext = ptBefore->ptNext;
  ptBefore->ptNext->ptPrev = ptNode;
  ptBefore->ptNext = ptNode;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          void LIST_InsertBefore(LIST_NODE_T *after, LIST_NODE_T *ptNode)
 *
 * Description  : 인자로 주어진 after노드의 이전에 ptNode를 삽입한다.
 * Parameter    : after - 삽입할 노드의 이후 노드의 포인터
 * Return value : 없음
 ****************************************************************************/
void LIST_InsertBefore(LIST_NODE_T *ptAfter, LIST_NODE_T *ptNode)
{
  ptNode->ptPrev = ptAfter->ptPrev;
  ptNode->ptNext = ptAfter;
  ptAfter->ptPrev->ptNext = ptNode;
  ptAfter->ptPrev = ptNode;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          void LIST_InsertFront(LIST_T *ptList, LIST_NODE_T *ptNode)
 *
 * Description  : 리스트의 맨 처음에 노드를 삽입한다.
 * Parameter    : ptList - 삽입할 리스트의 포인터
 *                ptNode - 삽입할 노드의 포인터
 * Return value : 없음
 ****************************************************************************/
void LIST_InsertFront(LIST_T *ptList, LIST_NODE_T *ptNode)
{
  LIST_InsertBefore(LIST_Begin(ptList), ptNode);
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          void LIST_InsertBack(LIST_T *ptList, LIST_NODE_T *ptNode)
 *
 * Description  : 리스트의 마지막에 노드를 삽입한다. 
 * Parameter    : ptList - 삽입할 리스트의 포인터
                  ptNode - 삽입할 노드의 포인터
 * Return value : 없음
 ****************************************************************************/
void LIST_InsertBack(LIST_T *ptList, LIST_NODE_T *ptNode)
{
  LIST_InsertBefore(LIST_End(ptList), ptNode);
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_Delete(LIST_NODE_T *ptNode)
 *
 * Description  : 주어진 노도를 삭제한다. 
 * Parameter    : ptNode - 삭제할 노드의 포인터
 * Return value : 삭제한 노드의 다음 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_Delete(LIST_NODE_T *ptNode)
{
  ptNode->ptPrev->ptNext = ptNode->ptNext;
  ptNode->ptNext->ptPrev = ptNode->ptPrev;
  return ptNode->ptNext;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_DeleteFront(LIST_T *ptList)
 *
 * Description  : 주어진 리스트의 처음 노드를 삭제한다.
 * Parameter    : ptList - 삭제할 리스트의 포인터
 * Return value : 삭제한 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_DeleteFront(LIST_T *ptList)
{
  LIST_NODE_T *ptFront = LIST_Front(ptList);
  LIST_Delete(ptFront);
  return ptFront;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          LIST_NODE_T *LIST_DeleteBack(LIST_T *ptList)
 *
 * Description  : 주어진 리스트의 마지막 노드를 삭제한다. 
 * Parameter    : ptList - 삭제할 리스트의 포인터
 * Return value : 삭제한 노드의 포인터
 ****************************************************************************/
LIST_NODE_T *LIST_DeleteBack(LIST_T *ptList)
{
  LIST_NODE_T *ptBack = LIST_Back(ptList);
  LIST_Delete(ptBack);
  return ptBack;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          bool LIST_IsHead(LIST_NODE_T *ptNode)
 *
 * Description  : 주어진 노드가 헤드 노드인지 검사한다.
 * Parameter    : 
 * Return value : 
 ****************************************************************************/
bool LIST_IsHead(LIST_NODE_T *ptNode)
{
  return ptNode != NULL && ptNode->ptPrev == NULL && ptNode->ptNext != NULL;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          bool LIST_IsInterior(LIST_NODE_T *ptNode)
 *
 * Description  : 주어진 노드가 내부 노드인지 확인한다.
 * Parameter    : 
 * Return value : 
 ****************************************************************************/
bool LIST_IsInterior(LIST_NODE_T *ptNode)
{
  return ptNode != NULL && ptNode->ptPrev != NULL && ptNode->ptNext != NULL;
}

/****************************************************************************
 * Programmer   : 신 민 수
 *
 *          bool LIST_IsTail(LIST_NODE_T *ptNode)
 *
 * Description  : 주어진 노드가 테일 노드인지 확인한다.
 * Parameter    : 
 * Return value : 
 ****************************************************************************/
bool LIST_IsTail(LIST_NODE_T *ptNode)
{
  return ptNode != NULL && ptNode->ptPrev != NULL && ptNode->ptNext == NULL;
}

