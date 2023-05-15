#include <vkernel.h>
#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <clib.h>
#include <apk.h>

#include <unistd.h>

#include <sqlite3.h>

static void	query_getter (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t	iter;
	HUINT32			count = 0;


	if (data && size)
	{
		DAPI_InitIterator(&iter, data, size, 0);
		while (DAPI_NextIterator(&iter))
			count++;

		printf(HxANSI_COLOR_YELLOW("\"%s\"") HxANSI_COLOR_GREEN(": count: %u\n"), userdata, count);

		DAPI_InitIterator(&iter, data, size, 0);
		while (DAPI_NextIterator(&iter))
		{
			printf(HxANSI_COLOR_BLUE("[%5d]------------------------------------------------------------------\n"), iter.iteration);
			if (DLIB_Print(iter.name, stdout, iter.data) < 0)
			{
				printf("\t %s[uid: "HUID_FMT", size:%u]\n", iter.name, iter.uid, iter.size);
				HLIB_LOG_Dump((HUINT8 *)iter.data, iter.size, 0, FALSE);
			}
		}

		printf(HxANSI_COLOR_YELLOW("\"%s\"") HxANSI_COLOR_GREEN(": count: %u\n"), userdata, count);
	}
	else
	{
		printf(HxANSI_COLOR_YELLOW("\"%s\"") HxANSI_COLOR_RED(": Empty result\n"), userdata);
	}
	HLIB_STD_MemFree(userdata);
}

static int	query (const char *sql)
{
	HINT32	id;

	id = DAPI_Query(sql, query_getter, HLIB_STD_StrDup(sql));
	DAPI_Wait(30000, id);

	return 0;
}

static int dump_type (FILE *fp, const char *table, const HUINT8 *bin, HUINT32 size, HINT32 depth)
{
	HINT32	i;
	HCHAR	tab[32];

	if (table == NULL)
		return -1;

	for (i = 0 ; i < 32 && i < depth ; i++)
		tab[i] = '\t';
	tab[i] = '\0';

	if (!strcmp(table, DxNAMEOF(DxGroupList_t)))
	{
		DxGroupList_t	*list = (DxGroupList_t *)bin;
		HxVector_t		*result;
		CListItem_t		*item;

		result = CLIB_DeSerialize(list->pucData);
		fprintf(fp, "%s%s:%s(size:%u, count:%u)\n", tab, table, list->szGroupName, list->nSize, result ? result->count : 0);
		if (result)
		{
			HxVECTOR_EACH(result, CListItem_t *, item, {
				fprintf(fp, "\t%suid:%8lld, lcnControl:%d, lcn:%d\n", tab, item->uid, item->bLCNControl, item->nLogicalNo);
			});
			HLIB_VECTOR_Delete(result);
		}
		return 0;
	}
	else
	{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
		const void *dst;
		HUINT32		dstSize;

		if (DLIB_HasTag(table, "BLOB")
			|| DLIB_Decode(table, 0, bin, size, &dst, &dstSize) != ERR_OK)
		{
			dst = bin;
			dstSize = size;
		}

		void *item = DLIB_NewType(table, 0, dst, dstSize);
#else
		const void *dst = NULL;
		const void *created = NULL;
		HUINT32 	dstSize = 0;
		HUINT32		createdSize = 0;

		if (!DLIB_HasTag(table, "BLOB"))
		{
			if (DLIB_CreateDecodedData(table, 0, bin, size, &created, &createdSize) == ERR_OK)
			{
				dst = created;
				dstSize = createdSize;
			}
		}

		if (NULL == dst)
		{
			dst = bin;
			dstSize = size;
		}

		void *item = DLIB_NewType(table, 0, dst, dstSize);

		if (NULL != created)
		{
			DLIB_DestroyDecodedData(table, created);
		}
#endif
		if (item)
		{
			i = DLIB_Print(table, fp, item);
			DLIB_FreeType(table, item);
			if (i >= 0)
				return 0;
		}
	}
	return -1;
}

