/*
* rxhash.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rxstdlib.h"
#include "rxhash.h"
#include "rxlist.h"

#define RxHASH_TABLE_MIN_SHIFT 3  /* 1 << 3 == 8 buckets */

typedef struct RxHashNode      RxHashNode_t;

struct RxHashNode
{
	void*   key;
	void*   value;

	/* If key_hash == 0, node is not in use
	* If key_hash == 1, node is a tombstone
	* If key_hash >= 2, node contains data */
	HUINT32      key_hash;
};

struct _RxHashTable
{
	HINT32             size;
	HINT32             mod;
	HUINT32            mask;
	HINT32             nnodes;
	HINT32             noccupied;  /* nnodes + tombstones */
	RxHashNode_t       *nodes;
	_RxHashFunc        hash_func;
	_RxEqualFunc       key_equal_func;
	volatile HINT32    ref_count;
	_RxDestroyNotify   key_destroy_func;
	_RxDestroyNotify   value_destroy_func;
};

typedef struct
{
	RxHashTable_t  *hash_table;
	void*     dummy1;
	void*     dummy2;
	int          position;
	HBOOL     dummy3;
	int          version;
} RxRealIter;



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

STATIC void 	_rxhashtable_SetShift 			(RxHashTable_t *hash_table, HINT32 shift);
STATIC HINT32	_rxhashtable_Find_ClosetShift 	(HINT32 n);
STATIC void 	_rxhashtable_SetShift_From_Size (RxHashTable_t *hash_table , HINT32 size);
STATIC void 	_rxhashtable_RemoveNode 		(RxHashTable_t *hash_table , RxHashNode_t *node , HBOOL notify);
STATIC void 	_rxhashtable_RemoveAllNodes 	(RxHashTable_t *hash_table , HBOOL    notify);
STATIC void 	_rxhashtable_Resize 			(RxHashTable_t *hash_table);
STATIC void 	_rxhashtable_IterRemoveOrSteal 	(RxRealIter *ri, HBOOL notify);
STATIC void 	_rxhashtable_InsertInternal		(RxHashTable_t *hash_table , void* key ,void* value , HBOOL keep_new_key);
STATIC HBOOL	_rxhashtable_RemoveInternal		(RxHashTable_t *hash_table , const void*  key , HBOOL notify);
STATIC HUINT32 	_rxhashtable_ForeachRemoveOrSteal(RxHashTable_t *hash_table , _RxHRFunc func , void*  user_data , HBOOL notify);
STATIC void* 	_rxhash_Malloc					(HUINT32 size);
STATIC INLINE void _rxhashtable_MaybeResize		(RxHashTable_t *hash_table);
STATIC INLINE HUINT32 _rxhashtable_LookupNode 	(RxHashTable_t *hash_table , const void*  key);
STATIC INLINE HUINT32 _rxhashtable_LookupNodeForInsertion	(RxHashTable_t *hash_table , const void* key , HUINT32 *hash_return);




STATIC void _rxhashtable_SetShift (RxHashTable_t *hash_table, HINT32 shift)
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



STATIC HINT32 _rxhashtable_Find_ClosetShift (HINT32 n)
{
	HINT32 i;

	for (i = 0; n; i++)
	n >>= 1;

	return i;
}



STATIC void _rxhashtable_SetShift_From_Size (RxHashTable_t *hash_table , HINT32 size)
{
	HINT32 shift;

	shift = _rxhashtable_Find_ClosetShift (size);
	shift = MAX (shift, RxHASH_TABLE_MIN_SHIFT);

	_rxhashtable_SetShift (hash_table, shift);
}


