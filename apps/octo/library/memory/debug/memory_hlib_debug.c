/*
* memdbg.c
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>

#include <htype.h>
#include <vkernel.h>

#include <memory_debug.h>
#include "_memory_debug_int.h"


#define MAX_TREE_HEIGHT 40

#undef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))

#undef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))


typedef HINT32		(*CompareFunc)			(const void* a , const void* b);
typedef HINT32		(*CompareDataFunc)		(const void* a , const void* b , void *userdata);
typedef void		(*DestroyNotify)		(void* data);
typedef void		(*DestroyDataNotify)	(void* data, void *userdata);
typedef HBOOL		(*TraverseFunc)			(void* key , void* value , void* data);
typedef void *		(*CloneFunc)			(const void *a);


typedef struct _Tree TREE_t;
typedef struct _TREENode TREENode;

struct _Tree
{
	TREENode			*root;
	CompareDataFunc		key_compare;
	DestroyNotify		key_destroy_func;
	DestroyDataNotify	value_destroy_func;
	void*				key_compare_data;
	HUINT32				nnodes;
	HBOOL				node_cache;
	TREENode			*freed;
};


struct _TREENode
{
	void* 		key;		/* key for this node */
	void* 		value;		/* value stored at this node */
	TREENode	*left;		/* left subtree */
	TREENode	*right;		/* right subtree */
	HINT8		balance;	/* height (left) 8 height(right) */
	HUINT8		left_child;
	HUINT8		right_child;
};

STATIC TREE_t*	_TREE_NewFull(CompareDataFunc	key_compare_func, void*	key_compare_data,
								 DestroyNotify key_destroy_func, DestroyDataNotify value_destroy_func);
STATIC void		_TREE_Destroy(TREE_t *tree);
STATIC void		_TREE_RemoveAll (TREE_t *tree);
STATIC void 	_TREE_Insert(TREE_t *tree, void *key, void *value);
STATIC HBOOL	_TREE_Remove(TREE_t *tree , void* key);
STATIC void*	_TREE_Lookup(TREE_t *tree , const void* key);
STATIC void		_TREE_Foreach(TREE_t *tree, TraverseFunc func, void* user_data);
STATIC HINT32	_TREE_Height(TREE_t *tree);
STATIC	HUINT32 _TREE_NODES(TREE_t *tree);
STATIC INLINE TREENode* _FirstNode(TREE_t *tree);



typedef struct
{
	HCHAR	* hlibFile;
	TREE_t	* orgTree;
}_HlibFile_t;

typedef struct
{
	HCHAR	* orgFile;
	HUINT32	orgLine;
}_OrgFileKey_t;

typedef struct
{
	_OrgFileKey_t	orgFileKey;
	TREE_t			* memTree;
}_OrgFile_t;

typedef struct
{
	HUINT32		line;
	HUINT32		time;
	void		* mem;
	HCHAR		* orgFile;
	HCHAR		* hlibFile;
	HUINT32		orgLine;
	void		* structHandle;
}_Mem_t;

typedef enum
{
	eHlibDbgALL			= 0,
	eHlibDbgOrgFile		= 1,
	eHlibDbgHlibFile	= 2,
	eHlibDbgMem			= 3
} MEMDBG_HLIB_t;

typedef struct
{
	MEMDBG_HLIB_t		eDbgType;
	HCHAR				* File;
	void				* mem;
	HUINT32				time;
}_debugtype_t;


void	ds_FreememTree(void* data, void *userdata)
{
	_Mem_t * mem = (_Mem_t *)data;
	if(mem)	free(mem);
}

void	ds_FreeHlibFile(void* data, void *userdata)
{
	_HlibFile_t * hlibFile = (_HlibFile_t *)data;

	if(hlibFile)
	{
		_TREE_RemoveAll(hlibFile->orgTree);
		free(hlibFile);
	}

}

void	ds_FreeOrgFile(void* data, void *userdata)
{
	_OrgFile_t * org = (_OrgFile_t *)data;

	if(org)
	{
		_TREE_RemoveAll(org->memTree);
		free(org);
	}
}

HINT32 ds_compareOrgFile(const void* a , const void* b , void *userdata)
{
	_OrgFile_t * _a = (_OrgFile_t * )a;
	_OrgFile_t * _b = (_OrgFile_t * )b;

	return memcmp(&_a->orgFileKey, &_b->orgFileKey, sizeof(_OrgFileKey_t));
}

