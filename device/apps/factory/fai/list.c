/****************************************************************************                                               
 * ProjectName :    Electronic Program Guide                                                                                
 *                                                                                                                          
 * ModuleDescription : ��ũ�� ����Ʈ                                                                                        
 *                                                                                                                          
 * FileName : list.c                                                                                                        
 *                                                                                                                          
 *     Date           Programmer               Work                                                                         
 * 2007. 12. 09       �� �� ��          ��ũ�� ����Ʈ ����, ����, ����                                                      
 ****************************************************************************/

#include "list.h"

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          void LIST_Init(LIST_T *ptList)
 *
 * Description  : ��ũ�� ����Ʈ�� �ʱ�ȭ�Ѵ�.
 * Parameter    : ptList - �ʱ�ȭ�� ��ũ�� ����Ʈ�� ������
 * Return value : ����
 ****************************************************************************/
void LIST_Init(LIST_T *ptList)
{
  ptList->tHead.ptPrev = NULL;
  ptList->tHead.ptNext = &ptList->tTail;
  ptList->tTail.ptPrev = &ptList->tHead;
  ptList->tTail.ptNext = NULL;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_Begin(LIST_T *ptList)
 *
 * Description  : ����Ʈ�� ó�� ��带 ���´�.
 * Parameter    : ptList - ���� ����Ʈ�� ������
 * Return value : ����Ʈ�� ó�� ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_Begin(LIST_T *ptList)
{
  return ptList->tHead.ptNext;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_Next(LIST_NODE_T *ptNode)
 *
 * Description  : ���ڷ� ���� ����� ���� ��带 ���´�.
 * Parameter    : ptNode - ���� ����� ���� ���
 * Return value : ���ڷ� ���� ����� ���� ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_Next(LIST_NODE_T *ptNode)
{
  return ptNode->ptNext;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_Prev(LIST_NODE_T *ptNode)
 *
 * Description  : ���ڷ� ���� ����� ���� ��带 ���´�.
 * Parameter    : ptNode - ���� ����� ���� ���
 * Return value : ���ڷ� ���� ����� ���� ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_Prev(LIST_NODE_T *ptNode)
{
  return ptNode->ptPrev;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_End(LIST_T *ptList)
 *
 * Description  : ����Ʈ�� ������ ��带 ���´�. 
 * Parameter    : ptList - ���� ����Ʈ�� ������
 * Return value : ����Ʈ�� ������ ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_End(LIST_T *ptList)
{
  return &ptList->tTail;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_Head(LIST_T *ptList)
 *
 * Description  : ����Ʈ�� ��� ��带 ���´�. 
 * Parameter    : ptList - ���� ����Ʈ�� ������
 * Return value : ����Ʈ�� ��� ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_Head(LIST_T *ptList)
{
  return &ptList->tHead;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_Tail(LIST_T *ptList)
 *
 * Description  : ����Ʈ�� ���� ��带 ���´�. 
 * Parameter    : ptList - ���� ����Ʈ�� ������
 * Return value : ����Ʈ�� ���� ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_Tail(LIST_T *ptList)
{
  return &ptList->tTail;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_Front(LIST_T *ptList)
 *
 * Description  : ����Ʈ�� ó�� ��带 ���´�. 
 * Parameter    : ptList - ���� ����Ʈ�� ������
 * Return value : ����Ʈ�� ó�� ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_Front(LIST_T *ptList)
{
  return ptList->tHead.ptNext;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_Back(LIST_T *ptList)
 *
 * Description  : ����Ʈ�� ������ ��带 ���´�.
 * Parameter    : ptList - ���� ����Ʈ�� ������
 * Return value : ����Ʈ�� ������ ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_Back(LIST_T *ptList)
{
  return ptList->tTail.ptPrev;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          bool LIST_IsEmpty(LIST_T *ptList)
 *
 * Description  : ����ִ� ����Ʈ���� Ȯ���Ѵ�.
 * Parameter    : ptList - Ȯ���� ����Ʈ�� ������
 * Return value : true - ����ִ� ����Ʈ
 *                false - ������� ���� ����Ʈ
 ****************************************************************************/
bool LIST_IsEmpty(LIST_T *ptList)
{
  return LIST_Begin(ptList) == LIST_End(ptList);
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          void LIST_InsertAfter(LIST_NODE_T *before, LIST_NODE_T *ptNode)
 *
 * Description  : ���ڷ� �־��� before����� ������ ptNode�� �����Ѵ�.
 * Parameter    : before - ������ ����� ���� ����� ������
                  ptNode - ������ ���
 * Return value : ����
 ****************************************************************************/
void LIST_InsertAfter(LIST_NODE_T *ptBefore, LIST_NODE_T *ptNode)
{
  ptNode->ptPrev = ptBefore;
  ptNode->ptNext = ptBefore->ptNext;
  ptBefore->ptNext->ptPrev = ptNode;
  ptBefore->ptNext = ptNode;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          void LIST_InsertBefore(LIST_NODE_T *after, LIST_NODE_T *ptNode)
 *
 * Description  : ���ڷ� �־��� after����� ������ ptNode�� �����Ѵ�.
 * Parameter    : after - ������ ����� ���� ����� ������
 * Return value : ����
 ****************************************************************************/
void LIST_InsertBefore(LIST_NODE_T *ptAfter, LIST_NODE_T *ptNode)
{
  ptNode->ptPrev = ptAfter->ptPrev;
  ptNode->ptNext = ptAfter;
  ptAfter->ptPrev->ptNext = ptNode;
  ptAfter->ptPrev = ptNode;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          void LIST_InsertFront(LIST_T *ptList, LIST_NODE_T *ptNode)
 *
 * Description  : ����Ʈ�� �� ó���� ��带 �����Ѵ�.
 * Parameter    : ptList - ������ ����Ʈ�� ������
 *                ptNode - ������ ����� ������
 * Return value : ����
 ****************************************************************************/
void LIST_InsertFront(LIST_T *ptList, LIST_NODE_T *ptNode)
{
  LIST_InsertBefore(LIST_Begin(ptList), ptNode);
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          void LIST_InsertBack(LIST_T *ptList, LIST_NODE_T *ptNode)
 *
 * Description  : ����Ʈ�� �������� ��带 �����Ѵ�. 
 * Parameter    : ptList - ������ ����Ʈ�� ������
                  ptNode - ������ ����� ������
 * Return value : ����
 ****************************************************************************/
void LIST_InsertBack(LIST_T *ptList, LIST_NODE_T *ptNode)
{
  LIST_InsertBefore(LIST_End(ptList), ptNode);
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_Delete(LIST_NODE_T *ptNode)
 *
 * Description  : �־��� �뵵�� �����Ѵ�. 
 * Parameter    : ptNode - ������ ����� ������
 * Return value : ������ ����� ���� ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_Delete(LIST_NODE_T *ptNode)
{
  ptNode->ptPrev->ptNext = ptNode->ptNext;
  ptNode->ptNext->ptPrev = ptNode->ptPrev;
  return ptNode->ptNext;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_DeleteFront(LIST_T *ptList)
 *
 * Description  : �־��� ����Ʈ�� ó�� ��带 �����Ѵ�.
 * Parameter    : ptList - ������ ����Ʈ�� ������
 * Return value : ������ ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_DeleteFront(LIST_T *ptList)
{
  LIST_NODE_T *ptFront = LIST_Front(ptList);
  LIST_Delete(ptFront);
  return ptFront;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          LIST_NODE_T *LIST_DeleteBack(LIST_T *ptList)
 *
 * Description  : �־��� ����Ʈ�� ������ ��带 �����Ѵ�. 
 * Parameter    : ptList - ������ ����Ʈ�� ������
 * Return value : ������ ����� ������
 ****************************************************************************/
LIST_NODE_T *LIST_DeleteBack(LIST_T *ptList)
{
  LIST_NODE_T *ptBack = LIST_Back(ptList);
  LIST_Delete(ptBack);
  return ptBack;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          bool LIST_IsHead(LIST_NODE_T *ptNode)
 *
 * Description  : �־��� ��尡 ��� ������� �˻��Ѵ�.
 * Parameter    : 
 * Return value : 
 ****************************************************************************/
bool LIST_IsHead(LIST_NODE_T *ptNode)
{
  return ptNode != NULL && ptNode->ptPrev == NULL && ptNode->ptNext != NULL;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          bool LIST_IsInterior(LIST_NODE_T *ptNode)
 *
 * Description  : �־��� ��尡 ���� ������� Ȯ���Ѵ�.
 * Parameter    : 
 * Return value : 
 ****************************************************************************/
bool LIST_IsInterior(LIST_NODE_T *ptNode)
{
  return ptNode != NULL && ptNode->ptPrev != NULL && ptNode->ptNext != NULL;
}

/****************************************************************************
 * Programmer   : �� �� ��
 *
 *          bool LIST_IsTail(LIST_NODE_T *ptNode)
 *
 * Description  : �־��� ��尡 ���� ������� Ȯ���Ѵ�.
 * Parameter    : 
 * Return value : 
 ****************************************************************************/
bool LIST_IsTail(LIST_NODE_T *ptNode)
{
  return ptNode != NULL && ptNode->ptPrev != NULL && ptNode->ptNext == NULL;
}

