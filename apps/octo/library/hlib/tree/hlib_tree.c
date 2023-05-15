/*
* hxtree.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "_hlib_stdlib.h"
#include "_hlib_tree.h"
#include "_hlib_mem.h"

#define	OS_malloc	HLIB_MEM_Malloc
#define	OS_free		HLIB_MEM_Free

#define MAX_HXTREE_HEIGHT 40

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

struct _HxTree
{
	HxTREENode			*root;
	HxCompareDataFunc	key_compare;
	HxDestroyNotify		key_destroy_func;
	HxDestroyDataNotify	value_destroy_func;
	void*				key_compare_data;
	HUINT32				nnodes;

	HBOOL				node_cache;
	HxTREENode			*freed;
#if defined(CONFIG_DEBUG)
	HCHAR				* debugText;
	HUINT32 			debugLine;
#endif

};


struct _HxTreeNode
{
	void* 		key;		/* key for this node */
	void* 		value;		/* value stored at this node */
	HxTREENode		*left;		/* left subtree */
	HxTREENode		*right;		/* right subtree */
	HINT8			balance;	/* height (left) 8 height(right) */
	HUINT8			left_child;
	HUINT8			right_child;
};

STATIC INLINE HINT32	btree_DefaultKeyCompareFunc	(const void* a , const void* b , void *userdata);

STATIC INLINE HxTREENode* btree_FirstNode(HxTREE_t *tree);

STATIC INLINE HxTREENode* btree_NodeNext(HxTREENode *node);

STATIC void btree_InsertInternal(HxTREE_t *tree , void* key , void* value , HBOOL replace);

STATIC HxTREENode* btree_NodeNew(HxTREE_t *tree, void* key , void* value);

STATIC void		btree_NodeDestroy (HxTREE_t *tree, HxTREENode *node);

STATIC HxTREENode* btree_NodeBalance (HxTREENode *node);

STATIC HxTREENode* btree_NodeRotateLeft (HxTREENode *node);

STATIC HxTREENode* btree_NodeRotateRight (HxTREENode *node);

STATIC HBOOL btree_RemoveInternal (HxTREE_t *tree , const void*  key , HBOOL steal);

STATIC INLINE HxTREENode *btree_NodePrevious (HxTREENode *node);

STATIC HxTREENode *btree_FindNode (HxTREE_t *tree, const void* key);

STATIC void* btree_NodeSearch (HxTREENode	*node,
							   HxCompareFunc   	search_func,
							   const void*  	data);



HxTREE_t*	_HLIB_TREE_New(HxCompareFunc key_compare_func, const HCHAR * file, HUINT32 line)
{
	return _HLIB_TREE_NewFull((HxCompareDataFunc)key_compare_func,NULL,NULL,NULL, file, line);
}


HxTREE_t*	_HLIB_TREE_NewWithData(HxCompareDataFunc key_compare_func ,	void* key_compare_data, const HCHAR * file, HUINT32 line)
{
	return _HLIB_TREE_NewFull(key_compare_func , key_compare_data , NULL , NULL, file, line);
}


HxTREE_t* 	_HLIB_TREE_NewFull(HxCompareDataFunc	key_compare_func,
									 void*			key_compare_data,
									 HxDestroyNotify		key_destroy_func,
									 HxDestroyNotify		value_destroy_func,
									 const HCHAR * file, HUINT32 line)
{

	HxTREE_t *tree;

	if(key_compare_func == NULL)
		key_compare_func = btree_DefaultKeyCompareFunc;
#if defined(CONFIG_DEBUG)
	tree = (HxTREE_t*)DS_MEM_Malloc(sizeof(HxTREE_t), NULL, file, line);
	tree->debugText = (HCHAR*)file;
	tree->debugLine = line;
#else
    tree = (HxTREE_t*)OS_malloc(sizeof(HxTREE_t));
#endif
    tree->root               = NULL;
    tree->key_compare        = key_compare_func;
    tree->key_destroy_func   = key_destroy_func;
    tree->value_destroy_func = (HxDestroyDataNotify)value_destroy_func;
    tree->key_compare_data   = key_compare_data;
    tree->nnodes             = 0;
	tree->node_cache         = FALSE;
	tree->freed              = NULL;

    return tree;

}




