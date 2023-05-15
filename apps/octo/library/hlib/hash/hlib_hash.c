/*
* hxhash.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "_hlib_stdlib.h"
#include "_hlib_hash.h"
#include "_hlib_list.h"

#define HXHASH_TABLE_MIN_SHIFT 3  /* 1 << 3 == 8 buckets */

typedef struct _HxHashNode      HxHashNode_t;

struct _HxHashNode
{
	void*   key;
	void*   value;

	/* If key_hash == 0, node is not in use
	* If key_hash == 1, node is a tombstone
	* If key_hash >= 2, node contains data */
	HUINT32      key_hash;
};

struct _HxHashTable
{
	HINT32             size;
	HINT32             mod;
	HUINT32            mask;
	HINT32             nnodes;
	HINT32             noccupied;  /* nnodes + tombstones */
	HxHashNode_t       *nodes;
	_HxHashFunc        hash_func;
	_HxEqualFunc       key_equal_func;
	volatile HINT32    ref_count;
	_HxDestroyNotify   key_destroy_func;
	_HxDestroyNotify   value_destroy_func;
#if defined(CONFIG_DEBUG)
	HCHAR				* debugText;
	HUINT32 			debugLine;
#endif
};

typedef struct
{
	HxHASH_t  *hash_table;
	void*     dummy1;
	void*     dummy2;
	int          position;
	HBOOL     dummy3;
	int          version;
} RealIter;



STATIC const HINT32 prime_mod [] =
{
	1,          /* For 1 << 0 */
	2,
	3,
	7,
	13,
	31,
	61,
	127,
	251,
	509,
	1021,
	2039,
	4093,
	8191,
	16381,
	32749,
	65521,      /* For 1 << 16 */
	131071,
	262139,
	524287,
	1048573,
	2097143,
	4194301,
	8388593,
	16777213,
	33554393,
	67108859,
	134217689,
	268435399,
	536870909,
	1073741789,
	2147483647  /* For 1 << 31 */
};

STATIC void 	hlib_hash_SetShift 			(HxHASH_t *hash_table, HINT32 shift);
STATIC HINT32	hlib_hash_Find_ClosetShift 	(HINT32 n);
STATIC void 	hlib_hash_SetShift_From_Size (HxHASH_t *hash_table , HINT32 size);
STATIC void 	hlib_hash_RemoveNode 		(HxHASH_t *hash_table , HxHashNode_t *node , HBOOL notify);
STATIC void 	hlib_hash_RemoveAllNodes 	(HxHASH_t *hash_table , HBOOL    notify);
STATIC void 	hlib_hash_Resize 			(HxHASH_t *hash_table);
STATIC void 	hlib_hash_IterRemoveOrSteal 	(RealIter *ri, HBOOL notify);
STATIC void 	hlib_hash_InsertInternal		(HxHASH_t *hash_table , void* key ,void* value , HBOOL keep_new_key);
STATIC HBOOL	hlib_hash_RemoveInternal		(HxHASH_t *hash_table , const void*  key , HBOOL notify);
STATIC HUINT32 	hlib_hash_ForeachRemoveOrSteal(HxHASH_t *hash_table , _HxHRFunc func , void*  user_data , HBOOL notify);
STATIC void* 	hlib_hash_Malloc					(HUINT32 size);
STATIC INLINE void hlib_hash_MaybeResize		(HxHASH_t *hash_table);
STATIC INLINE HUINT32 hlib_hash_LookupNode 	(HxHASH_t *hash_table , const void*  key);
STATIC INLINE HUINT32 hlib_hash_LookupNodeForInsertion	(HxHASH_t *hash_table , const void* key , HUINT32 *hash_return);




STATIC void hlib_hash_SetShift (HxHASH_t *hash_table, HINT32 shift)
{
	HINT32 i;
	HUINT32 mask = 0;

	hash_table->size = 1 << shift;
	hash_table->mod  = prime_mod [shift];

	for (i = 0; i < shift; i++)
	{
	  mask <<= 1;
	  mask |= 1;
	}

	hash_table->mask = mask;
}