typedef HERROR	(*memdbg_printCB)(const HCHAR *format, ...);

STATIC TREE_t			* sHlibFileTree	= NULL;
STATIC HINT32			snStartSec		= 30;
STATIC HINT32			snDurationSec	= 0;
STATIC HUINT32			semaID			= 0;
STATIC memdbg_printCB	debugPrint		= NULL;

STATIC	TREE_t * ds_GetHlibFileTree(void)
{
	if(sHlibFileTree == NULL)
	{
		sHlibFileTree = _TREE_NewFull(NULL, NULL, NULL, ds_FreeHlibFile);
	}
	return sHlibFileTree;
}

STATIC HBOOL	ds_PrintMemTreeInfo(void* key , void* value , void* data)
{
	_Mem_t		* memInfo = (_Mem_t	*)value;
	_debugtype_t * debugType = (_debugtype_t *)data;

	if(debugType->eDbgType == eHlibDbgMem
		&& (debugType->mem != memInfo->structHandle && debugType->mem != memInfo->mem))
	{
		return FALSE;
	}

	if(snStartSec > memInfo->time)
		return FALSE;

	if(debugType->time < snDurationSec + memInfo->time)
		return FALSE;

	if(debugType->eDbgType == eHlibDbgMem)
	{
		debugPrint("\t MEM H[0x%08x] M[0x%08x]\n",memInfo->structHandle, memInfo->mem);
		debugPrint("\t\t HLIB  L[%05d] F[%20s] T[%s]\n", memInfo->line, memInfo->hlibFile,  _memdbg_AscTime (memInfo->time, NULL, 26));
		debugPrint("\t\t ORG   L[%05d] F[%20s]\n", memInfo->orgLine, memInfo->orgFile);
	}
	else
		debugPrint("\t\tMEM H[0x%08x] M[0x%08x] L[%05d] T[%s]\n",memInfo->structHandle, memInfo->mem, memInfo->line, _memdbg_AscTime (memInfo->time, NULL, 26));

	return FALSE;
}

STATIC HBOOL	ds_PrintOrgTreeInfo(void* key , void* value , void* data)
{
	_OrgFile_t	* pstOrgFile = (_OrgFile_t	*)value;
	_debugtype_t * debugType = (_debugtype_t *)data;

	if(debugType->eDbgType == eHlibDbgOrgFile
		&& (debugType->File && strstr(pstOrgFile->orgFileKey.orgFile, debugType->File) == NULL))
	{
		return FALSE;
	}

	if(debugType->eDbgType != eHlibDbgMem)
	{
		debugPrint("\tORG L[%05d] F[%20s] C[%d]\n", pstOrgFile->orgFileKey.orgLine, _memdbg_RmPath(pstOrgFile->orgFileKey.orgFile), _TREE_NODES(pstOrgFile->memTree));
	}
	_TREE_Foreach(pstOrgFile->memTree, ds_PrintMemTreeInfo, data);
	return FALSE;
}

STATIC HBOOL	ds_PrintHlibTreeInfo(void* key , void* value , void* data)
{
	_HlibFile_t	* pstHlibFile = (_HlibFile_t	*)value;
	_debugtype_t * debugType = (_debugtype_t *)data;

	if(debugType->eDbgType == eHlibDbgHlibFile
		&& (debugType->File && strstr(pstHlibFile->hlibFile, debugType->File) == NULL))
	{
		return FALSE;
	}

	if(debugType->eDbgType != eHlibDbgMem)
	{
		debugPrint("\n\nHLIB %s\n", _memdbg_RmPath(pstHlibFile->hlibFile));
	}
	_TREE_Foreach(pstHlibFile->orgTree, ds_PrintOrgTreeInfo, data);

	return FALSE;
}



void	MEMDBG_HLIB_CONF(HINT32 nStartSec, HINT32 nDurationSec, HERROR(* memdbg_print)(const HCHAR *format, ...))
{
	snStartSec = nStartSec;
	snDurationSec = nDurationSec;
	debugPrint = memdbg_print;
}

