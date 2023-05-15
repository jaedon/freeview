/*************************************************************
 * @file		hxtree.h
 * @date		2011/01/19
 * @author	Moon-Young Kim (mykim@humaxdigital.com)
 * @brief		HUMAX common library header, related with tree functon.

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef HXTREE_H_
#define HXTREE_H_

#include "htype.h"

typedef struct _HxTree HxTREE_t;

typedef struct _HxTreeNode HxTREENode;

typedef HINT32		(*HxCompareFunc)			(const void* a , const void* b);
typedef HINT32		(*HxCompareDataFunc)		(const void* a , const void* b , void *userdata);
typedef void		(*HxDestroyNotify)			(void* data);
typedef void		(*HxDestroyDataNotify)		(void* data, void *userdata);
typedef HBOOL		(*HxTraverseFunc)			(void* key , void* value , void* data);
typedef void *		(*HxCloneFunc)				(const void *a);


/* �������� ���� Ʈ�� */


/**
 * HLIB_TREE_New: ���ο� HxTree �� ����.
 * key_compare_func: Ʈ���� �����ϱ� ���� �� �Լ�������.
 * 					 ����ڰ� ������ ���Լ��� ���� �θ𺸴� ������ left,
 *					 �θ𺸴� ũ�� right , ������ ���� ��忡 ��ü�ȴ�.
 * Return value: ���ο� HxTREE_t �� ������ ����.
 **/
HxTREE_t* 	_HLIB_TREE_New				(HxCompareFunc		key_compare_func, const HCHAR * file, HUINT32 line);


HxTREE_t*	_HLIB_TREE_NewWithData		(HxCompareDataFunc	key_compare_func,
									 		void*			key_compare_data, const HCHAR * file, HUINT32 line);

HxTREE_t* 	_HLIB_TREE_NewFull			(HxCompareDataFunc	key_compare_func,
											void*			key_compare_data,
											HxDestroyNotify		key_destroy_func,
											HxDestroyNotify		value_destroy_func, const HCHAR * file, HUINT32 line);


#define		HLIB_TREE_New(key_compare_func)		_HLIB_TREE_New(key_compare_func, __FILE__, __LINE__)

#define		HLIB_TREE_NewWithData(key_compare_func,key_compare_data)\
				_HLIB_TREE_NewWithData	(key_compare_func, key_compare_data, __FILE__, __LINE__)

#define		HLIB_TREE_NewFull(key_compare_func,key_compare_data,key_destroy_func,value_destroy_func)\
				_HLIB_TREE_NewFull(key_compare_func, key_compare_data,\
									 key_destroy_func, value_destroy_func,\
									 __FILE__, __LINE__)



/**
 * HLIB_TREE_Destroy: HxTree �� �Ҹ��Ŵ
 * tree: �Ҹ��ų HxTree
 **/
void		HLIB_TREE_Destroy				(HxTREE_t				*tree);

void		HLIB_TREE_NodeCache 		(HxTREE_t *tree, HBOOL enable);

HxTREE_t*	HLIB_TREE_Clone 			(HxTREE_t *tree,
									HxCloneFunc keyDup,
									HxCloneFunc valueDup);

void		HLIB_TREE_Extend 			(HxTREE_t *tree,
									HxTREE_t *other,
									HxCloneFunc keyDup,
									HxCloneFunc valueDup);

void		HLIB_TREE_RemoveAll 			(HxTREE_t *tree);




/**
 * HLIB_TREE_Insert: HxTree �� key/value �ѳ�带 �߰�. �ڵ����� �������� Ʈ�������� ������.
 * tree: HxTree
 * key: key
 * value: value
 **/
void 	HLIB_TREE_Insert				(HxTREE_t				*tree,
									 void*			key,
									 void*			value);

void	HLIB_TREE_Replace				(HxTREE_t				*tree,
									 void*			key,
									 void*			value);

HBOOL	HLIB_TREE_Remove				(HxTREE_t				*tree , void* 		key);

HBOOL	HLIB_TREE_Steal				(HxTREE_t				*tree , const void* 	key);

void* 	HLIB_TREE_Lookup				(HxTREE_t				*tree , const void*	key);

HBOOL	HLIB_TREE_LookupExtended		(HxTREE_t				*tree,
									 const void*		lookup_key,
									 void*			*orig_key,
									 void*			*value);

void	HLIB_TREE_Foreach				(HxTREE_t				*tree,
									 HxTraverseFunc		func,
									 void*			user_data);

void* HLIB_TREE_Search					(HxTREE_t				*tree,
									 HxCompareFunc		search_func,
									 const void*		user_data);

HINT32	HLIB_TREE_Height				(HxTREE_t				*tree);

HINT32	HLIB_TREE_Nnodes				(HxTREE_t				*tree);


void *		HLIB_TREE_Iterator (HxTREE_t *tree);
void *		HLIB_TREE_IteratorKey (void *iter);
void *		HLIB_TREE_IteratorValue (void *iter);
void *		HLIB_TREE_NextIterator (void *iter);

#define	HxTREE_EACH(tree,iter,expr)	\
	do {\
		for(iter = HLIB_TREE_Iterator(tree) ; iter ; iter = HLIB_TREE_NextIterator(iter)){\
			expr;\
		}\
	}while(0)





#endif