STATIC HINT32 hlib_hash_Find_ClosetShift (HINT32 n)
{
	HINT32 i;

	for (i = 0; n; i++)
	n >>= 1;

	return i;
}



STATIC void hlib_hash_SetShift_From_Size (HxHASH_t *hash_table , HINT32 size)
{
	HINT32 shift;

	shift = hlib_hash_Find_ClosetShift (size);
	shift = MAX (shift, HXHASH_TABLE_MIN_SHIFT);

	hlib_hash_SetShift (hash_table, shift);
}


STATIC INLINE HUINT32 hlib_hash_LookupNode (HxHASH_t *hash_table , const void*  key)
{
	HxHashNode_t *node;
	HUINT32 node_index;
	HUINT32 hash_value;
	HUINT32 step = 0;

	hash_value = (* hash_table->hash_func) (key);
	if (hash_value <= 1)
	hash_value = 2;

	node_index = hash_value % hash_table->mod;
	node = &hash_table->nodes [node_index];

	while (node->key_hash)
	{

	  if (node->key_hash == hash_value)
	  {
	  	if (hash_table->key_equal_func)
	    {
			if (hash_table->key_equal_func (node->key, key))
				break;
	    }
	    else if (node->key == key)
	    {
			break;
	    }
	  }

	  step++;
	  node_index += step;
	  node_index &= hash_table->mask;
	  node = &hash_table->nodes [node_index];
	}

	return node_index;
}


STATIC INLINE HUINT32 hlib_hash_LookupNodeForInsertion	(HxHASH_t	*hash_table,
                                        const void*  	key,
                                        HUINT32         *hash_return)
{
	HxHashNode_t *node;
	HUINT32 node_index;
	HUINT32 hash_value;
	HUINT32 first_tombstone = 0;
	HBOOL have_tombstone = FALSE;
	HUINT32 step = 0;

	hash_value = (* hash_table->hash_func) (key);
	//printf("hash_value  : %d    ,  key  : %d    key str :  %s \n",hash_value,key,key);
	if (hash_value <= 1)
	hash_value = 2;

	*hash_return = hash_value;

	node_index = hash_value % hash_table->mod;
	node = &hash_table->nodes [node_index];

	while (node->key_hash)
	{

	  if (node->key_hash == hash_value)
      {
        if (hash_table->key_equal_func)
          {
            if (hash_table->key_equal_func (node->key, key))
              return node_index;
          }
        else if (node->key == key)
          {
            return node_index;
          }
      }
      else if (node->key_hash == 1 && !have_tombstone)
      {
        first_tombstone = node_index;
        have_tombstone = TRUE;
      }

	  step++;
	  node_index += step;
	  node_index &= hash_table->mask;
	  node = &hash_table->nodes [node_index];
	}

	if (have_tombstone)
		return first_tombstone;

	return node_index;
}



STATIC void hlib_hash_RemoveNode (HxHASH_t *hash_table , HxHashNode_t *node , HBOOL notify)
{
	if (notify && hash_table->key_destroy_func)
	hash_table->key_destroy_func (node->key);

	if (notify && hash_table->value_destroy_func)
	hash_table->value_destroy_func (node->value);

	/* Erect tombstone */
	node->key_hash = 1;

	/* Be GC friendly */
	node->key = NULL;
	node->value = NULL;

	hash_table->nnodes--;
}



STATIC void hlib_hash_RemoveAllNodes (HxHASH_t *hash_table , HBOOL    notify)
{
	int i;

	for (i = 0; i < hash_table->size; i++)
	{
	  HxHashNode_t *node = &hash_table->nodes [i];

	  if (node->key_hash > 1)
      {
        if (notify && hash_table->key_destroy_func)
          hash_table->key_destroy_func (node->key);

        if (notify && hash_table->value_destroy_func)
          hash_table->value_destroy_func (node->value);
      }
	}

	HxSTD_MemSet(hash_table->nodes, 0, hash_table->size * sizeof (HxHashNode_t));

	hash_table->nnodes = 0;
	hash_table->noccupied = 0;
}