void	HLIB_TREE_Destroy(HxTREE_t				*tree)
{
	HxTREENode *node;
	HxTREENode *next;

	if(tree == NULL)
		return ;

	node = btree_FirstNode (tree);

	while (node)
	{
	  next = btree_NodeNext (node);

	  if (tree->key_destroy_func)
	  	tree->key_destroy_func (node->key);
	  if (tree->value_destroy_func)
	  	tree->value_destroy_func (node->value, tree->key_compare_data);

	  btree_NodeDestroy(tree, node);

	  node = next;
	}

	node = tree->freed;
	while (node)
	{
		next = node->left;
#if defined(CONFIG_DEBUG)
		DS_MEM_Free(node, tree, tree->debugText ,tree->debugLine);
#else
		OS_free(node);
#endif
		node = next;
	}
#if defined(CONFIG_DEBUG)
	DS_MEM_Free(tree, tree, tree->debugText ,tree->debugLine);
#else
	OS_free(tree);
#endif
}

void	HLIB_TREE_NodeCache (HxTREE_t *tree, HBOOL enable)
{
	tree->node_cache = enable;
}

HxTREE_t*	HLIB_TREE_Clone (HxTREE_t *tree, HxCloneFunc keyDup, HxCloneFunc valueDup)
{
	HxTREE_t	*clone;

	if (tree == NULL)
		return NULL;

	clone = HLIB_TREE_NewFull(
					tree->key_compare,
					tree->key_compare_data,
					tree->key_destroy_func,
					(HxDestroyNotify)tree->value_destroy_func
				);
	if (clone == NULL)
		return NULL;

	HLIB_TREE_Extend(clone, tree, keyDup, valueDup);

	return clone;
}

void	HLIB_TREE_Extend (HxTREE_t *tree, HxTREE_t *other, HxCloneFunc keyDup, HxCloneFunc valueDup)
{
	HxTREENode	*node;

	if(tree == NULL || other == NULL)
		return ;

	if (keyDup && valueDup)
	{
		for (node = btree_FirstNode(other) ; node ; node = btree_NodeNext(node))
			HLIB_TREE_Insert(tree, keyDup(node->key), valueDup(node->value));
	}
	else if (keyDup)
	{
		for (node = btree_FirstNode(other) ; node ; node = btree_NodeNext(node))
			HLIB_TREE_Insert(tree, keyDup(node->key), node->value);
	}
	else if (valueDup)
	{
		for (node = btree_FirstNode(other) ; node ; node = btree_NodeNext(node))
			HLIB_TREE_Insert(tree, node->key, valueDup(node->value));
	}
	else
	{
		for (node = btree_FirstNode(other) ; node ; node = btree_NodeNext(node))
			HLIB_TREE_Insert(tree, node->key, node->value);
	}
}

void	HLIB_TREE_RemoveAll (HxTREE_t *tree)
{
	HxTREENode *node;
	HxTREENode *next;

	if(tree == NULL)
		return ;

	node = btree_FirstNode (tree);

	while (node)
	{
	  next = btree_NodeNext (node);

	  if (tree->key_destroy_func)
	  	tree->key_destroy_func (node->key);
	  if (tree->value_destroy_func)
	  	tree->value_destroy_func (node->value, tree->key_compare_data);

	  btree_NodeDestroy(tree, node);

	  node = next;
	}

	tree->root = NULL;
	tree->nnodes = 0;
}


void 	HLIB_TREE_Insert(HxTREE_t				*tree,
									 void*			key,
									 void*			value)
{
	if(tree==NULL)
		return ;

	btree_InsertInternal(tree,key,value , FALSE);

}




void	HLIB_TREE_Replace(HxTREE_t *tree , void* key , void* value)
{
	if(tree == NULL)
		return ;
	btree_InsertInternal(tree , key , value, TRUE);

}




