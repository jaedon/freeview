#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <apk.h>
#include <unistd.h>


typedef struct
{
	HCHAR			*name;
	DxRecListData_t	*data;
	HxVector_t		*family;
} rec_t;

typedef struct
{
	HINT32			serial;
	HxTREE_t		*recs;
	HCHAR			buf[512];
	HCHAR			cmd[2048];
} recgen_t;

static void	free_rec (rec_t *r)
{
	if (r->name)
		HLIB_STD_MemFree(r->name);
	if (r->data)
		DLIB_FreeType(DxNAMEOF(DxRecListData_t), r->data);
	if (r->family)
		HLIB_VECTOR_Delete(r->family);
	HLIB_STD_MemFree(r);
}

static HBOOL	copy(const char *src, const char *dst, size_t limit)
{
	FILE	*in, *out;
	char	*buf;
	size_t	len;
	struct stat	stat;

	if (strcmp(src, dst) == 0)
		return FALSE;

	if (lstat(src, &stat) < 0)
		return FALSE;

	if (!S_ISREG(stat.st_mode))
		return FALSE;


	if ((in = fopen(src, "rb")) == NULL)
		return FALSE;

	if ((out = fopen(dst, "wb")) == NULL)
	{
		fclose(in);
		return FALSE;
	}

	buf = (char*)HLIB_STD_MemAlloc(4096);
	if (buf == NULL)
	{
		fclose(in);
		fclose(out);
		return FALSE;
	}

	while ((len = fread(buf, sizeof(char), 4096, in)) != 0)
	{
		if (len > limit)
			break;
		limit -= len;

		if (fwrite(buf, sizeof(char), len, out) == 0)
		{
			fclose(in);
			fclose(out);
			HLIB_STD_MemFree(buf);
			unlink(dst);
			return FALSE;
		}
		fflush(out);
	}

	fclose(in);
	fclose(out);
	HLIB_STD_MemFree(buf);
	return TRUE;
}


static rec_t *	get_random_rec (recgen_t *gen)
{
	HINT32	n;
	void	*iter;
	rec_t	*rec;

	n = HLIB_TREE_Nnodes(gen->recs);
	if (n == 0)
		return NULL;
	n = HLIB_MATH_Random() % n;
	iter = HLIB_TREE_Iterator(gen->recs);
	while (n--)
	{
		iter = HLIB_TREE_NextIterator(iter);
	}

	if (iter == NULL)
		return get_random_rec(gen);
	rec = (rec_t *)HLIB_TREE_IteratorValue(iter);

	if (rec->data == NULL || rec->family == NULL)
		return get_random_rec(gen);
	return rec;
}

static HINT32	get_serialno (const HCHAR *path)
{
	HCHAR	buf[64];
	HINT32	i;

	if (!HLIB_STD_StrStartWith(path, "g."))
		return 0;
	path += 2;

	i = 0;
	while (isdigit(*path))
	{
		buf[i++] = *path++;
	}
	buf[i] = '\0';
	return HLIB_STD_StrToINT32(buf);
}

