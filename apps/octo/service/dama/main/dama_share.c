


typedef enum
{
	DAMA_SHARE_TYPE_SET,
	DAMA_SHARE_TYPE_QUERY,
	DAMA_SHARE_TYPE_SYNC,
	DAMA_SHARE_TYPE_PUSH,

	DAMA_SHARE_TYPE_MAX
} dama_share_type_e;

typedef struct
{
	HCHAR		name[DAMA_OBJECTNAME_LEN];
	HUINT32		total;	// total share
	struct
	{
		HUINT32	total;
		HUINT32	each[DAMA_SHARE_TYPE_MAX];
	} recent;
} dama_share_table_t;

typedef struct
{
	HxTREE_t		*map;
	HUINT32			cycle;
	HUINT32			firstTick;
	HUINT32			lastTick;
} dama_share_t;

dama_share_t *	DAMA_SHARE_New (HUINT32 cycle)
{
	dama_share_t	*share;
	HxDestroyNotify	fn;

	share = (dama_share_t *)HLIB_STD_MemCalloc(sizeof(dama_share_t));
	if (share)
	{
		fn = (HxDestroyNotify)HLIB_STD_MemFree;
		share->map = HLIB_TREE_NewFull((HxCompareDataFunc)HLIB_STD_StrCaseCmp, NULL, NULL, fn);
		share->firstTick = share->lastTick = HLIB_STD_GetSystemTick();
		share->cycle = cycle;
		HLIB_TREE_NodeCache(share->map, TRUE);
	}
	return share;
}

void		DAMA_SHARE_Delete(dama_share_t *share)
{
	if (share)
	{
		HLIB_TREE_Destroy(share->map);
	}
}

void		DAMA_SHARE_AddInfo (dama_share_t *share, const HCHAR *name, dama_share_type_e type, HUINT32 tick)
{
	dama_share_table_t	*table;

	table = (dama_share_table_t *)HLIB_TREE_Lookup(share->map, (const void *)name);
	if (table == NULL)
	{
		table = (dama_share_table_t *)HLIB_STD_MemCalloc(sizeof(dama_share_table_t));
		if (table == NULL)
			return;
		HLIB_STD_StrNCpySafe(table->name, name, DAMA_OBJECTNAME_LEN);
		HLIB_TREE_Insert(share->map, table->name, table);
	}
	table->total += tick;
	table->recent.total += tick;
	table->recent.each[type] += tick;
}

void		DAMA_SHARE_Dump (dama_share_t *share, FILE *fp)
{
	HUINT32	tick;
	HUINT32	elapsed;
	void *	iter;
	HBOOL	printable;
	dama_share_table_t	*table;

	if (share == NULL || share->cycle == 0)
		return;

	tick = HLIB_STD_GetSystemTick();
	elapsed = tick - share->lastTick;
	if (elapsed < share->cycle)
		return;

	if (HLIB_TREE_Nnodes(share->map) == 0)
		return;

	share->lastTick = tick;

	for (iter = HLIB_TREE_Iterator(share->map) ; iter ; iter = HLIB_TREE_NextIterator(iter))
	{
		table = (dama_share_table_t *)HLIB_TREE_IteratorValue(iter);
		if (table->recent.total)
			break;
	}

	printable = iter ? TRUE : FALSE;

	if (printable)
	{
		fprintf(fp,
			"\t[01;34m DaMa resource share [00;39;49m \n"
			"\t+----------------------+-----+--------------------------------+\n"
			"\t|                      |total|recent ( set  query sync  push )|\n"
			"\t+----------------------+-----+--------------------------------+\n"
		);
	}
	for (iter = HLIB_TREE_Iterator(share->map) ; iter ; iter = HLIB_TREE_NextIterator(iter))
	{
		table = (dama_share_table_t *)HLIB_TREE_IteratorValue(iter);
		if (table->total == 0)
			continue;

		if (printable)
		{
			fprintf(fp,
				"\t| %20s | %3d%%| %3d%%  (%3d%%  %3d%%  %3d%%  %3d%% )|\n"
				, table->name
				, table->total * 100 / (tick - share->firstTick)
				, table->recent.total * 100 / elapsed
				, table->recent.total ? table->recent.each[0] * 100 / table->recent.total : 0
				, table->recent.total ? table->recent.each[1] * 100 / table->recent.total : 0
				, table->recent.total ? table->recent.each[2] * 100 / table->recent.total : 0
				, table->recent.total ? table->recent.each[3] * 100 / table->recent.total : 0
			);
		}
		table->recent.total = 0;
		memset(table->recent.each, 0, sizeof(HUINT32) * DAMA_SHARE_TYPE_MAX);
	}
	if (printable)
	{
		fprintf(fp,
			"\t+----------------------+-----+--------------------------------+\n"
		);
	}
}