STATIC INLINE HUINT32 _rxhashtable_LookupNode (RxHashTable_t *hash_table , const void*  key)
{
	RxHashNode_t *node;
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


STATIC INLINE HUINT32 _rxhashtable_LookupNodeForInsertion	(RxHashTable_t	*hash_table,
                                        const void*  	key,
                                        HUINT32         *hash_return)
{
	RxHashNode_t *node;
	HUINT32 node_index;
	HUINT32 hash_value;
	HUINT32 first_tombstone;
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



STATIC void _rxhashtable_RemoveNode (RxHashTable_t *hash_table , RxHashNode_t *node , HBOOL notify)
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



STATIC void _rxhashtable_RemoveAllNodes (RxHashTable_t *hash_table , HBOOL    notify)
{
	int i;

	for (i = 0; i < hash_table->size; i++)
	{
	  RxHashNode_t *node = &hash_table->nodes [i];

	  if (node->key_hash > 1)
      {
        if (notify && hash_table->key_destroy_func)
          hash_table->key_destroy_func (node->key);

        if (notify && hash_table->value_destroy_func)
          hash_table->value_destroy_func (node->value);
      }
	}

	RxSTD_MemSet(hash_table->nodes, 0, hash_table->size * sizeof (RxHashNode_t));

	hash_table->nnodes = 0;
	hash_table->noccupied = 0;
}



STATIC void _rxhashtable_Resize (RxHashTable_t *hash_table)
{
	RxHashNode_t *new_nodes;
	HINT32 old_size;
	HINT32 i;

	old_size = hash_table->size;
	_rxhashtable_SetShift_From_Size (hash_table, hash_table->nnodes * 2);

	new_nodes = _rxhash_Malloc(sizeof(RxHashNode_t)*hash_table->size);

	for (i = 0; i < old_size; i++)
	{
		RxHashNode_t *node = &hash_table->nodes [i];
		RxHashNode_t *new_node;
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

	RxSTD_Free(hash_table->nodes);
	hash_table->nodes = new_nodes;
	hash_table->noccupied = hash_table->nnodes;
}



STATIC INLINE void _rxhashtable_MaybeResize(RxHashTable_t *hash_table)
{
	HINT32 noccupied = hash_table->noccupied;
	HINT32 size = hash_table->size;

	if ((size > hash_table->nnodes * 4 && size > 1 << RxHASH_TABLE_MIN_SHIFT) ||
	  (size <= noccupied + (noccupied / 16)))

	_rxhashtable_Resize (hash_table);
}



RxHashTable_t* RxHashTable_New (_RxHashFunc    hash_func,
                  				_RxEqualFunc   key_equal_func)
{
	return RxHashTable_NewFull (hash_func, key_equal_func, NULL, NULL);
}

RxHashTable_t* RxHashTable_SelfNew (void)
{
	return RxHashTable_NewFull (RxHashTable_StrHash, RxHashTable_StrEqual, NULL, NULL);
}


RxHashTable_t* RxHashTable_NewFull (_RxHashFunc       hash_func,
                     	  			_RxEqualFunc      key_equal_func,
                       				_RxDestroyNotify  key_destroy_func,
                       				_RxDestroyNotify  value_destroy_func)
{
	RxHashTable_t *hash_table;

	hash_table = _rxhash_Malloc(sizeof(RxHashTable_t));
	_rxhashtable_SetShift (hash_table, RxHASH_TABLE_MIN_SHIFT);
	hash_table->nnodes             = 0;
	hash_table->noccupied          = 0;
	hash_table->hash_func          = hash_func ? hash_func : NULL;
	hash_table->key_equal_func     = key_equal_func;
	hash_table->ref_count          = 1;
	hash_table->key_destroy_func   = key_destroy_func;
	hash_table->value_destroy_func = value_destroy_func;
	hash_table->nodes              = _rxhash_Malloc(sizeof(RxHashNode_t) * (1 << RxHASH_TABLE_MIN_SHIFT));

	return hash_table;
}


void RxHashTable_IterInit (RxHashTableIter_t *iter,	RxHashTable_t *hash_table)
{
	RxRealIter *ri = (RxRealIter *) iter;

	if(iter==NULL)
		return ;
	if(hash_table == NULL)
		return;

	ri->hash_table = hash_table;
	ri->position = -1;
}



HBOOL RxHashTable_IterNext	(RxHashTableIter_t *iter,
							void*       *key,
							void*       *value)
{
	RxRealIter *ri = (RxRealIter *) iter;
	RxHashNode_t *node;
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



RxHashTable_t *RxHashTable_IterGetHashTable (RxHashTableIter_t *iter)
{
	if(iter == NULL)
		return NULL;

	return ((RxRealIter *) iter)->hash_table;
}

STATIC void _rxhashtable_IterRemoveOrSteal (RxRealIter *ri, HBOOL notify)
{
	if(ri == NULL)
		return ;

	_rxhashtable_RemoveNode (ri->hash_table, &ri->hash_table->nodes [ri->position], notify);

}



void RxHashTable_IterRemove (RxHashTableIter_t *iter)
{
	_rxhashtable_IterRemoveOrSteal ((RxRealIter *) iter, TRUE);
}



void RxHashTable_IterSteal (RxHashTableIter_t *iter)
{
	_rxhashtable_IterRemoveOrSteal ((RxRealIter *) iter, FALSE);
}



void RxHashTable_Destroy (RxHashTable_t *hash_table)
{
	if(hash_table==NULL)
		return ;
	if(hash_table->ref_count <= 0)
		return ;

	RxHashTable_RemoveAll (hash_table);
	//MHHashTable_Unref (hash_table);
}



void* RxHashTable_Lookup (RxHashTable_t *hash_table , const void* key)
{
	RxHashNode_t *node;
	HUINT32      node_index;

	if(hash_table == NULL)
		return NULL;

	node_index = _rxhashtable_LookupNode (hash_table, key);
	node = &hash_table->nodes [node_index];

	return node->key_hash ? node->value : NULL;
}



HBOOL RxHashTable_LookupExtended(RxHashTable_t *hash_table , const void*  lookup_key,
								void* *orig_key , void* *value)
{
	RxHashNode_t *node;
	HUINT32      node_index;

	if(hash_table == NULL)
		return FALSE;

	node_index = _rxhashtable_LookupNode (hash_table, lookup_key);
	node = &hash_table->nodes [node_index];

	if (!node->key_hash)
		return FALSE;

	if (orig_key)
		*orig_key = node->key;

	if (value)
		*value = node->value;

	return TRUE;
}



STATIC void _rxhashtable_InsertInternal(RxHashTable_t *hash_table , void* key,
										void* value , HBOOL keep_new_key)
{
	RxHashNode_t *node;
	HUINT32 node_index;
	HUINT32 key_hash;
	HUINT32 old_hash;

	if(hash_table == NULL)
		return ;
	if(hash_table->ref_count <= 0)
		return ;

	node_index = _rxhashtable_LookupNodeForInsertion (hash_table, key, &key_hash);
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
			_rxhashtable_MaybeResize (hash_table);
		}

	}
}



void RxHashTable_Insert(RxHashTable_t *hash_table , void* key , void* value)
{
	_rxhashtable_InsertInternal (hash_table, key, value, FALSE);
}

void RxHashTable_Replace(RxHashTable_t *hash_table , void* key , void* value)
{
	_rxhashtable_InsertInternal (hash_table, key, value, TRUE);
}

HBOOL RxHashTable_StrEqual(const void* v1 , const void* v2)
{
	const char *string1 = v1;
	const char *string2 = v2;

	return strcmp (string1, string2) == 0;
}


HUINT32 RxHashTable_StrHash (const void* v)
{
	/* 31 bit hash function */
	const signed char *p = v;
	HUINT32 h = *p;

	if (h)
	for (p += 1; *p != '\0'; p++)
		h = (h << 5) - h + *p;

	return h;
}


STATIC HBOOL _rxhashtable_RemoveInternal(RxHashTable_t *hash_table , const void*  key , HBOOL notify)
{
	RxHashNode_t *node;
	HUINT32 node_index;

	if(hash_table == NULL)
		return FALSE;

	node_index = _rxhashtable_LookupNode (hash_table, key);
	node = &hash_table->nodes [node_index];

	if (!node->key_hash)
		return FALSE;

	_rxhashtable_RemoveNode (hash_table, node, notify);
	_rxhashtable_MaybeResize (hash_table);

	return TRUE;
}



HBOOL RxHashTable_Remove (RxHashTable_t *hash_table , const void* key)
{
	return _rxhashtable_RemoveInternal (hash_table, key, TRUE);
}



HBOOL RxHashTable_Steal (RxHashTable_t *hash_table , const void*  key)
{
	return _rxhashtable_RemoveInternal (hash_table, key, FALSE);
}



void RxHashTable_RemoveAll(RxHashTable_t *hash_table)
{
	if(hash_table == NULL)
		return ;

	_rxhashtable_RemoveAllNodes (hash_table, TRUE);
	_rxhashtable_MaybeResize (hash_table);
}



void RxHashTable_StealAll (RxHashTable_t *hash_table)
{
	if(hash_table == NULL)
		return ;

	_rxhashtable_RemoveAllNodes (hash_table, FALSE);
	_rxhashtable_MaybeResize (hash_table);

}



STATIC HUINT32 _rxhashtable_ForeachRemoveOrSteal(RxHashTable_t *hash_table,
												 _RxHRFunc func,
                                      			 void*  user_data,
                                      			 HBOOL notify)
{
	HUINT32 deleted = 0;
	HINT32 i;

	for (i = 0; i < hash_table->size; i++)
	{
		RxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1 && (* func) (node->key, node->value, user_data))
		{
			_rxhashtable_RemoveNode (hash_table, node, notify);
			deleted++;
		}
	}

	_rxhashtable_MaybeResize (hash_table);

	return deleted;
}



HUINT32 RxHashTable_ForeachRemove (RxHashTable_t *hash_table , _RxHRFunc func , void* user_data)
{
	if(hash_table == NULL)
		return 0;
	if(func == NULL)
		return 0;

	return _rxhashtable_ForeachRemoveOrSteal (hash_table, func, user_data, TRUE);
}



HUINT32 RxHashTable_ForeachSteal (RxHashTable_t *hash_table , _RxHRFunc func , void* user_data)
{
	if(hash_table == NULL)
		return 0;
	if(func == NULL)
		return 0;

	return _rxhashtable_ForeachRemoveOrSteal (hash_table, func, user_data, FALSE);
}



void RxHashTable_Foreach (RxHashTable_t *hash_table , _RxHFunc func , void* user_data)
{
	HINT32 i;
	if(hash_table == NULL)
		return ;
	if(func == NULL)
		return ;

	//for(i = 0; i< MHHashTable_Size(hash_table) ; i++)
	for(i = 0; i<hash_table->size ; i++)
	{
		RxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1)
			(* func) (node->key, node->value, user_data);
	}
}