STATIC void hlib_hash_Resize (HxHASH_t *hash_table)
{
	HxHashNode_t *new_nodes;
	HINT32 old_size;
	HINT32 i;

	old_size = hash_table->size;
	hlib_hash_SetShift_From_Size (hash_table, hash_table->nnodes * 2);

#if defined(CONFIG_DEBUG)
	new_nodes = DS_MEM_Calloc(sizeof(HxHashNode_t)*hash_table->size, hash_table, hash_table->debugText, hash_table->debugLine);
#else
	new_nodes = hlib_hash_Malloc(sizeof(HxHashNode_t)*hash_table->size);
#endif

	for (i = 0; i < old_size; i++)
	{
		HxHashNode_t *node = &hash_table->nodes [i];
		HxHashNode_t *new_node;
		HUINT32 hash_val;
		HUINT32 step = 0;

		if (node->key_hash <= 1)
			continue;

		hash_val = node->key_hash % hash_table->mod;
		new_node = &new_nodes [hash_val];

		while (new_node->key_hash)
		{
		step++;
		hash_val += step;
		hash_val &= hash_table->mask;
		new_node = &new_nodes [hash_val];
		}

		*new_node = *node;
	}

#if defined(CONFIG_DEBUG)
	DS_MEM_Free(hash_table->nodes, hash_table, hash_table->debugText, hash_table->debugLine);
#else
	HLIB_MEM_Free(hash_table->nodes);
#endif
	hash_table->nodes = new_nodes;
	hash_table->noccupied = hash_table->nnodes;
}



STATIC INLINE void hlib_hash_MaybeResize(HxHASH_t *hash_table)
{
	HINT32 noccupied = hash_table->noccupied;
	HINT32 size = hash_table->size;

	if ((size > hash_table->nnodes * 4 && size > 1 << HXHASH_TABLE_MIN_SHIFT) ||
	  (size <= noccupied + (noccupied / 16)))
		hlib_hash_Resize (hash_table);
}


HxHASH_t* _HLIB_HASH_New (_HxHashFunc    hash_func,
                  				_HxEqualFunc   key_equal_func, const char* file, const int line)
{
	return _HLIB_HASH_NewFull (hash_func, key_equal_func, NULL, NULL, file, line);
}

HxHASH_t* _HLIB_HASH_SelfNew (const char* file, const int line)
{
	return _HLIB_HASH_NewFull (HLIB_HASH_StrHash, HLIB_HASH_StrEqual, NULL, NULL, file, line);
}


HxHASH_t* _HLIB_HASH_NewFull (_HxHashFunc       hash_func,
                     	  			_HxEqualFunc      key_equal_func,
                       				_HxDestroyNotify  key_destroy_func,
                       				_HxDestroyNotify  value_destroy_func
                       				, const char* file, const int line)
{
	HxHASH_t *hash_table;
#if defined(CONFIG_DEBUG)
	hash_table = DS_MEM_Calloc(sizeof(HxHASH_t), NULL, file, line);
#else
	hash_table = hlib_hash_Malloc(sizeof(HxHASH_t));
#endif

#if defined(CONFIG_DEBUG)
	hash_table->debugText	= (HCHAR*)file;
	hash_table->debugLine	= (HUINT32)line;
#endif
	hlib_hash_SetShift (hash_table, HXHASH_TABLE_MIN_SHIFT);
	hash_table->nnodes             = 0;
	hash_table->noccupied          = 0;
	hash_table->hash_func          = hash_func ? hash_func : NULL;
	hash_table->key_equal_func     = key_equal_func;
	hash_table->ref_count          = 1;
	hash_table->key_destroy_func   = key_destroy_func;
	hash_table->value_destroy_func = value_destroy_func;
#if defined(CONFIG_DEBUG)
	hash_table->nodes              = DS_MEM_Calloc(sizeof(HxHashNode_t) * (1 << HXHASH_TABLE_MIN_SHIFT), hash_table, file, line);
#else
	hash_table->nodes              = hlib_hash_Malloc(sizeof(HxHashNode_t) * (1 << HXHASH_TABLE_MIN_SHIFT));
#endif

	return hash_table;
}


