/*************************************************************
 * @file		hxudb.h
 * @date		2012/03/13
 * @author		someone
 * @brief		HLIB UserDB API

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HXUDB_H_
#define	_HXUDB_H_


typedef	struct	_HxUDBInfo*		HxUDBHandle;

#define		UDB_NOTUSE_VERSION			0xFFFFFFFF
/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */
extern	HxUDBHandle		HLIB_UDB_CreateInstance(HCHAR *filePath, HCHAR *fileName, HBOOL *pbNewFile, HCHAR *backupPath, HUINT32 ulVersion);
extern	void			HLIB_UDB_DestroyInstance(HxUDBHandle handle, HBOOL bSync);
extern	void			HLIB_UDB_Sync(HxUDBHandle handle);
extern	void			HLIB_UDB_Remove(HxUDBHandle handle, const HCHAR *key);
extern	void			HLIB_UDB_RemoveAll(HxUDBHandle handle);
extern	HERROR			HLIB_UDB_SetStartNode(HxUDBHandle handle, const HCHAR *root_key);
extern	HERROR			HLIB_UDB_ClearStartNode(HxUDBHandle handle);
extern	HINT32			HLIB_UDB_GetKeyCount(HxUDBHandle handle, const HCHAR *root_key);
extern	HxList_t*		HLIB_UDB_CreateKeyInfos(HxUDBHandle handle, const HCHAR *root_key, HINT32 *pnKeyCount);
extern	void			HLIB_UDB_DestroyKeyInfos(HxUDBHandle handle, HxList_t *subList);
extern	HxList_t*		HLIB_UDB_GetKeyInfos(HxUDBHandle handle, const HCHAR *root_key, HINT32 *pnKeyCount);
extern	void			HLIB_UDB_SetValue(HxUDBHandle handle, const HCHAR *key, HUINT32 ulValue);
extern	HERROR			HLIB_UDB_GetValue(HxUDBHandle handle, const HCHAR *key, HUINT32 *pulValue);
extern	void			HLIB_UDB_SetText(HxUDBHandle handle, const HCHAR *key, HCHAR *text);
extern	HERROR			HLIB_UDB_GetText(HxUDBHandle handle, const HCHAR *key, HCHAR *text, HINT32 n);
extern	void			HLIB_UDB_SetBinary(HxUDBHandle handle, const HCHAR *key, const void *obj, HINT32 size);
extern	HERROR			HLIB_UDB_GetBinary(HxUDBHandle handle, const HCHAR *key, void *obj, HINT32 *pnSize);


#endif /* _HXSOCKET_H_ */