static void dump_binary (FILE *fp, const HUINT8 *bin, HUINT32 size, HINT32 depth)
{
	HUINT8	line[16];
	HINT32	i, n;
	HINT32	len;
	HCHAR	tab[32];

	if (fp == NULL)
		fp = stdout;

	for (i = 0 ; i < 32 && i < depth ; i++)
		tab[i] = '\t';
	tab[i] = '\0';

	if (HLIB_JSON_IsJSON((const HCHAR *)bin, size))
	{
		fprintf(fp, "%s", bin);
		return;
	}

	for (len = (HINT32)size ; len > 0 ;)
	{
		if (len > 16)
			n = 16;
		else
			n = len;

		memcpy(line, bin, n);
		len -= n;
		bin += n;

		fprintf(fp, "%s", tab);
		for (i = 0 ; i < 16 ; i++)
		{
			if (i < n)
				fprintf(fp, "%02X ", line[i]);
			else
				fprintf(fp, "   ");
		}
		fprintf(fp, "\t");
		for (i = 0 ; i < 16 ; i++)
		{
			if (i < n)
			{
				if (isprint(line[i]))
					fprintf(fp, "%c", line[i]);
				else
					fprintf(fp, ".");
			}
			else
				fprintf(fp, " ");
		}
		fprintf(fp, "\n");
	}
}

static int	query_direct (const char *db_path, const char *sql)
{
	sqlite3			*db;
	sqlite3_stmt	*stmt;
	const char		*remain;
	int				i, n, c;

	if (sqlite3_open(db_path, &db) != SQLITE_OK)
	{
		fprintf(stderr, "failed to open file db:%s\n", db_path);
		return -1;
	}

	if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, &remain) != SQLITE_OK)
	{
		fprintf(stderr, "failed to prepare sql:%s, remain:%s\n", sql, remain);
		goto ERROR;
	}

	c = 0;
	n = sqlite3_column_count(stmt);
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		printf("[%5d]+++++++++++++++++++++++++++++++++++++\n", c++);
		for (i = 0 ; i < n ; i++)
		{
			switch (sqlite3_column_type(stmt, i))
			{
			case SQLITE_INTEGER:
				printf("\t%30s: %lld\n", sqlite3_column_name(stmt, i), sqlite3_column_int64(stmt, i));
				break;

			case SQLITE_BLOB:
				printf("\t%30s: BLOB\n", sqlite3_column_name(stmt, i));
				{
					const void *data = sqlite3_column_blob(stmt, i);
					int         size = sqlite3_column_bytes(stmt, i);

					if (dump_type(stdout, sqlite3_column_table_name(stmt, i), data, size, 1) != 0)
						dump_binary(stdout, data, size, 1);
				}
				break;

			case SQLITE_TEXT:
				printf("\t%30s: \'%s\'\n", sqlite3_column_name(stmt, i), sqlite3_column_text(stmt, i));
				break;

			case SQLITE_FLOAT:
			case SQLITE_NULL:
			default:
				break;
			}
		}
		printf("--------------------------------------------\n");
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
ERROR:
	sqlite3_close(db);
	return -1;
}


static int	extract (int argc, char *argv[])
{
	HINT32	sd;
	HCHAR	dest[256];

	argc--;
	argv++;
	if (argc <= 0)
		return -1;

	sd   = HLIB_RWSTREAM_Open(NULL, 0);
	while (argc--)
	{
		HxSTD_PrintToStrN(dest, 256, "%s.log", *argv);
		HLIB_RWSTREAM_Print(sd, "dama -x %s > %s", *argv, dest);
		printf(HxANSI_COLOR_YELLOW("\texec: \'%s\'\n"), HLIB_RWSTREAM_GetBuf(sd));
		system(HLIB_RWSTREAM_GetBuf(sd));
		HLIB_RWSTREAM_Rewind(sd);
		argv++;
	}
	HLIB_RWSTREAM_Close(sd);

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		goto HELP;

	VK_Init();
	HLIB_DBG_Init();
	DLIB_Init();

	if (HLIB_STD_StrCmpV(HxSTD_StrCmp, argv[1], "-h", "--help", NULL) >= 0)
	{
		goto HELP;
	}

	if (HLIB_STD_StrCmpV(HxSTD_StrCmp, argv[1], "-q", "--query", NULL) >= 0)
	{
		if (argc < 3)
			goto HELP;
		return query(argv[2]);
	}

	if (HLIB_STD_StrCmpV(HxSTD_StrCmp, argv[1], "-d", "--db", NULL) >= 0)
	{
		if (argc < 4)
			goto HELP;
		return query_direct(argv[2], argv[3]);
	}

	if (HLIB_STD_StrCmpV(HxSTD_StrCmp, argv[1], "-x", "--extract", NULL) >= 0)
	{
		if (argc < 3)
			goto HELP;

		argc--;
		argv++;
	}
	return extract(argc, argv);

HELP:
	printf(
		"USAGE: %s [OPTION] [FILE]\n"
		"       -h    help\n"
		"       -q    query\n"
		"       -d    query with db: (ex: $damatool -d foo.db \"SELECT * FROM foo\")\n"
		"       -x    extract log\n"
		, *argv
	);
	return 0;
}

