

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#ifndef	_HLIB_PATH_H_
#define	_HLIB_PATH_H_

#include "_hlib_types.h"

#define	HxPATH_MAX	256

/**
 * Return the current working directory of the calling process.
 */
HCHAR *		HLIB_PATH_GetCWD (HCHAR *cwd, HUINT32 n);

/**
 * Return a nomalized absolutized version of the pathname 'path'.
 * - It expends all symbolic links.
 */
HCHAR *		HLIB_PATH_AbsPath (const HCHAR *path, HCHAR *abspath, HUINT32 n);

/**
 * Return the base path of path name 'path'
 * '/a/b/c.a returns b
 */
HCHAR *		HLIB_PATH_BasePath (const HCHAR *path);

/**
 * Return the base name of pathname 'path'.
 * '/foo/bar/' returns 'bar'
 * "/foo/bar/"	returns bar
 * 	"/foo/bar"	returns bar
 *	"/foo/bar/ "	returns ' '
 *	"/foo"		returns foo
 *	"foo/"		returns foo
 *	"foo/bar/"	returns bar
 *	"foo/bar"	returns bar
 *	"foo"		returns foo
 */
HCHAR *		HLIB_PATH_BaseName (const HCHAR *path, HCHAR *basename, HUINT32 n);

/**
 * Return the directory name of pathname 'path'.
 */
HCHAR *		HLIB_PATH_DirName (const HCHAR *path, HCHAR *dirname, HUINT32 n);

/**
 * Return the file name of pathname 'path'.
 */
HCHAR *		HLIB_PATH_FileName (const HCHAR *path);

/**
 * Return the extension of pathname 'path'.
 */
HCHAR *		HLIB_PATH_Extension (const HCHAR *path);

/**
 * Return the 'root' name (without 'ext') of pathname 'path'
 */
HCHAR *		HLIB_PATH_SplitExt (const HCHAR *path, HCHAR *buf, HUINT32 n);

/**
 * Join one or more path components intelligently.
 * join("/foo/bar/../", "/wow/ABC", "file.txt") returns "/foo/wow/ABC/file.txt"
 */
HCHAR *		HLIB_PATH_Join (const HCHAR *path1, const HCHAR *path2, HCHAR *joined, HUINT32 n);
HCHAR *		HLIB_PATH_JoinPathes (const HCHAR *base, ...);

/**
 * Normalize a pathname. This collapses redundant separators and up-level references
 * so that A//B, A/B/, A/./B and A/foo/../B all become A/B.
 *
 *	"/"						- "/"
 *	"./"					- ""
 *	"foo/bar/../"			- "foo"
 *	"./foo"					- "foo"
 *	"////foo/bar///wow../"	- "/foo/bar/wow.."
 *	"./foo../bar/.."		- "foo.."
 */
HCHAR *		HLIB_PATH_Normalize (HCHAR *path);
void		HLIB_PATH_MemFree(void *p);

#endif	// _HLIB_PATH_H_