void HLIB_HASH_IterInit (HxHASH_TableIter_t *iter,	HxHASH_t *hash_table)
{
	RealIter *ri = (RealIter *) iter;

	if(iter==NULL)
		return ;
	if(hash_table == NULL)
		return;

	ri->hash_table = hash_table;
	ri->position = -1;
}



HBOOL HLIB_HASH_IterNext	(HxHASH_TableIter_t *iter,
							void*       *key,
							void*       *value)
{
	RealIter *ri = (RealIter *) iter;
	HxHashNode_t *node;
	HINT32 position;

	if(iter == NULL)
	return FALSE;

	position = ri->position;

	do
	{
		position++;
		if (position >= ri->hash_table->size)
		{
		  ri->position = position;
		  return FALSE;
		}

		node = &ri->hash_table->nodes [position];
	}
	while (node->key_hash <= 1);

	if (key != NULL)
		*key = node->key;
	if (value != NULL)
		*value = node->value;

	ri->position = position;
	return TRUE;
}



HxHASH_t *HLIB_HASH_IterGetHashTable (HxHASH_TableIter_t *iter)
{
	if(iter == NULL)
		return NULL;

	return ((RealIter *) iter)->hash_table;
}

STATIC void hlib_hash_IterRemoveOrSteal (RealIter *ri, HBOOL notify)
{
	if(ri == NULL)
		return ;

	hlib_hash_RemoveNode (ri->hash_table, &ri->hash_table->nodes [ri->position], notify);

}



void HLIB_HASH_IterRemove (HxHASH_TableIter_t *iter)
{
	hlib_hash_IterRemoveOrSteal ((RealIter *) iter, TRUE);
}



void HLIB_HASH_IterSteal (HxHASH_TableIter_t *iter)
{
	hlib_hash_IterRemoveOrSteal ((RealIter *) iter, FALSE);
}

void	HLIB_HASH_Unref				(HxHASH_t     *hash_table)
{
	if (hash_table == NULL)	return ;

#if defined(CONFIG_DEBUG)
	DS_MEM_Free(hash_table->nodes, hash_table, hash_table->debugText, hash_table->debugLine);
	DS_MEM_Free(hash_table, hash_table, hash_table->debugText, hash_table->debugLine);
#else
	HLIB_MEM_Free(hash_table->nodes);
	HLIB_MEM_Free(hash_table);
#endif
	hash_table = NULL;
}

void HLIB_HASH_Destroy (HxHASH_t *hash_table)
{
	if(hash_table==NULL)
		return ;
	if(hash_table->ref_count <= 0)
		return ;

	HLIB_HASH_RemoveAll (hash_table);
	HLIB_HASH_Unref (hash_table);
}



void* HLIB_HASH_Lookup (HxHASH_t *hash_table , const void* key)
{
	HxHashNode_t *node;
	HUINT32      node_index;

	if(hash_table == NULL)
		return NULL;

	node_index = hlib_hash_LookupNode (hash_table, key);
	node = &hash_table->nodes [node_index];

	return node->key_hash ? node->value : NULL;
}



HBOOL HLIB_HASH_LookupExtended(HxHASH_t *hash_table , const void*  lookup_key,
								void* *orig_key , void* *value)
{
	HxHashNode_t *node;
	HUINT32      node_index;

	if(hash_table == NULL)
		return FALSE;

	node_index = hlib_hash_LookupNode (hash_table, lookup_key);
	node = &hash_table->nodes [node_index];

	if (!node->key_hash)
		return FALSE;

	if (orig_key)
		*orig_key = node->key;

	if (value)
		*value = node->value;

	return TRUE;
}

