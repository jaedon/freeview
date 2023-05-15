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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "oplfilemgr.h"
#include "NativeHContentManager.h"
#include "macros.h"

#ifdef OIPF

/*****************************************************************************
 * HContentManager
 ******************************************************************************/
ContentOperateListener 		g_pOperationEventListener   = NULL;
ContentBrowseListener		g_pBrowseEventListener		= NULL;
ContentBrowseListener		g_pSearchEventListener		= NULL;
ContentExtMonitorListener  	g_pExtMonitorEventListener  = NULL;
ContentScanSurveyListener	g_pScanSurveyEventListener  = NULL;

/*****************************************************************************
 * FileManager Callback (FileOperation)
 *****************************************************************************
*/
void 			native_ContentManager_OperationEventListener(int id, int state)
{
	ENTRY;
	if(NULL != g_pOperationEventListener)
	{
		g_pOperationEventListener(id, state);
	}
}

OOIFReturnCode 	native_ContentManager_RegOperationEventListener(ContentOperateListener listener)
{
	ENTRY;
	g_pOperationEventListener = listener;
	OplFileManager::SetListener((OplFileManagerCallback)native_ContentManager_OperationEventListener);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_ContentManager_UnRegOperationEventListener()
{
	ENTRY;
	g_pOperationEventListener = NULL;
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * FileManager Callback (ScanSurvey Monitoring)
 *****************************************************************************
*/
void native_ContentManager_ScanSurveyEventListener(int state, void *pszTargetSource, int ulRequestId)
{
	if(NULL != g_pScanSurveyEventListener)
	{
		g_pScanSurveyEventListener(state, pszTargetSource, ulRequestId);
	}
}
OOIFReturnCode native_ContentManager_RegScanSurveyEventListener(ContentScanSurveyListener listener)
{
	ENTRY;
	g_pScanSurveyEventListener = listener;
	OplFileManager::SetScanSurveryListener((OplFileScanSurveyCallback)native_ContentManager_ScanSurveyEventListener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ContentManager_UnRegScanSurveyEventListener()
{
	ENTRY;
	g_pScanSurveyEventListener = NULL;
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * FileManager Callback (FileExtension Monitoring)
 *****************************************************************************
*/

void native_ContentManager_ExtMonitorEventListener(int state, void *pszTargetSource)
{
	ENTRY;
	if(NULL != g_pExtMonitorEventListener)
	{
		g_pExtMonitorEventListener(state, pszTargetSource);
	}
}

OOIFReturnCode native_ContentManager_RegExtMonitorEventListener(ContentExtMonitorListener listener)
{
	ENTRY;
	g_pExtMonitorEventListener = listener;
	OplFileManager::SetExtMonitorListener((OplFileExtensionMonitorCallback)native_ContentManager_ExtMonitorEventListener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ContentManager_UnRegExtMonitorEventListener()
{
	ENTRY;
	g_pExtMonitorEventListener = NULL;
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * FileBrowser Callback
 *****************************************************************************
*/
void 			native_ContentManager_BrowseEventListener(int nEvent, unsigned int requestId, int nSize, BrowserContentHandle *contentList)
{
	ENTRY;
	if(NULL != g_pBrowseEventListener)
	{
		g_pBrowseEventListener(nEvent, requestId, nSize, contentList);
	}
}

OOIFReturnCode 	native_ContentManager_RegBrowseEventListener(ContentBrowseListener listener)
{
	ENTRY;
	g_pBrowseEventListener = listener;
	OplFileBrowser::SetListener(native_ContentManager_BrowseEventListener);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_ContentManager_UnRegBrowseEventListener()
{
	ENTRY;
	g_pBrowseEventListener = NULL;
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * FileSearch Callback
 *****************************************************************************
*/
static void	native_ContentManager_SearchEventListener(int nEvent, unsigned int requestId, int nSize, BrowserContentHandle *contentList)
{
	ENTRY;
	if(NULL != g_pSearchEventListener)
	{
		g_pSearchEventListener(nEvent, requestId, nSize, contentList);
	}
}

OOIFReturnCode 	native_ContentManager_RegSearchEventListener(ContentBrowseListener listener)
{
	ENTRY;
	g_pSearchEventListener = listener;
	OplFileSearch::SetListener(native_ContentManager_SearchEventListener);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_ContentManager_UnRegSearchEventListener()
{
	ENTRY;
	g_pSearchEventListener = NULL;
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * Browse
 *****************************************************************************
*/
OOIFReturnCode	native_ContentManager_ReScan(const char *szScanPath, bool bRecursive, char *szFileExt, OOIFBoolean *retval)
{
	*retval = (OOIFNumber)OplFileManager::Rescan(szScanPath, bRecursive, szFileExt);
	OOIF_LOG_DEBUG("ReScan true/false = %d \n", *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentManager_BrowseContent(const char *szBrowsePath, int nBrowsingType, int nReqItemCount,
																 char *szFileExt, int nSortType, const char *szRootPath, OOIFNumber *requestId)
{
	ENTRY;

	unsigned int tmp = 0;

	tmp = OplFileBrowser::BrowseContent(szBrowsePath, nBrowsingType, nReqItemCount, szFileExt, nSortType, szRootPath);

	 *requestId = (OOIFNumber)tmp;

	return OOIF_RETURN_OK;
}

OOIFNumber 	native_ContentManager_GetBrowseState()
{
	ENTRY;
	return (OOIFNumber)OplFileBrowser::GetState();
}

OOIFReturnCode native_ContentManager_ExtensionMonitor(const char *pszExt, OOIFBoolean *retval)
{
	ENTRY;
	*retval = (OOIFBoolean)OplFileManager::ExtenstionMonitor(pszExt);
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * Search
 *****************************************************************************
*/
OOIFReturnCode 	native_ContentManager_SearchContent(const char *szSearchPath, const char *szCategory,
												const char *szKeyword, int nReqItemCount, int nSortType, OOIFNumber *requestId)
{
	ENTRY;
	*requestId = (OOIFNumber)OplFileSearch::SearchContent(szSearchPath, szCategory, szKeyword, nReqItemCount, nSortType );
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * Operation
 *****************************************************************************
*/
OOIFReturnCode	native_ContentManager_StartAutoScan(unsigned int ulCycle, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OplFileManager::StartAutoScan(ulCycle);
	OOIF_LOG_DEBUG("StartAutoScan ticket = %d \n", *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentManager_StopAutoScan( OOIFBoolean *retval )
{
	*retval = (OOIFBoolean)OplFileManager::StopAutoScan( );
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentManager_GetOperationState(OOIFNumber ticket, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFNumber)OplFileManager::GetState(ticket);
	OOIF_LOG_DEBUG("GetOperationState ticket = %d, result = %d \n", ticket, *retval);
	return OOIF_RETURN_OK;
}


OOIFReturnCode	native_ContentManager_AutoCopy(char *from, char *to, OOIFBoolean bRecursive, char *ext, OOIFNumber *retval)
{
	*retval = (OOIFNumber)OplFileManager::Copy(from, to, bRecursive, ext);
	OOIF_LOG_DEBUG("COPY from [%s] to [%s], ticket = %d \n", from, to, *retval);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_ContentManager_CopyContent(char *from, char *to, OOIFBoolean bRecursive, char *ext, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFNumber)OplFileManager::Copy(from, to, bRecursive, ext);
	OOIF_LOG_DEBUG("COPY from [%s] to [%s], ticket = %d \n", from, to, *retval);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_ContentManager_MoveContent(char *from, char *to, OOIFBoolean bRecursive, char *ext, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFNumber)OplFileManager::Move(from, to, bRecursive, ext);
	OOIF_LOG_DEBUG("MOVE from [%s] to [%s], ticket = %d \n", from, to, *retval);
	return OOIF_RETURN_OK;

}
OOIFReturnCode 	native_ContentManager_DeleteContent(char *src, OOIFBoolean bRecursive, char *ext, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFNumber)OplFileManager::Remove(src, bRecursive, ext);
	OOIF_LOG_DEBUG("DEL path [%s], ticket = %d \n", src, *retval);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_ContentManager_RenameContent(char *from, char *to, char *ext, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFNumber)OplFileManager::Rename(from, to);
	OOIF_LOG_DEBUG("RENAME from [%s] to [%s], ticket = %d \n", from, to, *retval);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_ContentManager_Makedir(char *name, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFNumber)OplFileManager::Mkdir(name);
	OOIF_LOG_DEBUG("MKDIR path [%s], ticket = %d \n", name, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentManager_Removedir(char *name, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFNumber)OplFileManager::Rmdir(name);
	OOIF_LOG_DEBUG("RMDIR path [%s] ticket = %d \n", name, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentManager_GetSize(const char *path, const char *ext, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFNumber)OplFileManager::GetSize( path, ext );
	OOIF_LOG_DEBUG("GetSize path [%s] ext [%s] size = %d \n", path, ext, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode	native_ContentManager_HasFile(char *name, OOIFBoolean *retval)
{
	ENTRY;
	*retval = (OOIFBoolean)HasFile(name);
	OOIF_LOG_DEBUG("HasFile[%s] result = %d \n", name, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentManager_IsExist(char *name, OOIFBoolean *retval)
{
	ENTRY;
	*retval = (OOIFBoolean)IsExist(name);
	OOIF_LOG_DEBUG("IsExist[%s] result = %d \n", name, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentManager_IsScanning(char *name, OOIFBoolean bOnlyCheck, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFNumber)OplFileManager::IsScanning(name, bOnlyCheck);
	OOIF_LOG_DEBUG("isScanning[%s] result = %d \n", name, *retval);
	return OOIF_RETURN_OK;
}


OOIFReturnCode	native_ContentManager_StopOperation(OOIFNumber ticket, OOIFNumber *retval)
{
	ENTRY;
	*retval = (OOIFBoolean)OplFileManager::Stop(ticket);
	OOIF_LOG_DEBUG("STOP ticket = %d, result = %d \n", ticket, *retval);
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * HContentManagerOperationToken
 *****************************************************************************
*/
void * native_ContentMgr_operToken_createHanle(void)
{
	ENTRY;
	OplFileOperToken *pOperToken = new OplFileOperToken;

	return (void *)pOperToken;
}

OOIFReturnCode 	native_ContentMgr_OperToken_init(void * identifier, const char *extFilter, OOIFNumber *retval)
{
	ENTRY;
	if(identifier)
	{
		OplFileOperToken *pOperToken =  static_cast<OplFileOperToken *>(identifier);
		*retval = pOperToken->init( extFilter );
	}
	else
	{
		return  OOIF_RETURN_VALUE_UNDEFINED;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentMgr_OperToken_addPath(void * identifier, const char *path, OOIFNumber *retval)
{
	ENTRY;
	if(identifier)
	{
		OplFileOperToken *pOperToken =  static_cast<OplFileOperToken *>(identifier);
		*retval = pOperToken->addPath(path);
	}
	else
	{
		return  OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentMgr_OperToken_removePath(void * identifier, const char *path, OOIFNumber *retval)
{
	ENTRY;
	if(identifier)
	{
		OplFileOperToken *pOperToken =  static_cast<OplFileOperToken *>(identifier);
		*retval = pOperToken->removePath(path);
	}
	else
	{
		return  OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentMgr_OperToken_copy(void * identifier, const char *path, OOIFNumber *retval)
{
	ENTRY;
	if(identifier)
	{
		OplFileOperToken *pOperToken =  static_cast<OplFileOperToken *>(identifier);
		*retval = pOperToken->copy(path);
	}
	else
	{
		return  OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentMgr_OperToken_move(void * identifier, const char *path, OOIFNumber *retval)
{
	ENTRY;
	if(identifier)
	{
		OplFileOperToken *pOperToken =  static_cast<OplFileOperToken *>(identifier);
		*retval = pOperToken->move(path);
	}
	else
	{
		return  OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentMgr_OperToken_remove(void * identifier, OOIFNumber *retval)
{
	ENTRY;
	if(identifier)
	{
		OplFileOperToken *pOperToken =  static_cast<OplFileOperToken *>(identifier);
		*retval = pOperToken->remove();
	}
	else
	{
		return  OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_ContentMgr_OperToken_cancel(void * identifier, OOIFNumber *retval)
{
	ENTRY;
	if(identifier)
	{
		OplFileOperToken *pOperToken =  static_cast<OplFileOperToken *>(identifier);
		*retval = pOperToken->cancel();
	}
	else
	{
		return  OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}
/*****************************************************************************
 * HContent
 *****************************************************************************
*/
OOIFReturnCode 	native_Content_GetType(BrowserContentHandle identifier, OOIFNumber *retval )
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetContentType();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}
OOIFReturnCode 	native_Content_GetFileType(BrowserContentHandle identifier, OOIFNumber *retval )
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetFileType();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}
OOIFReturnCode 	native_Content_GetFullpath(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetFullPath();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}

	return eRet;
}
OOIFReturnCode 	native_Content_GetHasSubitem(BrowserContentHandle identifier, OOIFBoolean *retval )
{
	ENTRY;

	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetHasSubItem() ? TRUE : FALSE;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode 	native_Content_GetSize(BrowserContentHandle identifier, OOIFNumber *retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetSize();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode 	native_Content_GetDate(BrowserContentHandle identifier,const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetDate();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode 	native_Content_GetDuration(BrowserContentHandle identifier, OOIFNumber *retval )
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetDuration();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}

	return eRet;
}

OOIFReturnCode 	native_Content_GetBitRate(BrowserContentHandle identifier, OOIFNumber *retval )
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetBitRate();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}

	return eRet;
}

OOIFReturnCode 	native_Content_GetSampleRate(BrowserContentHandle identifier, OOIFNumber *retval )
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetSampleRate();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}

	return eRet;
}

OOIFReturnCode 	native_Content_GetLastViewTime(BrowserContentHandle identifier, OOIFNumber *retval )
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetLastViewTime();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}

	return eRet;
}
OOIFReturnCode 	native_Content_GetLargeThumbnailUrl(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetLargeThumbnailUrl();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}

	return eRet;
}
OOIFReturnCode 	native_Content_GetThumbnailUrl(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetThumbnailUrl();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}

	return eRet;
}
OOIFReturnCode 	native_Content_GetTitle(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetTitle();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode 	native_Content_GetGenre(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetGenre();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode 	native_Content_GetResolution(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetResolution();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}

	return eRet;
}
OOIFReturnCode 	native_Content_GetThumbResolution(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetThumbResolution();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode 	native_Content_GetPictureDate(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetPictureDate();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode 	native_Content_GetAlbum(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetAlbum();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}

	return eRet;
}
OOIFReturnCode 	native_Content_GetArtist(BrowserContentHandle identifier, const char **retval)
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetArtist();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}

	return eRet;
}

OOIFReturnCode	native_Content_GetStorageCategory(BrowserContentHandle identifier, OOIFNumber *retval )
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetStorageCategory();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode 	native_Content_GetWatched(BrowserContentHandle identifier, OOIFNumber *retval )
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(identifier);
		*retval = content->GetWatched();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}

	return eRet;
}
OOIFReturnCode 	native_Content_ReleaseHandle(BrowserContentHandle id )
{
	ENTRY;
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(id)
	{
		OplBrowsedContent *content =  static_cast<OplBrowsedContent *>(id);

		if ( NULL != content )
		{
			delete content;
			content = NULL;
		}
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}


#endif // OIPF
