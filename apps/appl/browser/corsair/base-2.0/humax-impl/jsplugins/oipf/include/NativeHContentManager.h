/*************************************************************************************************************
	File 		: NativeHContentManager.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/24
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/


#ifndef __NATIVE_HCONTENT_MANAGER__
#define __NATIVE_HCONTENT_MANAGER__

#include "NativeOOIFTypes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef OIPF

typedef enum{
	NATIVE_FILEBRS_STATE_START			=0,
	NATIVE_FILEBRS_STATE_ADD,
	NATIVE_FILEBRS_STATE_COMPLETE,
	NATIVE_FILEBRW_STATE_ERROR,
} eNativeFileBrowserState;

typedef enum {
	NATIVE_FILEMGR_STATE_IDLE			= 0,

	NATIVE_FILEMGR_STATE_COPYING,
	NATIVE_FILEMGR_STATE_COPY_COMPLETE,
	NATIVE_FILEMGR_STATE_COPY_ERROR,

	NATIVE_FILEMGR_STATE_REMOVING,
	NATIVE_FILEMGR_STATE_REMOVE_COMPLETE,
	NATIVE_FILEMGR_STATE_REMOVE_ERROR,

	NATIVE_FILEMGR_STATE_MOVING,
	NATIVE_FILEMGR_STATE_MOVE_COMPLETE,
	NATIVE_FILEMGR_STATE_MOVE_ERROR,

	NATIVE_FILEMGR_STATE_RENAMING,
	NATIVE_FILEMGR_STATE_RENAME_COMPLETE,
	NATIVE_FILEMGR_STATE_RENAME_ERROR,

	NATIVE_FILEMGR_STATE_MAKING_DIR,
	NATIVE_FILEMGR_STATE_MAKING_DIR_COMPLETE,
	NATIVE_FILEMGR_STATE_MAKING_DIR_ERROR
} eNativeFileManagerState;

/*****************************************************************************
 * HContentBrowser
 *****************************************************************************
*/
typedef void *BrowserContentHandle;
typedef void (*ContentOperateListener)(int ticket, int result);
typedef void (*ContentExtMonitorListener)(int event, void *pszTargetSource);
typedef void (*ContentScanSurveyListener)(int event, void *pszPath, int requestId);
typedef void (*ContentBrowseListener)(int event, unsigned int requestId, int size, BrowserContentHandle *data);

void			native_ContentManager_OperationEventListener(int id, int state);
void 			native_ContentManager_BrowseEventListener(int nEvent, unsigned int requestId, int nSize, BrowserContentHandle *contentList);
void 			native_ContentManager_ExtMonitorEventListener(int state, void *pszTargetSource);
void 			native_ContentManager_ScanSurveyEventListener(int state, void *pszPath, int requestId);

OOIFReturnCode  native_ContentManager_RegScanSurveyEventListener(ContentScanSurveyListener listener);
OOIFReturnCode 	native_ContentManager_RegOperationEventListener(ContentOperateListener listener);
OOIFReturnCode 	native_ContentManager_RegBrowseEventListener(ContentBrowseListener listener);
OOIFReturnCode 	native_ContentManager_RegSearchEventListener(ContentBrowseListener listener);
OOIFReturnCode 	native_ContentManager_RegExtMonitorEventListener(ContentExtMonitorListener listener);

OOIFReturnCode  native_ContentManager_UnRegScanSurveyEventListener();
OOIFReturnCode 	native_ContentManager_UnRegOperationEventListener();
OOIFReturnCode 	native_ContentManager_UnRegBrowseEventListener();
OOIFReturnCode 	native_ContentManager_UnRegSearchEventListener();
OOIFReturnCode 	native_ContentManager_UnRegExtMonitorEventListener();

OOIFReturnCode	native_ContentManager_ReScan(const char *szScanPath, bool bRecursive, char *szFileExt, OOIFBoolean *retval);
OOIFReturnCode	native_ContentManager_StartAutoScan(unsigned int ulCycle, OOIFNumber *retval);
OOIFReturnCode 	native_ContentManager_StopAutoScan( OOIFBoolean *retval );