HBOOL	HLIB_TREE_Remove(HxTREE_t *tree , void* key)
{
	HBOOL removed;
	if(tree == NULL)
		return FALSE;

	removed = btree_RemoveInternal(tree , key , FALSE);
	return removed;
}




HBOOL	HLIB_TREE_Steal(HxTREE_t *tree , const void* key)
{
	HBOOL removed;
	if(tree == NULL)
		return FALSE;

	removed = btree_RemoveInternal(tree,key,TRUE);
	return removed;
}




void* HLIB_TREE_Lookup(HxTREE_t *tree , const void* key)
{
	HxTREENode *node;
	if(tree == NULL)
		return NULL;
	node = btree_FindNode(tree , key);
	return node ? node->value : NULL;
}



HBOOL	HLIB_TREE_LookupExtended(HxTREE_t *tree , const void* lookup_key ,
							 void* *orig_key , void* *value)
{
	HxTREENode *node;
	if(tree == NULL)
		return FALSE;

	node = btree_FindNode(tree,lookup_key);
	if(node)
	{
		if(orig_key)
			*orig_key = node->key;
		if(value)
			*value = node->value;
		return TRUE;
	}
	else
		return FALSE;
}



void	HLIB_TREE_Foreach(HxTREE_t				*tree,
					 HxTraverseFunc		func,
					 void*			user_data)
{
	HxTREENode *node;
	if(tree == NULL)
		return ;
	if(!tree->root)
		return ;

	node = btree_FirstNode(tree);

	while(node)
	{
		if((*func)(node->key,node->value,user_data))
			break;

		node = btree_NodeNext(node);
	}
}




void* HLIB_TREE_Search(HxTREE_t				*tree,
					HxCompareFunc		search_func,
					const void*		user_data)
{
	if(tree == NULL)
		return NULL;
	if(tree->root)
		return btree_NodeSearch(tree->root , search_func , user_data);
	else
		return NULL;
}



HINT32	HLIB_TREE_Height(HxTREE_t *tree)
{
	HxTREENode *node;
	HINT32 height;

	if(tree == NULL)
		return 0;

	if(!tree->root)
		return 0;

	height = 0;
	node = tree->root;

	while(1)
	{
		height += 1 + MAX(node->balance,0);
		if(!node->left_child)
			return height;
		node = node->left;
	}
}



HINT32	HLIB_TREE_Nnodes(HxTREE_t	*tree)
{
	if(tree == NULL)
		return 0;
	return tree->nnodes;
}

void *		HLIB_TREE_Iterator (HxTREE_t *tree)
{
	if (tree == NULL)
		return NULL;
	return (void *)btree_FirstNode(tree);
}

void *		HLIB_TREE_IteratorKey (void *iter)
{
	if (iter == NULL)
		return NULL;
	return ((HxTREENode *)iter)->key;
}

void *		HLIB_TREE_IteratorValue (void *iter)
{
	if (iter == NULL)
		return NULL;
	return ((HxTREENode *)iter)->value;
}

void *		HLIB_TREE_NextIterator (void *iter)
{
	if (iter == NULL)
		return NULL;
	return (void *)btree_NodeNext((HxTREENode *)iter);
}



STATIC INLINE HINT32	btree_DefaultKeyCompareFunc	(const void* a , const void* b , void *userdata)
{
	(void)userdata;
	return (HINT32)a - (HINT32)b;
}

STATIC INLINE HxTREENode* btree_FirstNode(HxTREE_t *tree)
{
	HxTREENode *tmp;

	if(!tree->root)
		return NULL;

	tmp = tree->root;
	while(tmp->left_child)
		tmp = tmp->left;

	return tmp;
}


STATIC INLINE HxTREENode* btree_NodeNext(HxTREENode *node)
{
	HxTREENode *tmp;
	tmp = node->right;

	if(node->right_child)
		while(tmp->left_child)
			tmp = tmp->left;

	return tmp;
}