static int	process_input (recgen_t *gen, const HCHAR *inpath)
{
	HxDIR_t	*dir;
	rec_t	*rec;
	HCHAR	*name;
	HCHAR	*path;
	HINT32	no;
	HCHAR	buf[512];

	dir = HLIB_DIR_Open(inpath);
	if (!dir)
		return -1;
	do
	{
		path = (HCHAR *)HLIB_DIR_GetName(dir);
		name = HLIB_PATH_SplitExt(path, buf, 512);
		if (!HxSTD_StrEmpty(name) && strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
		{
			no   = get_serialno(path);
			if (no > gen->serial)
				gen->serial = no;

			rec  = (rec_t *)HLIB_TREE_Lookup(gen->recs, name);
			if (rec == NULL)
			{
				rec = (rec_t *)HLIB_STD_MemCalloc(sizeof(rec_t));
				rec->name = HLIB_STD_StrDup(name);
				HLIB_TREE_Insert(gen->recs, rec->name, rec);
			}

			if (HLIB_STD_StrEndWith(path, ".hjm"))
			{
				HxJSON_t json;
				HCHAR	*fullpath = HLIB_PATH_JoinPathes(HLIB_DIR_GetPath(dir), path, NULL);
				json = HLIB_JSON_DecodeFile(fullpath);
				HLIB_STD_MemFree(fullpath);
				if (json)
				{
					rec->data = DxRECLISTDATA_Decode(json, NULL);
					HLIB_JSON_Delete(json);
				}
			}
			else
			{
				if (rec->family == NULL)
					rec->family = HLIB_VECTOR_NewEasy(0, HLIB_STD_MemFree_CB, NULL);
				HLIB_VECTOR_Add(rec->family, HLIB_PATH_JoinPathes(HLIB_DIR_GetPath(dir), path, NULL));
			}
		}
	} while (HLIB_DIR_Next(dir) == ERR_OK);

	HLIB_DIR_Close(dir);
	return 0;
}

static int	generate (recgen_t *gen, HINT32 no, rec_t *rec)
{
	DxRecListData_t	*data;
	HCHAR	*tmp;
	FILE	*fp;

	data = (DxRecListData_t *)HLIB_STD_MemDup(rec->data, sizeof(DxRecListData_t) + sizeof(DxRecList_ExtEvt_t) * rec->data->ulExtEvtCnt);
	if (data->ulExtEvtCnt)
		data->pstExtEvts = (DxRecList_ExtEvt_t *)(data + 1);

	data->ulRecStartTime = data->ulRecStartTime + no;
	HxSTD_PrintToStrN(data->szName, DxRECLIST_NAME_LEN, "g.%05d.%s", no, rec->data->szName);
	HLIB_STD_StrNCpySafe(data->stMeta.szFileName, data->szName, DxRECLIST_PATH);
	HLIB_STD_StrNCpySafe(data->stMeta.aucDisplayEvtName, data->szName, DxRECLIST_NAME_LEN);
	tmp = strrchr(data->szUrl, ';');
	if (tmp)
	{
		tmp++;
		HxSTD_PrintToStrN(tmp, DxRECLIST_URL - strlen(data->szUrl), "%x", data->ulRecStartTime);
	}

	tmp = DxRECLISTDATA_Encode(data, NULL);
	HLIB_STD_MemFree(data);
	if (tmp == NULL)
	{
		printf(HxANSI_COLOR_RED("\tFailed to encode(%s)")"\n", rec->name);
		return -1;
	}

	HxSTD_PrintToStrN(gen->buf, 512, "%s%s.hjm", data->stMeta.szPath, data->stMeta.szFileName);
	fp = fopen(gen->buf, "w");
	if (fp == NULL)
	{
		HLIB_STD_MemFree(tmp);
		printf(HxANSI_COLOR_RED("\tFailed to open file(%s)")"\n", gen->buf);
		return -1;
	}
	fprintf(fp, "%s", tmp);
	HLIB_STD_MemFree(tmp);
	fclose(fp);

	printf(HxANSI_COLOR_GREEN("\tgenerated...")" %s\n", gen->buf);

	if (rec->family)
	{
		HCHAR	*file;
		HCHAR	ext[64];
		HCHAR	*ret;

		HxVECTOR_EACH(rec->family, HCHAR *, tmp, {
			HLIB_STD_StrNCpySafe(gen->buf, tmp, 512);
			file = HLIB_PATH_FileName(gen->buf);
			ret  = strrchr(file, '.');
			if (ret)
				HLIB_STD_StrNCpySafe(ext, ret, 64);
			else
				ext[0] = '\0';
			HxSTD_PrintToStrN(file, 512 - strlen(gen->buf), "%s%s", data->szName, ext);
			HxSTD_PrintToStrN(gen->cmd, 2048, "cp -rf \"%s\" \"%s\"", tmp, gen->buf);

			if (HLIB_STD_StrEndWith(ext, "ts"))
			{
				if (!copy(tmp, gen->buf, 1024 * 1024 * 20))
					system(gen->cmd);
			}
			else system(gen->cmd);
			//symlink(tmp, gen->buf);
			printf(HxANSI_COLOR_GREEN("\t         ...")" %s\n", gen->buf);
		});
	}
	return 0;
}

static int recgen (int argc, char *argv[])
{
	recgen_t	recgen;
	HUINT32		n;

	printf(HxANSI_COLOR_CYAN("Recording generator")"\n");

	if (argc < 3)
	{
		printf(HxANSI_COLOR_YELLOW("USAGE:\n"));
		printf(HxANSI_COLOR_YELLOW("\t$%s [gen] [recording path, ...]\n"), *argv);
		printf(HxANSI_COLOR_YELLOW("\tex. $%s 10 /mnt/hd2/Recordings\n"), *argv);
		return -1;
	}
	argc--;
	argv++;

	n = HLIB_STD_StrToINT32(*argv++);
	argc--;

	memset(&recgen, 0, sizeof(recgen_t));
	recgen.recs = HLIB_TREE_NewFull((HxCompareDataFunc)HxSTD_StrCmp, NULL, NULL, (HxDestroyNotify)free_rec);
	for (; argc-- ; argv++)
	{
		process_input(&recgen, *argv);
	}

	if (n == 0)
		n = HLIB_TREE_Nnodes(recgen.recs);

	while (n--)
	{
		rec_t *rec = get_random_rec(&recgen);
		if (rec == NULL)
			continue;

		if (generate(&recgen, recgen.serial + 1, rec) == 0)
			recgen.serial++;
	}
	HLIB_TREE_Destroy(recgen.recs);
	return 0;
}

int main(int argc, char *argv[])
{
	VK_Init();
	HLIB_DBG_Init();
	DLIB_Init();

	return recgen(argc, argv);
}

