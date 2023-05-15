/*************************************************************
 * @file		hxhash.h
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


#include "htype.h"
#include "_hlib_list.h"



typedef struct _HxHashTable  HxHASH_t;

typedef HBOOL	(*_HxHRFunc)		(void*  key , void*  value , void*  user_data);

typedef HBOOL	(*_HxEqualFunc)	(const void* a , const void* b);

typedef void	(*_HxDestroyNotify)(void* data);

typedef void	(*_HxFunc)		(void* data , void* user_data);

typedef	HUINT32	(*_HxHashFunc)	(const void* key);

typedef void	(*_HxHFunc)		(void* key , void* value , void* user_data);


typedef struct _HxHashTableIter HxHASH_TableIter_t;

struct _HxHashTableIter
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

HxHASH_t* _HLIB_HASH_NewFull 		(_HxHashFunc		hash_func,
					   					_HxEqualFunc	    key_equal_func,
									    _HxDestroyNotify  key_destroy_func,
										_HxDestroyNotify  value_destroy_func,
										const char* file, const int line);


HxHASH_t* _HLIB_HASH_New			(_HxHashFunc	    hash_func,
									    _HxEqualFunc	    key_equal_func
									    ,const char* file, const int line
									    );

HxHASH_t* _HLIB_HASH_SelfNew 		(const char* file, const int line);



#define HLIB_HASH_NewFull(a,b,c,d)					_HLIB_HASH_NewFull(a,b,c,d, __FILE__, __LINE__)
#define HLIB_HASH_New(hash_func, key_equal_func)	_HLIB_HASH_New(hash_func, key_equal_func, __FILE__, __LINE__)
#define HLIB_HASH_SelfNew() 						_HLIB_HASH_SelfNew(__FILE__, __LINE__)


void	HLIB_HASH_Destroy	  			(HxHASH_t	   *hash_table);

void	HLIB_HASH_Insert	   			(HxHASH_t	   *hash_table,
										void*	    key,
										void*	    value);

void	HLIB_HASH_Replace         	(HxHASH_t     *hash_table,
										void*	    key,
										void*	    value);

HBOOL    HLIB_HASH_Remove		   		(HxHASH_t	   *hash_table,
										const void*   key);

void     HLIB_HASH_RemoveAll        	(HxHASH_t     *hash_table);

HBOOL    HLIB_HASH_Steal             	(HxHASH_t     *hash_table,
										const void*   key);

void     HLIB_HASH_StealAll			(HxHASH_t * hash_table);

void*    HLIB_HASH_Lookup		   		(HxHASH_t	   *hash_table,
										const void*   key);

HBOOL    HLIB_HASH_LookupExtended	(HxHASH_t	   *hash_table,
										const void*   lookup_key,
										void*	   *orig_key,
										void*	   *value);
void* 	HLIB_HASH_Detach (HxHASH_t *hash_table, const void* key);
void	 HLIB_HASH_Foreach			(HxHASH_t	   *hash_table,
										_HxHFunc	    func,
										void*	    user_data);

void*    HLIB_HASH_Find	           	(HxHASH_t	   *hash_table,
										_HxHRFunc	    predicate,
										void*	    user_data);

HUINT32	HLIB_HASH_ForeachRemove	   	(HxHASH_t	   *hash_table,
										_HxHRFunc	    func,
										void*	    user_data);

HUINT32 HLIB_HASH_ForeachSteal		(HxHASH_t	   *hash_table,
										_HxHRFunc	    func,
										void*	    user_data);

HUINT32	    HLIB_HASH_Size		   	(HxHASH_t	   *hash_table);

HxList_t*     HLIB_HASH_GetKeys        (HxHASH_t     *hash_table);

HxList_t*     HLIB_HASH_GetValues      (HxHASH_t     *hash_table);

void	HLIB_HASH_IterInit       	(HxHASH_TableIter_t *iter,
										HxHASH_t     *hash_table);

HBOOL	HLIB_HASH_IterNext         	(HxHASH_TableIter_t *iter,
										void*       *key,
										void*       *value);

HxHASH_t* HLIB_HASH_IterGetHashTable (HxHASH_TableIter_t *iter);

void	HLIB_HASH_IterRemove       	(HxHASH_TableIter_t *iter);

void    HLIB_HASH_IterSteal        	(HxHASH_TableIter_t *iter);

HxHASH_t* HLIB_HASH_Ref			(HxHASH_t 	   *hash_table);

void	HLIB_HASH_Unref				(HxHASH_t     *hash_table);

HBOOL 	HLIB_HASH_StrEqual			(const void* v1 , const void* v2);
HUINT32 HLIB_HASH_StrHash 			(const void* v);

HUINT32		HLIB_HASH_GetStringKey(const void *key);
HBOOL		HLIB_HASH_IsStringEqual(const void *a, const void *b);

HBOOL	HLIB_HASH_IsIntEqual	(const void* a , const void* b);
HUINT32	HLIB_HASH_GetIntKey	(const void* key);


#endif