STATIC void btree_InsertInternal(HxTREE_t *tree , void* key , void* value , HBOOL replace)
{
	HxTREENode *node;
	HxTREENode *path[MAX_HXTREE_HEIGHT];
	int idx;

	if(tree == NULL)
		return ;

	if (!tree->root)
	{
		tree->root = btree_NodeNew (tree, key,value);
		tree->nnodes++;
		return;
	}

	idx = 0;
	path[idx++] = NULL;
	node = tree->root;

  	while (1)
	{
		int cmp = tree->key_compare (key, node->key, tree->key_compare_data);

		if (cmp == 0)
		{
			if (tree->value_destroy_func)
				tree->value_destroy_func (node->value, tree->key_compare_data);

			node->value = value;

			if (replace)
			{
				if (tree->key_destroy_func)
					tree->key_destroy_func (node->key);

				node->key = key;
			}
			else
			{

				if (tree->key_destroy_func)
					tree->key_destroy_func (key);
			}

			return;
		}

		else if (cmp < 0)
		{
			if (node->left_child)
			{
				assert(idx < MAX_HXTREE_HEIGHT);
				path[idx++] = node;
				node = node->left;
			}
			else
			{
				HxTREENode *child = btree_NodeNew (tree, key, value);

				child->left = node->left;
				child->right = node;
				node->left = child;
				node->left_child = TRUE;
				node->balance -= 1;

				tree->nnodes++;

				break;
			}
		}

		else
		{
			if (node->right_child)
			{
				assert(idx < MAX_HXTREE_HEIGHT);
				path[idx++] = node;
				node = node->right;
			}
			else
			{
				HxTREENode *child = btree_NodeNew (tree, key, value);
				child->right = node->right;
				child->left = node;
				node->right = child;
				node->right_child = TRUE;
				node->balance += 1;
				tree->nnodes++;
				break;
			}
		}
	}


	while (1)
	{
		HxTREENode *bparent = path[--idx];
		HBOOL left_node = (bparent && node == bparent->left);
		//g_assert (!bparent || bparent->left == node || bparent->right == node);

		if (node->balance < -1 || node->balance > 1)
		{
			node = btree_NodeBalance (node);
			if (bparent == NULL)
				tree->root = node;
			else if (left_node)
				bparent->left = node;
			else
				bparent->right = node;
		}

		if (node->balance == 0 || bparent == NULL)
			break;

		if (left_node)
			bparent->balance -= 1;
		else
			bparent->balance += 1;

		node = bparent;
	}
}



STATIC HxTREENode* btree_NodeNew(HxTREE_t *tree, void* key , void* value)
{
	HxTREENode *node;

	if (tree->freed)
	{
		node = tree->freed;
		tree->freed = node->left;
	}
	else
	{
#if defined(CONFIG_DEBUG)
		node = (HxTREENode*)DS_MEM_Malloc(sizeof(HxTREENode), tree, tree->debugText , tree->debugLine);
#else
		node = (HxTREENode*)OS_malloc(sizeof(HxTREENode));
#endif
	}

	if (node)
	{
		node->balance		=	0;
		node->left			=	NULL;
		node->right			=	NULL;
		node->left_child	= 	FALSE;
		node->right_child	=	FALSE;
		node->key			=	key;
		node->value			=	value;
	}
	return node;
}

STATIC void		btree_NodeDestroy (HxTREE_t *tree, HxTREENode *node)
{
	if (tree->node_cache)
	{
		node->left  = tree->freed;
		tree->freed = node;
	}
	else
	{
#if defined(CONFIG_DEBUG)
		DS_MEM_Free(node, tree, tree->debugText ,tree->debugLine);
#else
		OS_free(node);
#endif
	}
}

STATIC HxTREENode* btree_NodeBalance (HxTREENode *node)
{
  if (node->balance < -1)
  {
  	if (node->left->balance > 0)
		node->left = btree_NodeRotateLeft (node->left);
    node = btree_NodeRotateRight (node);
  }
  else if (node->balance > 1)
  {
  	if (node->right->balance < 0)
		node->right = btree_NodeRotateRight (node->right);
	node = btree_NodeRotateLeft (node);
  }
  return node;
}