HERROR	MEMDBG_HLIB_Print(const HCHAR *opt, const HCHAR *opt2, void* num)
{
	TREE_t 			* hlibFileTree = NULL;
	STATIC _debugtype_t stDebugType;


	if(opt == NULL)
	{
		return ERR_FAIL;
	}
	if(strcmp(opt, "all") == 0)
	{
		stDebugType.eDbgType = eHlibDbgALL;
	}
	else if(strcmp(opt, "hlibfile") == 0)
	{
		stDebugType.eDbgType = eHlibDbgHlibFile;
		stDebugType.File = (HCHAR *) opt2;
	}
	else if(strcmp(opt, "orgfile") == 0)
	{
		stDebugType.eDbgType = eHlibDbgOrgFile;
		stDebugType.File = (HCHAR *) opt2;
	}
	else if(strcmp(opt, "address") == 0)
	{
		stDebugType.eDbgType = eHlibDbgMem;
		stDebugType.mem = num;
		if(stDebugType.mem == 0)
			return ERR_FAIL;
	}
	else
	{
		return ERR_FAIL;
	}

	stDebugType.time = _memdbg_GetSystemTick()/1000;

	debugPrint("|============================================================================\n");
	debugPrint("|==================== HLIB MEM DEBUG start : %d duration : %d current Time %s\n",snStartSec, snDurationSec, _memdbg_AscTime (stDebugType.time, NULL, 26));
	debugPrint("|============================================================================\n\n");

	if(semaID == 0)
	{
		(void)VK_SEM_Create((unsigned long *)&semaID, "TEMP", VK_SUSPENDTYPE_FIFO);
	}
	VK_SEM_Get(semaID);
	hlibFileTree = ds_GetHlibFileTree();
	_TREE_Foreach(hlibFileTree, ds_PrintHlibTreeInfo, (void *)&stDebugType);
	VK_SEM_Release(semaID);

	return ERR_OK;
}

HERROR ds_AddMemInfo(void * memAddr, void *handle, void *orgFile, HINT32 orgLine, void *file, HINT32 line)
{
	_HlibFile_t	* pstHlibFile = NULL;
	_OrgFile_t	* pstOrgFile = NULL;
	_Mem_t		* memInfo = NULL;

	_OrgFile_t	orgFileTemp;

	TREE_t 		* hlibFileTree = NULL;

	if(semaID == 0)	(void)VK_SEM_Create((unsigned long *)&semaID, "TEMP", VK_SUSPENDTYPE_FIFO);
	VK_SEM_Get(semaID);
	hlibFileTree = ds_GetHlibFileTree();
	pstHlibFile = _TREE_Lookup(hlibFileTree, file);

	//printf("hlibFileTree[%x] hlibFileNode[%x]\n", hlibFileTree, pstHlibFile);

	if(pstHlibFile == NULL)
	{
		pstHlibFile = (_HlibFile_t *)malloc(sizeof(_HlibFile_t));
		if (pstHlibFile == NULL)
		{
			return ERR_FAIL;
		}

		memset(pstHlibFile,0,sizeof(_HlibFile_t));

		pstHlibFile->hlibFile = file;
		pstHlibFile->orgTree = NULL;
		_TREE_Insert(hlibFileTree, (void *)file, (void *)pstHlibFile);
	}

	if(pstHlibFile->orgTree == NULL)
	{
		pstHlibFile->orgTree = _TREE_NewFull(ds_compareOrgFile, NULL, NULL, ds_FreeOrgFile);
	}

	//printf("pstHlibFile->hlibFile[%s] tree[%x]\n",pstHlibFile->hlibFile, pstHlibFile->orgTree);
	//printf("Test Print[%d] %s[%d] %s[%d]\n", TEST_COUNT, orgFile, orgLine, file, line);
	orgFileTemp.orgFileKey.orgFile = orgFile;
	orgFileTemp.orgFileKey.orgLine = orgLine;
	pstOrgFile = _TREE_Lookup(pstHlibFile->orgTree, &orgFileTemp);

	if(pstOrgFile == NULL)
	{
		pstOrgFile = (_OrgFile_t *)malloc(sizeof(_OrgFile_t));
		if(pstOrgFile == NULL)
		{
			return ERR_FAIL;
		}

		memset(pstOrgFile,0,sizeof(_OrgFile_t));
		pstOrgFile->orgFileKey.orgLine = orgLine;
		pstOrgFile->orgFileKey.orgFile = orgFile;
		pstOrgFile->memTree = NULL;
		_TREE_Insert(pstHlibFile->orgTree, pstOrgFile, pstOrgFile);
	}

	if(pstOrgFile->memTree == NULL)
	{
		pstOrgFile->memTree = _TREE_NewFull(NULL, NULL, NULL, ds_FreememTree);
	}

	memInfo = (_Mem_t *)malloc(sizeof(_Mem_t));
	if (NULL != memInfo)
	{
		memset(memInfo,0,sizeof(_Mem_t));
		memInfo->mem = memAddr;
		memInfo->line = line;
		memInfo->structHandle = handle;
		memInfo->orgLine = orgLine;
		memInfo->orgFile = orgFile;
		memInfo->hlibFile = file;
		memInfo->time = _memdbg_GetSystemTick()/1000;

		_TREE_Insert(pstOrgFile->memTree, memAddr, memInfo);
	}

	VK_SEM_Release(semaID);
	return ERR_OK;
}