void* RxHashTable_Find (RxHashTable_t *hash_table , _RxHRFunc predicate , void* user_data)
{
	HINT32 i;

	if(hash_table == NULL)
		return NULL;
	if(predicate == NULL)
		return NULL;

	for (i = 0; i < hash_table->size; i++)
	{
		RxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1 && predicate (node->key, node->value, user_data))
			return node->value;
	}

	return NULL;
}



HUINT32 RxHashTable_Size (RxHashTable_t *hash_table)
{
	if(hash_table == NULL)
		return 0;

	return hash_table->nnodes;
}



RxList_t *RxHashTable_GetKeys (RxHashTable_t *hash_table)
{
	HINT32 i;
	RxList_t *retval;
	if(hash_table == NULL)
		return NULL;

	retval = NULL;
	for (i = 0; i < hash_table->size; i++)
	{
		RxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1)
			retval = RxList_Prepend(retval, node->key);
	}

	return retval;
}



RxList_t *RxHashTable_GetValues (RxHashTable_t *hash_table)
{
	HINT32 i;
	RxList_t *retval;

	if(hash_table == NULL)
		return NULL;

	retval = NULL;

	for (i = 0; i < hash_table->size; i++)
	{
		RxHashNode_t *node = &hash_table->nodes [i];

		if (node->key_hash > 1)
			retval = RxList_Prepend(retval, node->value);
	}

	return retval;
}





STATIC void* _rxhash_Malloc(HUINT32 size)
{
	void* mem = RxSTD_Alloc(size);
	if(mem == NULL)
	{
		assert(mem != NULL);
		return NULL;
	}
	if(mem)
		memset(mem,0,size);
	return mem;
}

HUINT32		RxHash_Table_String_Key(const void *key)
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

HBOOL		RxHash_Table_String_Equal(const void *a, const void *b)
{
	const	char	*string1 = (char*)a, *string2 = (char*)b;

	return strcmp(string1, string2) == 0;
}
