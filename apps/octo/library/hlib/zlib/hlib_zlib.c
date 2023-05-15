/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/**
	@file     hxzlib.c
	@brief   zlib interface function file

	Description: libxml interface function file		\n
	Module: ZLIB API			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <dlfcn.h>

#include <zlib.h>
#include "_hlib_types.h"
#include "_hlib_log.h"
#include "_hlib_zlib.h"
#include "_hlib_file.h"
#include "_hlib_mem.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define	ZLIB_RETURN_IF(expr, err)	do{if(expr){printf("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	ZLIB_GOTO_IF(expr, pos)		do{if(expr){printf("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

#else
#define	ZLIB_RETURN_IF(expr, err)	do{if(expr){return (err);}}while(0)
#define	ZLIB_GOTO_IF(expr, pos)		do{if(expr){goto pos;}}while(0)
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32		ulSemId;			//	Semaphore가 필요없을것 같지만 나중을 대비해서 우선 만들어 놓음.

	void		*pvZLibSo;
	int			(*_uncompress)(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);
	int			(*_compress)(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);
	gzFile		(*_gzopen64)(const char *, const char *);
	int			(*_gzread)(gzFile file, voidp buf, unsigned len);
	int			(*_gzclose)(gzFile file);
} zlib_t;

#define	zlibSo(func)		(hlib_zlib_InitLibrary())->_##func

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

static HERROR		__zlib_Uncompress(HUINT8 *pucDest, HUINT32 *pulDestLen, HUINT8 *pucSource, HUINT32 ulSourceLen);
static HERROR		__zlib_Compress(HUINT8 *pucDest, HUINT32 *pulDestLen, HUINT8 *pucSource, HUINT32 ulSourceLen);
static HERROR 		__gz_Uncompress(HUINT8 *infilename, HUINT8 *outfilename);

static zlib_t*	hlib_zlib_InitLibrary(void)
{
	static zlib_t	*s_pstZLib = NULL;

	if (s_pstZLib == NULL)
	{
		s_pstZLib = HLIB_MEM_Calloc(sizeof(zlib_t));
		s_pstZLib->pvZLibSo = dlopen("/usr/lib/libz.so", RTLD_LAZY);
		if (s_pstZLib->pvZLibSo == NULL)
		{
			HxLOG_Critical("Can't find /usr/lib/libz.so\n");
			HxLOG_Assert(0);
			HLIB_MEM_Free(s_pstZLib);
			return NULL;
		}

		s_pstZLib->_uncompress = dlsym(s_pstZLib->pvZLibSo, "uncompress");
		s_pstZLib->_compress = dlsym(s_pstZLib->pvZLibSo, "compress");
		s_pstZLib->_gzopen64 = dlsym(s_pstZLib->pvZLibSo, "gzopen64");
		s_pstZLib->_gzread = dlsym(s_pstZLib->pvZLibSo, "gzread");
		s_pstZLib->_gzclose = dlsym(s_pstZLib->pvZLibSo, "gzclose");

		HxLOG_Assert(s_pstZLib->_uncompress && s_pstZLib->_compress);
		HxLOG_Assert(s_pstZLib->_gzopen64 && s_pstZLib->_gzread && s_pstZLib->_gzclose);
		HxLOG_Warning("libZ dynamic loading is done...\n");
	}

	return s_pstZLib;
}

HERROR		HLIB_ZLIB_Uncompress(HUINT8 *pucDest, HUINT32 *pulDestLen, HUINT8 *pucSource, HUINT32 ulSourceLen)
{
	ZLIB_RETURN_IF(pucDest == NULL || pulDestLen == NULL || pucSource == NULL, ERR_FAIL);
	ZLIB_RETURN_IF(__zlib_Uncompress(pucDest, pulDestLen, pucSource, ulSourceLen) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR		HLIB_ZLIB_Compress(HUINT8 *pucDest, HUINT32 *pulDestLen, HUINT8 *pucSource, HUINT32 ulSourceLen)
{
	ZLIB_RETURN_IF(pucDest == NULL || pulDestLen == NULL || pucSource == NULL, ERR_FAIL);
	ZLIB_RETURN_IF(__zlib_Compress(pucDest, pulDestLen, pucSource, ulSourceLen)!= ERR_OK, ERR_FAIL);
	return ERR_OK;
}

HERROR HLIB_GZ_Uncompress(HUINT8 *infilename, HUINT8 *outfilename)
{
	ZLIB_RETURN_IF(infilename == NULL || outfilename == NULL, ERR_FAIL);
	ZLIB_RETURN_IF(HLIB_FILE_Exist(infilename) != TRUE, ERR_FAIL);
	ZLIB_RETURN_IF(__gz_Uncompress(infilename, outfilename) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

#define ______LOCAL_ZLIB_FUNC______


static HERROR		__zlib_Uncompress(HUINT8 *pucDest, HUINT32 *pulDestLen, HUINT8 *pucSource, HUINT32 ulSourceLen)
{
	ZLIB_RETURN_IF(pucDest == NULL || pulDestLen == NULL || pucSource == NULL, ERR_FAIL);
	ZLIB_RETURN_IF((HERROR)zlibSo(uncompress((Bytef*)pucDest, (uLongf*)pulDestLen, (const Bytef*)pucSource, (uLong)ulSourceLen)) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

static HERROR		__zlib_Compress(HUINT8 *pucDest, HUINT32 *pulDestLen, HUINT8 *pucSource, HUINT32 ulSourceLen)
{
	ZLIB_RETURN_IF(pucDest == NULL || pulDestLen == NULL || pucSource == NULL, ERR_FAIL);
	ZLIB_RETURN_IF((HERROR)zlibSo(compress((Bytef*)pucDest, (uLongf*)pulDestLen, (const Bytef*)pucSource, (uLong)ulSourceLen)) != ERR_OK, ERR_FAIL);
	return ERR_OK;
}

#define UNCOMPRESS_BUF_SIZE (20*1024)

HERROR __gz_Uncompress(HUINT8 *infilename, HUINT8 *outfilename)
{
	HERROR	herr = ERR_FAIL;
    HUINT8 *buf = NULL;
    int		len=0;
	gzFile	file = (gzFile)NULL;
	FILE	*out = (FILE*)NULL;

	buf = (HUINT8 *)HLIB_MEM_Malloc(UNCOMPRESS_BUF_SIZE);
	ZLIB_GOTO_IF (buf == NULL, EXIT_ZLIB_EXIT);

	out = fopen(outfilename, "wb");
	ZLIB_GOTO_IF (out == NULL, EXIT_ZLIB_EXIT);
    file = zlibSo(gzopen64(infilename, "rb"));
	ZLIB_GOTO_IF(file == NULL, EXIT_ZLIB_EXIT);

    for (;;)
	{
        len = zlibSo(gzread(file, buf, UNCOMPRESS_BUF_SIZE));
        if (len <= 0) break;
        ZLIB_GOTO_IF ((int)fwrite(buf, 1, (unsigned)len, out) != len, EXIT_ZLIB_EXIT);
    }

	herr = ERR_OK;

	EXIT_ZLIB_EXIT :

	if(out)
	{
		fclose(out);
	}
	zlibSo(gzclose(file));
	if(buf != NULL) HLIB_MEM_Free(buf);
	return herr;
}


/*********************** End of File ******************************/