void* HLIB_HASH_Detach (HxHASH_t *hash_table, const void* key)
{
	void *value;

	value = HLIB_HASH_Lookup(hash_table, key);
	if (value)
		HLIB_HASH_Steal(hash_table, key);
	return value;
}

STATIC void hlib_hash_InsertInternal(HxHASH_t *hash_table , void* key,
										void* value , HBOOL keep_new_key)
{
	HxHashNode_t *node;
	HUINT32 node_index;
	HUINT32 key_hash;
	HUINT32 old_hash;

	if(hash_table == NULL)
		return ;
	if(hash_table->ref_count <= 0)
		return ;

	node_index = hlib_hash_LookupNodeForInsertion (hash_table, key, &key_hash);
	node = &hash_table->nodes [node_index];

	old_hash = node->key_hash;

	if (old_hash > 1)
	{
		if (keep_new_key)
		{
		  if (hash_table->key_destroy_func)
		  	hash_table->key_destroy_func (node->key);
		  node->key = key;
		}
		else
		{
		  if (hash_table->key_destroy_func)
		    hash_table->key_destroy_func (key);
		}

		if (hash_table->value_destroy_func)
			hash_table->value_destroy_func (node->value);

		node->value = value;
	}
	else
	{
		node->key = key;
		node->value = value;
		node->key_hash = key_hash;

		hash_table->nnodes++;

		if (old_hash == 0)
		{
			hash_table->noccupied++;
			hlib_hash_MaybeResize (hash_table);
		}

	}
}



void HLIB_HASH_Insert(HxHASH_t *hash_table , void* key , void* value)
{
	hlib_hash_InsertInternal (hash_table, key, value, FALSE);
}

void HLIB_HASH_Replace(HxHASH_t *hash_table , void* key , void* value)
{
	hlib_hash_InsertInternal (hash_table, key, value, TRUE);
}

HBOOL HLIB_HASH_StrEqual(const void* v1 , const void* v2)
{
	const char *string1 = v1;
	const char *string2 = v2;

	return strcmp (string1, string2) == 0;
}


HUINT32 HLIB_HASH_StrHash (const void* v)
{
	/* 31 bit hash function */
	const signed char *p = v;
	HUINT32 h = *p;

	if (h)
	for (p += 1; *p != '\0'; p++)
		h = (h << 5) - h + *p;

	return h;
}


STATIC HBOOL hlib_hash_RemoveInternal(HxHASH_t *hash_table , const void*  key , HBOOL notify)
{
	HxHashNode_t *node;
	HUINT32 node_index;

	if(hash_table == NULL)
		return FALSE;

	node_index = hlib_hash_LookupNode (hash_table, key);
	node = &hash_table->nodes [node_index];

	if (!node->key_hash)
		return FALSE;

	hlib_hash_RemoveNode (hash_table, node, notify);
	hlib_hash_MaybeResize (hash_table);

	return TRUE;
}



HBOOL HLIB_HASH_Remove (HxHASH_t *hash_table , const void* key)
{
	return hlib_hash_RemoveInternal (hash_table, key, TRUE);
}



HBOOL HLIB_HASH_Steal (HxHASH_t *hash_table , const void*  key)
{
	return hlib_hash_RemoveInternal (hash_table, key, FALSE);
}



void HLIB_HASH_RemoveAll(HxHASH_t *hash_table)
{
	if(hash_table == NULL)
		return ;

	hlib_hash_RemoveAllNodes (hash_table, TRUE);
	hlib_hash_MaybeResize (hash_table);
}



void HLIB_HASH_StealAll (HxHASH_t *hash_table)
{
	if(hash_table == NULL)
		return ;

	hlib_hash_RemoveAllNodes (hash_table, FALSE);
	hlib_hash_MaybeResize (hash_table);

}