HERROR ds_RemoveMemInfo(void * memAddr, void *handle, void *orgFile, HINT32 orgLine, void *file, HINT32 line)
{
	_HlibFile_t		* pstHlibFile	= NULL;
	_OrgFile_t		* pstOrgFile	= NULL;
	TREE_t 			* hlibFileTree	= NULL;
	TREENode		* freeNode		= NULL;
	_OrgFile_t		orgFileTemp;

	if(semaID == 0)
	{
		(void)VK_SEM_Create((unsigned long *)&semaID, "TEMP", VK_SUSPENDTYPE_FIFO);
	}
	VK_SEM_Get(semaID);
	hlibFileTree = ds_GetHlibFileTree();
	pstHlibFile = _TREE_Lookup(hlibFileTree, file);


	assert(pstHlibFile);
	assert(pstHlibFile->orgTree);

	orgFileTemp.orgFileKey.orgFile = orgFile;
	orgFileTemp.orgFileKey.orgLine = orgLine;

	pstOrgFile = _TREE_Lookup(pstHlibFile->orgTree, &orgFileTemp);
	assert(pstOrgFile);
	assert(	pstOrgFile->memTree);

	if(_TREE_Remove(pstOrgFile->memTree,memAddr) == FALSE)		assert(1);

	freeNode = _FirstNode(pstOrgFile->memTree);
	if(freeNode == NULL)
	{
		_TREE_Destroy(pstOrgFile->memTree);
		pstOrgFile->memTree = NULL;
		_TREE_Remove(pstHlibFile->orgTree, pstOrgFile);
	}

	freeNode = _FirstNode(pstHlibFile->orgTree);
	if(freeNode == NULL)
	{
		_TREE_Destroy(pstHlibFile->orgTree);
		pstHlibFile->orgTree = NULL;
		_TREE_Remove(hlibFileTree, pstHlibFile->hlibFile);
	}
	VK_SEM_Release(semaID);
	return ERR_OK;
}

void *	MEMDBG_HLIB_Malloc (HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	void * memAddr = MEMDBG_Malloc(size, 0, file, line);
	ds_AddMemInfo(memAddr, handle, (void *)orgFile, orgLine, (void *)file, line);
	return memAddr;
}

void	MEMDBG_HLIB_Free (void *mem, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	ds_RemoveMemInfo(mem, handle, (void *)orgFile, orgLine, (void *)file, line);
	MEMDBG_Free(mem, file, line);
}

void *	MEMDBG_HLIB_Calloc (HUINT32 nmemb, HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	void * memAddr = MEMDBG_Calloc(nmemb, size, 0, file, line);
	ds_AddMemInfo(memAddr, handle, (void *)orgFile, orgLine, (void *)file, line);
	return memAddr;
}

void *	MEMDBG_HLIB_Realloc (void *mem, HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	void * memAddr = MEMDBG_Realloc(mem, size, 0, file, line);
	ds_RemoveMemInfo(mem, handle, (void *)orgFile, orgLine, (void *)file, line);
	ds_AddMemInfo(memAddr, handle, (void *)orgFile, orgLine, (void *)file, line);
	return memAddr;
}

HCHAR *	MEMDBG_HLIB_StrDup (const HCHAR *str, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	void * memAddr = MEMDBG_StrDup(str, 0, file, line);
	ds_AddMemInfo(memAddr, handle, (void *)orgFile, orgLine, (void *)file, line);
	return memAddr;
}