STATIC HxTREENode* btree_NodeRotateLeft (HxTREENode *node)
{
  HxTREENode *right;
  HINT32 a_bal;
  HINT32 b_bal;

  right = node->right;

  if (right->left_child)
    node->right = right->left;
  else
    {
      node->right_child = FALSE;
      node->right = right;
      right->left_child = TRUE;
    }
  right->left = node;

  a_bal = node->balance;
  b_bal = right->balance;

  if (b_bal <= 0)
    {
      if (a_bal >= 1)
	right->balance = b_bal - 1;
      else
	right->balance = a_bal + b_bal - 2;
      node->balance = a_bal - 1;
    }
  else
    {
      if (a_bal <= b_bal)
	right->balance = a_bal - 2;
      else
	right->balance = b_bal - 1;
      node->balance = a_bal - b_bal - 1;
    }

  return right;
}

STATIC HxTREENode* btree_NodeRotateRight (HxTREENode *node)
{
  HxTREENode *left;
  HINT32 a_bal;
  HINT32 b_bal;

  left = node->left;

  if (left->right_child)
    node->left = left->right;
  else
    {
      node->left_child = FALSE;
      node->left = left;
      left->right_child = TRUE;
    }
  left->right = node;

  a_bal = node->balance;
  b_bal = left->balance;

  if (b_bal <= 0)
    {
      if (b_bal > a_bal)
	left->balance = b_bal + 1;
      else
	left->balance = a_bal + 2;
      node->balance = a_bal - b_bal + 1;
    }
  else
    {
      if (a_bal <= -1)
	left->balance = b_bal + 1;
      else
	left->balance = a_bal + b_bal + 2;
      node->balance = a_bal + 1;
    }

  return left;
}