STATIC HUINT32 hlib_hash_ForeachRemoveOrSteal(HxHASH_t *hash_table,
												 _HxHRFunc func,
                                      			 void*  user_data,
                                      			 HBOOL notify)
{
	HUINT32 deleted = 0;
	HINT32 i;

	for (i = 0; i < hash_table->size; i++)
	{
		HxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1 && (* func) (node->key, node->value, user_data))
		{
			hlib_hash_RemoveNode (hash_table, node, notify);
			deleted++;
		}
	}

	hlib_hash_MaybeResize (hash_table);

	return deleted;
}



HUINT32 HLIB_HASH_ForeachRemove (HxHASH_t *hash_table , _HxHRFunc func , void* user_data)
{
	if(hash_table == NULL)
		return 0;
	if(func == NULL)
		return 0;

	return hlib_hash_ForeachRemoveOrSteal (hash_table, func, user_data, TRUE);
}



HUINT32 HLIB_HASH_ForeachSteal (HxHASH_t *hash_table , _HxHRFunc func , void* user_data)
{
	if(hash_table == NULL)
		return 0;
	if(func == NULL)
		return 0;

	return hlib_hash_ForeachRemoveOrSteal (hash_table, func, user_data, FALSE);
}



void HLIB_HASH_Foreach (HxHASH_t *hash_table , _HxHFunc func , void* user_data)
{
	HINT32 i;
	if(hash_table == NULL)
		return ;
	if(func == NULL)
		return ;

	//for(i = 0; i< HLIB_HASH_Size(hash_table) ; i++)
	for(i = 0; i<hash_table->size ; i++)
	{
		HxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1)
			(* func) (node->key, node->value, user_data);
	}
}



void* HLIB_HASH_Find (HxHASH_t *hash_table , _HxHRFunc predicate , void* user_data)
{
	HINT32 i;

	if(hash_table == NULL)
		return NULL;
	if(predicate == NULL)
		return NULL;

	for (i = 0; i < hash_table->size; i++)
	{
		HxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1 && predicate (node->key, node->value, user_data))
			return node->value;
	}

	return NULL;
}



HUINT32 HLIB_HASH_Size (HxHASH_t *hash_table)
{
	if(hash_table == NULL)
		return 0;

	return hash_table->nnodes;
}



HxList_t *HLIB_HASH_GetKeys (HxHASH_t *hash_table)
{
	HINT32 i;
	HxList_t *retval;
	if(hash_table == NULL)
		return NULL;

	retval = NULL;
	for (i = 0; i < hash_table->size; i++)
	{
		HxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1)
			retval = HLIB_LIST_Prepend(retval, node->key);
	}

	return retval;
}



HxList_t *HLIB_HASH_GetValues (HxHASH_t *hash_table)
{
	HINT32 i;
	HxList_t *retval;

	if(hash_table == NULL)
		return NULL;

	retval = NULL;

	for (i = 0; i < hash_table->size; i++)
	{
		HxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1)
			retval = HLIB_LIST_Prepend(retval, node->value);
	}

	return retval;
}





STATIC void* hlib_hash_Malloc(HUINT32 size)
{
	void* mem = HLIB_MEM_Malloc(size);
	if(mem == NULL)
	{
		assert(mem != NULL);
		return NULL;
	}
	if(mem)
		memset(mem,0,size);
	return mem;
}

HUINT32		HLIB_HASH_GetStringKey(const void *key)
{
	const	char*	p = (char*)key;
	HUINT32	h = *p;

	if (h)
	{
		for (p += 1; *p != '\0'; p++)
			h = (h << 5) - h + *p;
	}

	return h;
}

HBOOL		HLIB_HASH_IsStringEqual(const void *a, const void *b)
{
	const	char	*string1 = (char*)a, *string2 = (char*)b;

	return strcmp(string1, string2) == 0;
}



HUINT32	HLIB_HASH_GetIntKey	(const void* key)
{
	return (HUINT32)key;
}

HBOOL	HLIB_HASH_IsIntEqual	(const void* a , const void* b)
{
	return (HUINT32)a == (HUINT32)b;
}