void *	MEMDBG_HLIB_MemDup (const void *mem, HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	void * memAddr = MEMDBG_MemDup(mem, size, 0, file, line);
	ds_AddMemInfo(memAddr, handle, (void *)orgFile, orgLine, (void *)file, line);
	return memAddr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

STATIC INLINE HINT32	_DefaultKeyCompareFunc	(const void* a , const void* b , void *userdata)
{
	(void)userdata;
	return (HINT32)a - (HINT32)b;
}

STATIC TREENode* _NodeRotateLeft (TREENode *node)
{
  TREENode *right;
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

STATIC TREENode* _NodeRotateRight (TREENode *node)
{
  TREENode *left;
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

STATIC INLINE TREENode *_NodePrevious (TREENode *node)
{
	TREENode *tmp;

	tmp = node->left;

	if (node->left_child)
	while (tmp->right_child)
	  tmp = tmp->right;

	return tmp;
}

STATIC TREENode* _NodeNew(TREE_t *tree, void* key , void* value)
{
	TREENode *node;

	if (tree->freed)
	{
		node = tree->freed;
		tree->freed = node->left;
	}
	else
	{
		node = (TREENode*)malloc(sizeof(TREENode));
		memset(node,0,sizeof(TREENode));
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

STATIC void		_NodeDestroy (TREE_t *tree, TREENode *node)
{
	if (tree->node_cache)
	{
		node->left  = tree->freed;
		tree->freed = node;
	}
	else
	{
		free(node);
	}
}

STATIC TREENode* _NodeBalance (TREENode *node)
{
  if (node->balance < -1)
  {
  	if (node->left->balance > 0)
		node->left = _NodeRotateLeft (node->left);
    node = _NodeRotateRight (node);
  }
  else if (node->balance > 1)
  {
  	if (node->right->balance < 0)
		node->right = _NodeRotateRight (node->right);
	node = _NodeRotateLeft (node);
  }
  return node;
}

STATIC INLINE TREENode* _FirstNode(TREE_t *tree)
{
	TREENode *tmp;

	if(!tree->root)
		return NULL;

	tmp = tree->root;
	while(tmp->left_child)
		tmp = tmp->left;

	return tmp;
}


STATIC INLINE TREENode* _NodeNext(TREENode *node)
{
	TREENode *tmp;
	tmp = node->right;

	if(node->right_child)
		while(tmp->left_child)
			tmp = tmp->left;

	return tmp;
}

STATIC void _InsertInternal(TREE_t *tree , void* key , void* value , HBOOL replace)
{
	TREENode *node;
	TREENode *path[MAX_TREE_HEIGHT];
	int idx;

	if(tree == NULL)
		return ;

	if (!tree->root)
	{
		tree->root = _NodeNew (tree, key,value);
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
				assert(idx < MAX_TREE_HEIGHT);
				path[idx++] = node;
				node = node->left;
			}
			else
			{
				TREENode *child = _NodeNew (tree, key, value);

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
				assert(idx < MAX_TREE_HEIGHT);
				path[idx++] = node;
				node = node->right;
			}
			else
			{
				TREENode *child = _NodeNew (tree, key, value);
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
		TREENode *bparent = path[--idx];
		HBOOL left_node = (bparent && node == bparent->left);
		//g_assert (!bparent || bparent->left == node || bparent->right == node);

		if (node->balance < -1 || node->balance > 1)
		{
			node = _NodeBalance (node);
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

STATIC HBOOL _RemoveInternal (TREE_t *tree , const void*  key , HBOOL steal)
{
	TREENode *node, *parent, *balance;
	TREENode *path[MAX_TREE_HEIGHT];
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
		int cmp = tree->key_compare(key, node->key, tree->key_compare_data);

		if (cmp == 0)
		break;
		else if (cmp < 0)
		{
		  if (!node->left_child)
		    return FALSE;

			assert(idx < MAX_TREE_HEIGHT);
			path[idx++] = node;
			node = node->left;
		}
		else
		{
			if (!node->right_child)
		    	return FALSE;

			assert(idx < MAX_TREE_HEIGHT);
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
	    	TREENode *tmp = _NodeNext (node);
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
			TREENode *tmp = _NodePrevious (node);
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
			TREENode *prev = node->left;
			TREENode *next = node->right;
			TREENode *nextp = node;
			int old_idx = idx + 1;
			idx++;

			/* path[idx] == parent */
			/* find the immediately next node (and its parent) */
			while (next->left_child)
			{
				assert(idx < MAX_TREE_HEIGHT - 1);
				path[++idx] = nextp = next;
				next = next->left;
			}
			assert(old_idx < MAX_TREE_HEIGHT);
			assert(idx < MAX_TREE_HEIGHT);
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
			TREENode *bparent = path[--idx];
			//g_assert (!bparent || bparent->left == balance || bparent->right == balance);
			left_node = (bparent && balance == bparent->left);

			if(balance->balance < -1 || balance->balance > 1)
			{
				balance = _NodeBalance (balance);
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
	_NodeDestroy(tree, node);
	tree->nnodes--;

	return TRUE;
}

STATIC TREENode *_FindNode (TREE_t *tree, const void* key)
{
	TREENode *node;
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

STATIC TREE_t* 	_TREE_NewFull(CompareDataFunc	key_compare_func, void*	key_compare_data,
									 DestroyNotify key_destroy_func, DestroyDataNotify value_destroy_func)
{

	TREE_t *tree;

	if(key_compare_func == NULL)
		key_compare_func = _DefaultKeyCompareFunc;

    tree = (TREE_t*)malloc(sizeof(TREE_t));
	memset(tree,0,sizeof(TREE_t));
    tree->root               = NULL;
    tree->key_compare        = key_compare_func;
    tree->key_destroy_func   = key_destroy_func;
    tree->value_destroy_func = (DestroyDataNotify)value_destroy_func;
    tree->key_compare_data   = key_compare_data;
    tree->nnodes             = 0;
	tree->node_cache         = FALSE;
	tree->freed              = NULL;

    return tree;

}

STATIC void	_TREE_Destroy(TREE_t *tree)
{
	TREENode *node;
	TREENode *next;

	if(tree == NULL)
		return ;

	node = _FirstNode (tree);

	while (node)
	{
	  next = _NodeNext (node);

	  if (tree->key_destroy_func) tree->key_destroy_func (node->key);
	  if (tree->value_destroy_func)	tree->value_destroy_func (node->value, tree->key_compare_data);

	  _NodeDestroy(tree, node);
	  node = next;
	}

	node = tree->freed;
	while (node)
	{
		next = node->left;
		free(node);
		node = next;
	}
	free(tree);
}

STATIC void	_TREE_RemoveAll (TREE_t *tree)
{
	TREENode *node;
	TREENode *next;

	if(tree == NULL)
		return ;

	node = _FirstNode (tree);

	while (node)
	{
	  next = _NodeNext (node);

	  if (tree->key_destroy_func)
	  	tree->key_destroy_func (node->key);
	  if (tree->value_destroy_func)
	  	tree->value_destroy_func (node->value, tree->key_compare_data);

	  _NodeDestroy(tree, node);

	  node = next;
	}

	tree->root = NULL;
	tree->nnodes = 0;
}

STATIC void 	_TREE_Insert(TREE_t *tree, void *key, void *value)
{
	if(tree==NULL)	return ;
	_InsertInternal(tree,key,value , FALSE);
}

STATIC HBOOL	_TREE_Remove(TREE_t *tree , void* key)
{
	if(tree == NULL) return FALSE;
	return _RemoveInternal(tree , key , FALSE);
}

STATIC void* _TREE_Lookup(TREE_t *tree , const void* key)
{
	TREENode *node;
	if(tree == NULL)
		return NULL;
	node = _FindNode(tree , key);
	return node ? node->value : NULL;
}

STATIC void	_TREE_Foreach(TREE_t *tree, TraverseFunc func, void* user_data)
{
	TREENode *node;
	if(tree == NULL)return;
	if(!tree->root)return;

	node = _FirstNode(tree);

	while(node)
	{
		if((*func)(node->key,node->value,user_data))
			break;

		node = _NodeNext(node);
	}
}

STATIC HINT32	_TREE_Height(TREE_t *tree)
{
	TREENode *node;
	HINT32 height;

	if(tree == NULL) return 0;

	if(!tree->root) return 0;

	height = 0;
	node = tree->root;

	while(1)
	{
		height += 1 + MAX(node->balance,0);
		if(!node->left_child) return height;
		node = node->left;
	}
}

STATIC	HUINT32 _TREE_NODES(TREE_t *tree)
{
	if(tree == NULL) return 0;
	return tree->nnodes;
}