OOIFReturnCode	native_ContentManager_AutoCopy(char *from, char *to, OOIFBoolean bRecursive, char *ext, OOIFNumber *retval);
OOIFReturnCode 	native_ContentManager_CopyContent(char *from, char *to, OOIFBoolean bRecursive, char *ext,OOIFNumber *retval);
OOIFReturnCode 	native_ContentManager_MoveContent(char *from, char *to, OOIFBoolean bRecursive, char *ext, OOIFNumber *retval);
OOIFReturnCode 	native_ContentManager_DeleteContent(char *src, OOIFBoolean bRecursive, char *ext, OOIFNumber *retval);
OOIFReturnCode 	native_ContentManager_RenameContent(char *from, char *to, char *ext, OOIFNumber *retval);
OOIFReturnCode 	native_ContentManager_Makedir(char *name, OOIFNumber *retval);
OOIFReturnCode 	native_ContentManager_Removedir(char *name, OOIFNumber *retval);
OOIFReturnCode 	native_ContentManager_GetSize(const char *path, const char *ext, OOIFNumber *retval);

OOIFReturnCode 	native_ContentManager_IsExist(char *name, OOIFBoolean *retval);
OOIFReturnCode	native_ContentManager_HasFile(char *name, OOIFBoolean *retval);
OOIFReturnCode 	native_ContentManager_IsScanning(char *name, OOIFBoolean bOnlyCheck, OOIFNumber *retval);

OOIFReturnCode	native_ContentManager_StopOperation(OOIFNumber ticket, OOIFNumber *retval);
OOIFReturnCode 	native_ContentManager_GetOperationState(OOIFNumber ticket, OOIFNumber *retval);
OOIFNumber 		native_ContentManager_GetBrowseState();
OOIFReturnCode 	native_ContentManager_ExtensionMonitor(const char *szExt, OOIFBoolean *retval);

OOIFReturnCode 	native_ContentManager_BrowseContent(const char *szBrowsePath, int nBrowsingType, int nReqItemCount,
																 char *szFileExt, int nSortType, const char *szRootPath,
																 OOIFNumber *requestId);
OOIFReturnCode 	native_ContentManager_SearchContent(const char *szSearchPath, const char *szCategory,
								const char *szKeyword, int nReqItemCount, int nSortTypea, OOIFNumber *requestId);
/* Methods */

/*****************************************************************************
 * HContentManagerOperationToken
 *****************************************************************************
*/
void * native_ContentMgr_operToken_createHanle(void);
OOIFReturnCode 	native_ContentMgr_OperToken_init(void * identifier, const char *extFilter, OOIFNumber *retval);
OOIFReturnCode 	native_ContentMgr_OperToken_addPath(void * identifier, const char *path, OOIFNumber *retval);
OOIFReturnCode 	native_ContentMgr_OperToken_removePath(void * identifier, const char *path, OOIFNumber *retval);
OOIFReturnCode 	native_ContentMgr_OperToken_copy(void * identifier, const char *path, OOIFNumber *retval);
OOIFReturnCode 	native_ContentMgr_OperToken_move(void * identifier, const char *path, OOIFNumber *retval);
OOIFReturnCode 	native_ContentMgr_OperToken_remove(void * identifier, OOIFNumber *retval);
OOIFReturnCode 	native_ContentMgr_OperToken_cancel(void * identifier, OOIFNumber *retval);


/*****************************************************************************
 * HContent
 *****************************************************************************
*/


OOIFReturnCode native_Content_GetType(BrowserContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_Content_GetFileType(BrowserContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_Content_GetFullpath(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetHasSubitem(BrowserContentHandle identifier, OOIFBoolean *retval );
OOIFReturnCode 	native_Content_GetSize(BrowserContentHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_Content_GetDate(BrowserContentHandle identifier,const char **retval);
OOIFReturnCode native_Content_GetDuration(BrowserContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_Content_GetBitRate(BrowserContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_Content_GetSampleRate(BrowserContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_Content_GetLastViewTime(BrowserContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_Content_GetLargeThumbnailUrl(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetThumbnailUrl(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetResolution(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetThumbResolution(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetPictureDate(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetAlbum(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetArtist(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetWatched(BrowserContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_Content_ReleaseHandle(BrowserContentHandle id );
OOIFReturnCode native_Content_GetTitle(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetGenre(BrowserContentHandle identifier, const char **retval);
OOIFReturnCode native_Content_GetStorageCategory(BrowserContentHandle identifier, OOIFNumber *retval );


#endif /* OIPF */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HCONTENT_BROWSER__ */

