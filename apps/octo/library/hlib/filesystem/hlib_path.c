
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "_hlib_path.h"
#include "_hlib_stdlib.h"
#include "_hlib_rwstream.h"

#define	HxPATH_DELIM_C	'/'
#define	HxPATH_DELIM_S	"/"
#define	HxPATH_DELIM	HxPATH_DELIM_C
#define	HLIB_PATH_STACK_MAX	32

/**
 * Return the current working directory of the calling process.
 */
HCHAR *		HLIB_PATH_GetCWD (HCHAR *cwd, HUINT32 n)
{
	if (!(cwd && n))
		return NULL;
#if defined(OS_LINUX)
	return getcwd(cwd, n);
#else
	return NULL;
#endif
}

/**
 * Return a nomalized absolutized version of the pathname 'path'.
 */
HCHAR * 	HLIB_PATH_AbsPath (const HCHAR *path, HCHAR *abspath, HUINT32 n)
{
#if defined(OS_LINUX)
#ifndef	PATH_MAX
#define	PATH_MAX	HxPATH_MAX
#endif
	HCHAR			temp[PATH_MAX];
	const HCHAR		*ret = path;
	
	ret = realpath(path, temp);
	if (ret == NULL)
	{
		return NULL;
	}
	return HLIB_STD_StrNCpySafe(abspath, ret, n);
#else
	return NULL;
#endif
}

HCHAR *		HLIB_PATH_BasePath (const HCHAR *path)
{
	HCHAR	*ret;

	if ((ret = HxSTD_StrRChar(path, '/')))
		*ret = 0;
	ret = HxSTD_StrRChar(path, '/');

	return ret;
}

/**
 * Return the base name of pathname 'path'.
 * '/foo/bar/' returns 'bar'
 */
HCHAR *		HLIB_PATH_BaseName (const HCHAR *path, HCHAR *basename, HUINT32 n)
{
	HCHAR		*ret;
	HCHAR		token[HxPATH_MAX];
	HUINT32		i;

	if (basename == NULL || n == 0)
		return NULL;
	ret = NULL;
	for (i = 0 ; *path != '\0' ; path++)
	{
		if (*path == HxPATH_DELIM)
		{
			token[i] = '\0';
			i = 0;
			ret = HLIB_STD_StrNCpySafe(basename, token, n);
		}
		else
		{
			token[i++] = *path;
		}
	}

	if (i > 0)
	{
		token[i] = '\0';
		ret = HLIB_STD_StrNCpySafe(basename, token, n);
	}
	if (HxSTD_StrEmpty(ret))
		return NULL;
	return ret;
}

/**
 * Return the directory name of pathname 'path'.
 */
HCHAR *		HLIB_PATH_DirName (const HCHAR *path, HCHAR *dirname, HUINT32 n)
{
	HCHAR	*ret;

	if (!(dirname && n))
		return NULL;

	HLIB_STD_StrNCpySafe(dirname, path, n);

#if defined(OS_LINUX)
	{
		struct stat fstat;
		if (stat(path, &fstat) == 0 && S_ISDIR(fstat.st_mode))
		{
			return HLIB_STD_StrRStrip(dirname, HxPATH_DELIM_S);
		}
	}
#endif
	ret = strrchr(dirname, HxPATH_DELIM);
	if (ret)
	{
		*ret = '\0';
	}
	return dirname;
}

/**
 * Return the file name of pathname 'path'.
 */
HCHAR *		HLIB_PATH_FileName (const HCHAR *path)
{
	HCHAR *ret = strrchr(path, HxPATH_DELIM);
	if (ret)
		ret++;
	return ret;
}

HCHAR *		HLIB_PATH_Extension (const HCHAR *path)
{
	HCHAR *ret = strrchr(path, '.');
	if (ret)
		ret++;

	return ret;
}

HCHAR *		HLIB_PATH_SplitExt (const HCHAR *path, HCHAR *buf, HUINT32 n)
{
	HCHAR	*r;

	if (!(buf && n))
		return NULL;

	HLIB_STD_StrNCpySafe(buf, path, n);

	r = strrchr(buf, '.');
	if (r)
		*r = '\0';
	return buf;
}

/**
 * Join one or more path components intelligently.
 * join("/foo/bar/../", "/wow/ABC", "file.txt") returns "/foo/wow/ABC/file.txt"
 */
HCHAR * 	HLIB_PATH_Join (const HCHAR *path1, const HCHAR *path2, HCHAR *joined, HUINT32 n)
{
	if (!(path1 && path2 && joined && n))
		return NULL;

	HxSTD_PrintToStrN(joined, n, "%s%c%s", path1, HxPATH_DELIM, path2);
	return HLIB_PATH_Normalize(joined);
}

HCHAR *		HLIB_PATH_JoinPathes (const HCHAR *path, ...)
{
	va_list	ap;
	HINT32	sd;
	HCHAR	*in;
	HCHAR	*out;

	if (path == NULL)
		return NULL;

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	if (!sd)
		return NULL;

	HLIB_RWSTREAM_PutString(sd, path);
	va_start(ap, path);
	for (;;)
	{
		path = va_arg(ap, const HCHAR *);
		if (path == NULL)
			break;
		HLIB_RWSTREAM_Print(sd, "%c%s", HxPATH_DELIM, path);
	}
	va_end(ap);
	in  = HLIB_RWSTREAM_CloseWithoutBuf(sd);
	out = HLIB_PATH_Normalize(in);
	if (out == NULL)
	{
		HLIB_MEM_Free(in);
		return NULL;
	}
	return out;
}

/**
 * Normalize a pathname. This collapses redundant separators and up-level references
 * so that A//B, A/B/, A/./B and A/foo/../B all become A/B.
 */
HCHAR *		HLIB_PATH_Normalize (HCHAR *path)
{
	HCHAR	*freed;
	HCHAR	*buf;
	HCHAR	*token, *save;
	HUINT32	n;
	HCHAR	temp[HxPATH_MAX];
	HCHAR	*stack[HLIB_PATH_STACK_MAX];
	HINT32	i, top = 0;
	HCHAR	*d;

	if (path == NULL)
		return NULL;

	freed = NULL;
	if ((n = strlen(path)) < HxPATH_MAX)
		buf = temp;
	else
	{
		freed = buf = (HCHAR *)HLIB_MEM_Malloc(n + 1);
		if (buf == NULL)
			return NULL;
	}

	memcpy(buf, path, n + 1);

	token = strtok_r(buf, HxPATH_DELIM_S, &save);
	while (token)
	{
		if (strcmp(token, ".") == 0)
			;// do nothing
		else if (strcmp(token, "..") == 0)
		{
			if (top-- == 0)	// stack underflow
			{
				if (freed) HLIB_MEM_Free(freed);
				return NULL;
			}
		}
		else
		{
			if (top == HLIB_PATH_STACK_MAX)	// stack overflow
			{
				if (freed) HLIB_MEM_Free(freed);
				return NULL;
			}
			stack[top++] = token;
		}
		token = strtok_r(NULL, HxPATH_DELIM_S, &save);
	}
	d = path;
	if (*d == HxPATH_DELIM)
		*d++;
	for (i = 0 ; i < top ; i++)
	{
		HCHAR *s = stack[i];
		if (i != 0)
			*d++ = HxPATH_DELIM;
		while (*s != '\0')
			*d++ = *s++;
	}
	*d = '\0';
	if (freed) HLIB_MEM_Free(freed);
	return path;
}

void	HLIB_PATH_MemFree(void *p)
{
	HLIB_MEM_Free(p);
}


