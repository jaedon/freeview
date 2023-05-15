/*************************************************************
 * @file		rxhash.h
 * @date		2011/01/20
 * @author	Moon-Young Kim (mykim@humaxdigital.com)
 * @brief		HUMAX common library header, related with hash functon.

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef HXHASH_H_
#define HXHASH_H_

//#define MAX_HXTREE_HEIGHT 40

#undef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))

#undef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))


#ifndef NULL
#define NULL					0
#endif

#ifndef FALSE
#define FALSE					0
#endif

#ifndef TRUE
#define TRUE					1
#endif


#include "rxtype.h"
#include "rxlist.h"



typedef struct _RxHashTable  RxHashTable_t;

typedef HBOOL	(*_RxHRFunc)		(void*  key , void*  value , void*  user_data);

typedef HBOOL	(*_RxEqualFunc)	(const void* a , const void* b);

typedef void	(*_RxDestroyNotify)(void* data);

typedef void	(*_RxFunc)		(void* data , void* user_data);

typedef	HUINT32	(*_RxHashFunc)	(const void* key);

typedef void	(*_RxHFunc)		(void* key , void* value , void* user_data);


typedef struct _RxHashTableIter RxHashTableIter_t;

struct _RxHashTableIter
{
	void*	dummy1;
	void*	dummy2;
	void*	dummy3;
	HINT32	dummy4;
	HBOOL	dummy5;
	void*	dummy6;
};

/* Hash tables
 */
RxHashTable_t* RxHashTable_New			(_RxHashFunc	    hash_func,
									    _RxEqualFunc	    key_equal_func);

RxHashTable_t* RxHashTable_SelfNew 		(void);


RxHashTable_t* RxHashTable_NewFull     	(_RxHashFunc	    hash_func,
					   					_RxEqualFunc	    key_equal_func,
									    _RxDestroyNotify  key_destroy_func,
									    _RxDestroyNotify  value_destroy_func);

void	RxHashTable_Destroy	  			(RxHashTable_t	   *hash_table);

void	RxHashTable_Insert	   			(RxHashTable_t	   *hash_table,
										void*	    key,
										void*	    value);

void	RxHashTable_Replace         	(RxHashTable_t     *hash_table,
										void*	    key,
										void*	    value);

HBOOL    RxHashTable_Remove		   		(RxHashTable_t	   *hash_table,
										const void*   key);

void     RxHashTable_RemoveAll        	(RxHashTable_t     *hash_table);

HBOOL    RxHashTable_Steal             	(RxHashTable_t     *hash_table,
										const void*   key);

void     RxHashTable_StealAll			(RxHashTable_t * hash_table);

void*    RxHashTable_Lookup		   		(RxHashTable_t	   *hash_table,
										const void*   key);

HBOOL    RxHashTable_LookupExtended	(RxHashTable_t	   *hash_table,
										const void*   lookup_key,
										void*	   *orig_key,
										void*	   *value);

void	 RxHashTable_Foreach			(RxHashTable_t	   *hash_table,
										_RxHFunc	    func,
										void*	    user_data);

void*    RxHashTable_Find	           	(RxHashTable_t	   *hash_table,
										_RxHRFunc	    predicate,
										void*	    user_data);

HUINT32	RxHashTable_ForeachRemove	   	(RxHashTable_t	   *hash_table,
										_RxHRFunc	    func,
										void*	    user_data);

HUINT32 RxHashTable_ForeachSteal		(RxHashTable_t	   *hash_table,
										_RxHRFunc	    func,
										void*	    user_data);

HUINT32	    RxHashTable_Size		   	(RxHashTable_t	   *hash_table);

RxList_t*     RxHashTable_GetKeys        (RxHashTable_t     *hash_table);

RxList_t*     RxHashTable_GetValues      (RxHashTable_t     *hash_table);

void	RxHashTable_IterInit       	(RxHashTableIter_t *iter,
										RxHashTable_t     *hash_table);

HBOOL	RxHashTable_IterNext         	(RxHashTableIter_t *iter,
										void*       *key,
										void*       *value);

RxHashTable_t* RxHashTable_IterGetHashTable (RxHashTableIter_t *iter);

void	RxHashTable_IterRemove       	(RxHashTableIter_t *iter);

void    RxHashTable_IterSteal        	(RxHashTableIter_t *iter);

RxHashTable_t* RxHashTable_Ref			(RxHashTable_t 	   *hash_table);

void	RxHashTable_Unref				(RxHashTable_t     *hash_table);

HBOOL 	RxHashTable_StrEqual			(const void* v1 , const void* v2);
HUINT32 RxHashTable_StrHash 			(const void* v);

HUINT32		RxHash_Table_String_Key(const void *key);
HBOOL		RxHash_Table_String_Equal(const void *a, const void *b);


#endif