STATIC HBOOL btree_RemoveInternal (HxTREE_t *tree , const void*  key , HBOOL steal)
{
	HxTREENode *node, *parent, *balance;
	HxTREENode *path[MAX_HXTREE_HEIGHT];
	int idx;
	HBOOL left_node;

	if(tree == NULL)
		return FALSE;

	if (!tree->root)
		return FALSE;

	idx = 0;
	path[idx++] = NULL;
	node = tree->root;

	while (1)
	{
		int cmp = tree->key_compare (key, node->key, tree->key_compare_data);

		if (cmp == 0)
		break;
		else if (cmp < 0)
		{
		  if (!node->left_child)
		    return FALSE;

			assert(idx < MAX_HXTREE_HEIGHT);
			path[idx++] = node;
			node = node->left;
		}
		else
		{
			if (!node->right_child)
		    	return FALSE;

			assert(idx < MAX_HXTREE_HEIGHT);
			path[idx++] = node;
			node = node->right;
		}
	}

	/* the following code is almost equal to g_tree_remove_node,
	 except that we do not have to call g_tree_node_parent. */
	balance = parent = path[--idx];
	//g_assert (!parent || parent->left == node || parent->right == node);
	left_node = (parent && node == parent->left);

	if (!node->left_child)
	{
		if (!node->right_child)
	  	{
		  	if (!parent)
				tree->root = NULL;
			else if (left_node)
		    {
		    	parent->left_child = FALSE;
				parent->left = node->left;
				parent->balance += 1;
			}
			else
			{
				parent->right_child = FALSE;
				parent->right = node->right;
				parent->balance -= 1;
			}
		}
		else /* node has a right child */
	    {
	    	HxTREENode *tmp = btree_NodeNext (node);
			tmp->left = node->left;

			if (!parent)
				tree->root = node->right;
			else if (left_node)
	        {
	        	parent->left = node->right;
				parent->balance += 1;
			}
			else
			{
				parent->right = node->right;
				parent->balance -= 1;
	        }
		}
	}
	else /* node has a left child */
	{
		if (!node->right_child)
		{
			HxTREENode *tmp = btree_NodePrevious (node);
			tmp->right = node->right;

			if (parent == NULL)
				tree->root = node->left;
			else if (left_node)
			{
				parent->left = node->left;
				parent->balance += 1;
			}
			else
			{
				parent->right = node->left;
				parent->balance -= 1;
			}
		}
		else /* node has a both children (pant, pant!) */
		{
			HxTREENode *prev = node->left;
			HxTREENode *next = node->right;
			HxTREENode *nextp = node;
			int old_idx = idx + 1;
			idx++;

			/* path[idx] == parent */
			/* find the immediately next node (and its parent) */
			while (next->left_child)
			{
				assert(idx < MAX_HXTREE_HEIGHT - 1);
				path[++idx] = nextp = next;
				next = next->left;
			}
			assert(old_idx < MAX_HXTREE_HEIGHT);
			assert(idx < MAX_HXTREE_HEIGHT);
			path[old_idx] = next;
			balance = path[idx];

			/* remove 'next' from the tree */
			if (nextp != node)
			{
				if (next->right_child)
					nextp->left = next->right;
				else
					nextp->left_child = FALSE;
				nextp->balance += 1;

				next->right_child = TRUE;
				next->right = node->right;
			}
			else
				node->balance -= 1;

			/* set the prev to point to the right place */
			while (prev->right_child)
				prev = prev->right;
			prev->right = next;

			/* prepare 'next' to replace 'node' */
			next->left_child = TRUE;
			next->left = node->left;
			next->balance = node->balance;

			if (!parent)
				tree->root = next;
			else if (left_node)
				parent->left = next;
			else
				parent->right = next;
		}
	}

	/* restore balance */
	if (balance)
	{
		while (1)
		{
			HxTREENode *bparent = path[--idx];
			//g_assert (!bparent || bparent->left == balance || bparent->right == balance);
			left_node = (bparent && balance == bparent->left);

			if(balance->balance < -1 || balance->balance > 1)
			{
				balance = btree_NodeBalance (balance);
				if (!bparent)
					tree->root = balance;
				else if (left_node)
					bparent->left = balance;
				else
					bparent->right = balance;
			}

			if (balance->balance != 0 || !bparent)
				break;

			if (left_node)
				bparent->balance += 1;
			else
				bparent->balance -= 1;

			balance = bparent;
		}
	}

	if (!steal)
	{
		if (tree->key_destroy_func)
			tree->key_destroy_func (node->key);
		if (tree->value_destroy_func)
			tree->value_destroy_func (node->value, tree->key_compare_data);
	}

	//g_slice_free (GTreeNode, node);
	btree_NodeDestroy(tree, node);
	tree->nnodes--;

	return TRUE;
}


STATIC INLINE HxTREENode *btree_NodePrevious (HxTREENode *node)
{
	HxTREENode *tmp;

	tmp = node->left;

	if (node->left_child)
	while (tmp->right_child)
	  tmp = tmp->right;

	return tmp;
}


STATIC HxTREENode *btree_FindNode (HxTREE_t *tree, const void* key)
{
	HxTREENode *node;
	HINT32 cmp;

	node = tree->root;
	if (!node)
		return NULL;

	while (1)
	{
		cmp = tree->key_compare (key, node->key, tree->key_compare_data);
		if (cmp == 0)
			return node;
		else if (cmp < 0)
		{
			if (!node->left_child)
				return NULL;

			node = node->left;
		}
		else
		{
			if (!node->right_child)
				return NULL;
			node = node->right;
		}
	}
}


STATIC void* btree_NodeSearch (HxTREENode	*node,
							   HxCompareFunc   	search_func,
							   const void*  	data)
{
	HINT32 dir;

	if (!node)
		return NULL;

	while (1)
	{
		dir = (* search_func) (node->key, data);
		if (dir == 0)
			return node->value;
		else if (dir < 0)
		{
			if (!node->left_child)
				return NULL;
			node = node->left;
		}
		else
		{
			if (!node->right_child)
				return NULL;

			node = node->right;
		}
	}
}

