/*************************************************************************************************************
	File 		: HContentManager.cpp
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

#ifdef OIPF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "HContentManager.h"
#include "hlib.h"

using namespace Ooif;

#define RELEASE_HANDLE(x)		\
		if(NULL != x){			\
			HLIB_STD_MemFree(x);	\
			x = NULL;			\
		}						\

typedef enum
{
	MAX_CATEGORY_LEN = 64,
	MAX_KEYWORD_LEN = 128,
	MAX_FILEEXT_LEN = 256,
	MAX_PATH_LEN	= 512,
}ContentBrowser_e;

/*******************************************************************/
/**************    		HContentManager   **************************/
/*******************************************************************/
HContentManager *HContentManager::m_pHContentManager;
HContentManager::HContentManager() : EventTarget()
{
	ENTRY;
	classType = CLASS_CONTENTMANAGER;
	m_pHContentManager = this;

	native_ContentManager_RegBrowseEventListener(HContentManager::cbBrowseEventHandler);
	native_ContentManager_RegSearchEventListener(HContentManager::cbSearchEventHandler);
	native_ContentManager_RegOperationEventListener(HContentManager::cbOperationEventHandler);
	native_ContentManager_RegExtMonitorEventListener(HContentManager::cbExtMonitorEventHandler);
	native_ContentManager_RegScanSurveyEventListener(HContentManager::cbScanSurveyEventHandler);
}

HContentManager::~HContentManager()
{
	ENTRY;
	m_pHContentManager = NULL;
	native_ContentManager_UnRegOperationEventListener();
	native_ContentManager_UnRegBrowseEventListener();
	native_ContentManager_UnRegSearchEventListener();
	native_ContentManager_UnRegExtMonitorEventListener();
	native_ContentManager_UnRegScanSurveyEventListener();
}

int HContentManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(autoCopy, 				obj, "sss", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(copyContent, 			obj, "osss", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(deleteContent, 		obj, "oss", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(moveContent, 			obj, "osss", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(makeDir, 				obj, "s", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeDir, 			obj, "s", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getSize, 				obj, "ss", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopFileOperation, 	obj, "o", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(browseContent, 		obj, "snnsns", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(searchContent, 		obj, "sssn", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getOperationState, 	obj, "n", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(reScan, 				obj, "sbs", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(startAutoScan,			obj, "n", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopAutoScan, 			obj, "", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isExist,				obj, "s", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(hasFile,				obj, "s", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isScanning,			obj, "s", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(requestScanNotify, 	obj, "s",		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(extensionMonitor,		obj, "s", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createOperationToken,	obj, "s", 		JSP_GET_VALUE_CACHE);

	/* EventHandler*/
	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	GET_NUMBER("browseState", 			native_ContentManager_GetBrowseState(), 	JSP_GET_VALUE);

	INTRINSIC_EVENT_GET(ContentBrowse);
	INTRINSIC_EVENT_GET(ContentSearch);
	INTRINSIC_EVENT_GET(ContentOperate);
	INTRINSIC_EVENT_GET(ContentMonitor);
	INTRINSIC_EVENT_GET(ContentScanSurvey);
	return JSP_GET_NOTFOUND;
}

int HContentManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(ContentBrowse);
	INTRINSIC_EVENT(ContentSearch);
	INTRINSIC_EVENT(ContentOperate);
	INTRINSIC_EVENT(ContentMonitor);
	INTRINSIC_EVENT(ContentScanSurvey);
	return JSP_PUT_NOTFOUND;
}

int HContentManager::extensionMonitor(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval;

	char szExt[MAX_FILEEXT_LEN]	= {0};
	memset(szExt, '\0', sizeof(szExt));
	strncpy( szExt, 	STRING_VALUE(argv[0]), sizeof(szExt)-1);

	native_ContentManager_ExtensionMonitor(szExt, &retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
int	HContentManager::browseContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(6);
	OOIFNumber ticket = 0;

	char szBrowsePath[MAX_PATH_LEN] 	= {0};
	int  nBrowseType				  	= 0;
	int  nReqItemCount				  	= 0;
	char szFileExt[MAX_FILEEXT_LEN] 	= {0};
	int  nSortType 				 	= 0;
	char szRootPath[MAX_PATH_LEN] 		= {0};

	memset(szBrowsePath, '\0', sizeof(szBrowsePath));
	memset(szFileExt, '\0', sizeof(szFileExt));
	memset(szRootPath, '\0', sizeof(szRootPath));

	strncpy( szBrowsePath, 	STRING_VALUE(argv[0]), sizeof(szBrowsePath)-1);
	nBrowseType 		= 	NUMBER_VALUE(argv[1]);
	nReqItemCount 		= 	NUMBER_VALUE(argv[2]);
	strncpy( szFileExt, 	STRING_VALUE(argv[3]), sizeof(szFileExt)-1);
	nSortType 			= 	NUMBER_VALUE(argv[4]);
	strncpy( szRootPath, 	STRING_VALUE(argv[5]), sizeof(szRootPath)-1);

	native_ContentManager_BrowseContent(szBrowsePath, nBrowseType, nReqItemCount, szFileExt, nSortType, szRootPath, &ticket);

	RETURN_NUMBER(ticket, JSP_CALL_VALUE);
}

int	HContentManager::searchContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(5);
	OOIFNumber ticket = 0;

	char szSearchPath[MAX_PATH_LEN] 	= {0};
	char szCategory[MAX_CATEGORY_LEN] 	= {0};
	char szKeyword[MAX_KEYWORD_LEN] 	= {0};
	int  nReqItemCount				  	= 0;
	int  nSortType 				 	= 0;

	//memset(szSearchPath, '\0', sizeof(szSearchPath));

	strncpy( szSearchPath, 	STRING_VALUE(argv[0]), sizeof(szSearchPath)-1 );
	strncpy( szCategory, 	STRING_VALUE(argv[1]), sizeof(szCategory)-1 );
	strncpy( szKeyword, 	STRING_VALUE(argv[2]), sizeof(szKeyword)-1 );
	nReqItemCount 			= 	NUMBER_VALUE(argv[3]);
	nSortType 				= 	NUMBER_VALUE(argv[4]);

	if ( 0 == nReqItemCount )
		nReqItemCount = 10;

	native_ContentManager_SearchContent(szSearchPath, szCategory, szKeyword, nReqItemCount, nSortType, &ticket);

	RETURN_NUMBER(ticket, JSP_CALL_VALUE);
}
int	HContentManager::reScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(3);
	OOIFBoolean retval;

	char szScanRootPath[MAX_PATH_LEN] = {0};
	char szFileExt[MAX_FILEEXT_LEN] 	= {0};
	int  bRecursive = 0;

	memset(szScanRootPath, '\0', sizeof(szScanRootPath));
	memset(szFileExt, '\0', sizeof(szFileExt));

	strncpy( szScanRootPath, 	STRING_VALUE(argv[0]), sizeof(szScanRootPath)-1);
	bRecursive = NUMBER_VALUE(argv[1]);
	strncpy( szFileExt, 		STRING_VALUE(argv[2]), sizeof(szFileExt)-1);

	native_ContentManager_ReScan(szScanRootPath, bRecursive, szFileExt, &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::startAutoScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval;

	native_ContentManager_StartAutoScan(NUMBER_VALUE(argv[0]), &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::stopAutoScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval;

	native_ContentManager_StopAutoScan(&retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

/* DAMA ���� * �� ����� �� ������ autocopy �ʿ����  */
int	HContentManager::autoCopy(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(4);
	OOIFNumber retval;

	OOIFBoolean bRecursive = false;
	char szSrc[MAX_PATH_LEN] = {0};
	char szDst[MAX_PATH_LEN] = {0};
	char szExt[MAX_PATH_LEN] = {0};

	memset(szSrc, '\0', sizeof(szSrc));
	memset(szDst, '\0', sizeof(szDst));
	memset(szExt, '\0', sizeof(szExt));

	strncpy( szSrc,	STRING_VALUE(argv[0]), sizeof(szSrc)-1);
	strncpy( szDst,	STRING_VALUE(argv[1]), sizeof(szDst)-1);
	bRecursive = BOOLEAN_VALUE(argv[2]);
	strncpy( szExt,	STRING_VALUE(argv[3]), sizeof(szExt)-1);

	/* TODO: Need param list */
	native_ContentManager_AutoCopy(szSrc, szDst, bRecursive, szExt, &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::copyContentArray(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(4);
	OOIFNumber retval = 0;

	ContentCollection *contentCollection = OBJECT_VALUE(argv[0], ContentCollection);
	int count = contentCollection->getSize();
	count = count;


	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::copyContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = 0;
	if ( 4 == argc )
	{
	char szSrcFile[MAX_PATH_LEN] 	= {0};
	char szDstFile[MAX_PATH_LEN] 	= {0};
	char szExt[MAX_PATH_LEN] 		= {0};
	OOIFBoolean bRecursive = false;

	memset(szSrcFile, '\0', sizeof(szSrcFile));
	memset(szDstFile, '\0', sizeof(szDstFile));
	memset(szExt, '\0', sizeof(szExt));

		//argv[0].type = JSP_TYPE_STRING;
	strncpy( szSrcFile,	STRING_VALUE(argv[0]), sizeof(szSrcFile)-1);
	strncpy( szDstFile,	STRING_VALUE(argv[1]), sizeof(szDstFile)-1);
	bRecursive = BOOLEAN_VALUE(argv[2]);
	strncpy( szExt,		STRING_VALUE(argv[3]), sizeof(szExt)-1);

	native_ContentManager_CopyContent(szSrcFile, szDstFile, bRecursive, szExt, &retval);
	}
	else if ( 2 == argc )
	{
		HContentManagerOperationToken *pToken = OBJECT_VALUE(argv[0], HContentManagerOperationToken);
		const char *pPath = STRING_VALUE(argv[1]);
		if ( pToken )
			native_ContentMgr_OperToken_copy( pToken->getIdentifier(), pPath, &retval );
		else
		{
			OOIF_LOG_DEBUG( "[HCONTENT_MGR] pToken(copy) is NULL!!!!\n" );
		}
	}
	else
	{
		CHECK_ARGUMENT_COUNT(4);
	}
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::deleteContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = 0;

	if ( 3 == argc )
	{
	OOIFBoolean bRecursive = false;
	char szDelPath[MAX_PATH_LEN] 	= {0};
	char szExt[MAX_PATH_LEN] 		= {0};

	memset(szDelPath, '\0', sizeof(szDelPath));
	memset(szExt, '\0', sizeof(szExt));

	strncpy( szDelPath,	STRING_VALUE(argv[0]), sizeof(szDelPath)-1);
	bRecursive = BOOLEAN_VALUE(argv[1]);
	strncpy( szExt,		STRING_VALUE(argv[2]), sizeof(szExt)-1);

	native_ContentManager_DeleteContent(szDelPath, bRecursive, szExt, &retval);
	}
	else if ( 1 == argc )
	{
		HContentManagerOperationToken *pToken = OBJECT_VALUE(argv[0], HContentManagerOperationToken);
		if ( pToken )
			native_ContentMgr_OperToken_remove( pToken->getIdentifier(), &retval );
		else
		{
			OOIF_LOG_DEBUG( "[HCONTENT_MGR] pToken(move) is NULL!!!!\n" );
		}
	}
	else
	{
		CHECK_ARGUMENT_COUNT(3);
	}
	RETURN_NUMBER(retval, JSP_CALL_VALUE);

}

int	HContentManager::moveContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = 0;

	if ( 4 == argc )
	{
	char szSrcFile[MAX_PATH_LEN] 	= {0};
	char szDstFile[MAX_PATH_LEN] 	= {0};
	char szExt[MAX_PATH_LEN] 		= {0};
	OOIFBoolean bRecursive = false;

	memset(szSrcFile, '\0', sizeof(szSrcFile));
	memset(szDstFile, '\0', sizeof(szDstFile));
	memset(szExt, '\0', sizeof(szExt));

	strncpy( szSrcFile,	STRING_VALUE(argv[0]), sizeof(szSrcFile)-1);
	strncpy( szDstFile,	STRING_VALUE(argv[1]), sizeof(szDstFile)-1);
	bRecursive		=	BOOLEAN_VALUE(argv[2]);
	strncpy( szExt,		STRING_VALUE(argv[3]), sizeof(szExt)-1);

	native_ContentManager_MoveContent(szSrcFile, szDstFile, bRecursive, szExt, &retval);
	}
	else if ( 2 == argc )
	{
		HContentManagerOperationToken *pToken = OBJECT_VALUE(argv[0], HContentManagerOperationToken);
		const char *pPath = STRING_VALUE(argv[1]);
		if ( pToken )
			native_ContentMgr_OperToken_move( pToken->getIdentifier(), pPath, &retval );
		else
		{
			OOIF_LOG_DEBUG( "[HCONTENT_MGR] pToken(move) is NULL!!!!\n" );
		}
	}
	else
	{
		CHECK_ARGUMENT_COUNT(4);
	}

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::renameContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(3);
	OOIFNumber retval;

	char szSrcFile[MAX_PATH_LEN] 	= {0};
	char szDstFile[MAX_PATH_LEN] 	= {0};
	char szExt[MAX_PATH_LEN] 		= {0};

	memset(szSrcFile, '\0', sizeof(szSrcFile));
	memset(szDstFile, '\0', sizeof(szDstFile));
	memset(szExt, '\0', sizeof(szExt));

	strncpy( szSrcFile,	STRING_VALUE(argv[0]), sizeof(szSrcFile)-1);
	strncpy( szDstFile,	STRING_VALUE(argv[1]), sizeof(szDstFile)-1);
	strncpy( szExt,		STRING_VALUE(argv[2]), sizeof(szExt)-1);

	native_ContentManager_RenameContent(szSrcFile, szDstFile, szExt, &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);

}

int	HContentManager::hasFile(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval = TRUE;

	char szPath[MAX_PATH_LEN] = {0};
	memset(szPath, '\0', sizeof(szPath));
	strncpy( szPath,	STRING_VALUE(argv[0]), sizeof(szPath)-1);

	native_ContentManager_HasFile(szPath, &retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int HContentManager::requestScanNotify(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval = 0;

	char szPath[MAX_PATH_LEN] = {0};
	memset(szPath, '\0', sizeof(szPath));
	strncpy( szPath, STRING_VALUE(argv[0]), sizeof(szPath)-1);
	native_ContentManager_IsScanning(szPath, false, &retval); // not only currency state but also scan_complete result

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HContentManager::isScanning(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval = 0;

	char szPath[MAX_PATH_LEN] = {0};
	memset(szPath, '\0', sizeof(szPath));
	strncpy( szPath, STRING_VALUE(argv[0]), sizeof(szPath)-1);
	native_ContentManager_IsScanning(szPath, true, &retval); // only to check currency state

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}
int	HContentManager::isExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval = TRUE;

	char szPath[MAX_PATH_LEN] = {0};
	memset(szPath, '\0', sizeof(szPath));
	strncpy( szPath,	STRING_VALUE(argv[0]), sizeof(szPath)-1);

	native_ContentManager_IsExist(szPath, &retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HContentManager::makeDir(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval;

	char szMakePath[MAX_PATH_LEN] = {0};
	memset(szMakePath, '\0', sizeof(szMakePath));
	strncpy( szMakePath,	STRING_VALUE(argv[0]), sizeof(szMakePath)-1);

	native_ContentManager_Makedir(szMakePath, &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::removeDir(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval;

	char szRemovePath[MAX_PATH_LEN] = {0};
	memset(szRemovePath, '\0', sizeof(szRemovePath));
	strncpy( szRemovePath,	STRING_VALUE(argv[0]), sizeof(szRemovePath)-1);

	native_ContentManager_Removedir(szRemovePath, &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::getSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	OOIFNumber retval;

	const char *pPath = (const char *)STRING_VALUE(argv[0]);
	const char *pExt = (const char *)STRING_VALUE(argv[1]);

	native_ContentManager_GetSize( pPath, pExt, &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::stopFileOperation(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval;

	HContentManagerOperationToken *pToken = OBJECT_VALUE(argv[0], HContentManagerOperationToken);
	if ( pToken )
	{
		native_ContentMgr_OperToken_cancel( pToken->getIdentifier(), &retval );
	}
	else
	{
	int nTicket = NUMBER_VALUE(argv[0]);

	native_ContentManager_StopOperation(nTicket, &retval);
	}

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::getOperationState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval;
	int ticket =	NUMBER_VALUE(argv[0]);

	native_ContentManager_GetOperationState(ticket, &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManager::createOperationToken(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval;
	void *pHandId = NULL;
	const char *pExtFilter =	STRING_VALUE(argv[0]);
	pHandId = native_ContentMgr_operToken_createHanle();
	native_ContentMgr_OperToken_init( pHandId, pExtFilter,  &retval );
	HContentManagerOperationToken *pOperToken = new HContentManagerOperationToken(pHandId);

	RETURN_OBJECT( this_obj, pOperToken, result, JSP_GET_VALUE );
}

void HContentManager::cbScanSurveyEventHandler(int state, void * pszTargetSource, int ulRequestId)
{
	ENTRY;
	Event *e = NULL;
	jsplugin_obj *context = NULL;
	HContentManager *pContentManager = m_pHContentManager;

	if ( NULL == pContentManager )
	{
		OOIF_LOG_DEBUG("HContentManager has Null instance !!!! \n");
		return ;
	}
	context = pContentManager->getHost();
	if(context)
	{
		e = new HContentManagerScanSurveyResultEvent( context, "ContentScanSurvey", state, (char*)pszTargetSource, ulRequestId);

		if( NULL != e)
		{
			OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)", pContentManager, e);
			pContentManager->dispatchEvent(e);
		}
	}
	OOIF_LOG_DEBUG("[%s] - state(%d), detect file(%s) \n", __FUNCTION__, state, (char*)pszTargetSource);

}

void HContentManager::cbExtMonitorEventHandler(int state, void *pszTargetSource)
{
	ENTRY;
	Event *e = NULL;
	jsplugin_obj *context = NULL;
	HContentManager *pContentManager = m_pHContentManager;

	if ( NULL == pContentManager )
	{
		OOIF_LOG_DEBUG("HContentManager has Null instance !!!! \n");
		return ;
	}
	context = pContentManager->getHost();
	if(context)
	{
		e = new HContentManagerMonitorResultEvent( context, "ContentMonitor", state, (char*)pszTargetSource);

		if( NULL != e)
		{
			OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)", pContentManager, e);
			pContentManager->dispatchEvent(e);
		}
	}
	OOIF_LOG_DEBUG("[%s] - state(%d), detect file(%s) \n", __FUNCTION__, state, (char*)pszTargetSource);

}

void HContentManager::cbBrowseEventHandler( int event, unsigned int requestId, int size, BrowserContentHandle *contentList)
{
	ENTRY;
	Event *e = NULL;
	jsplugin_obj *context = NULL;
	HContentManager *pContentManager = m_pHContentManager;

	if ( NULL == pContentManager )
	{
		OOIF_LOG_DEBUG("HContentManager has Null instance !!!! \n");
		return ;
	}
	context = pContentManager->getHost();
	if(context)
	{
		switch( event )
		{
			case NATIVE_FILEBRS_STATE_START:
			case NATIVE_FILEBRS_STATE_ADD:
			case NATIVE_FILEBRS_STATE_COMPLETE:
			case NATIVE_FILEBRW_STATE_ERROR:
				e = new HContentManagerBrowseResultEvent( context, "ContentBrowse", event, requestId, size, contentList );
				break;

			default :
				return ;
		}
		if( NULL != e)
		{
			OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)", pContentManager, e);
			pContentManager->dispatchEvent(e);
		}
	}
	OOIF_LOG_DEBUG("[%s] - event(%d), size(%d) \n", __FUNCTION__, event, size);
}

void HContentManager::cbSearchEventHandler(int event, unsigned int requestId, int size, BrowserContentHandle *contentList)
{
	ENTRY;
	Event *e = NULL;
	jsplugin_obj *context = NULL;
	HContentManager *pContentManager = m_pHContentManager;

	if ( NULL == pContentManager )
	{
		OOIF_LOG_DEBUG("HContentManager has Null instance !!!! \n");
		return ;
	}
	context = pContentManager->getHost();
	if(context)
	{
		switch( event )
		{
			case NATIVE_FILEBRS_STATE_START:
			case NATIVE_FILEBRS_STATE_ADD:
			case NATIVE_FILEBRS_STATE_COMPLETE:
			case NATIVE_FILEBRW_STATE_ERROR:
				e = new HContentManagerSearchResultEvent( context, "ContentSearch", event, requestId, size, contentList );
				break;

			default :
				return ;
		}
		if( NULL != e)
		{
			OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)", pContentManager, e);
			pContentManager->dispatchEvent(e);
		}
	}
	OOIF_LOG_DEBUG("[%s] - event(%d), size(%d) \n", __FUNCTION__, event, size);
}

void HContentManager::cbOperationEventHandler(int ticket, int result)
{
	ENTRY;
	ENTRY;
	Event *e = NULL;
	jsplugin_obj *context = NULL;
	HContentManager *pContentManager = m_pHContentManager;

	if ( NULL == pContentManager )
	{
		OOIF_LOG_DEBUG("HContentManager has Null instance !!!! \n");
		return ;
	}
	context = pContentManager->getHost();
	if(context)
	{
		e = new HContentManagerOperateResultEvent( context, "ContentOperate", ticket, result);
		if( NULL != e)
		{
			OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)", pContentManager, e);
			pContentManager->dispatchEvent(e);
		}
	}

	OOIF_LOG_DEBUG("[%s] - ticket(%d), result(%d) \n", __FUNCTION__, ticket, result);
}

/*******************************************************************/
/********    		HContentManagerOperationToken   ******************/
/*******************************************************************/
HContentManagerOperationToken::HContentManagerOperationToken( void * id ) : m_identifier(id)//, WrapperObject<HContentManagerOperationToken>(id)

{
	classType = CLASS_CONTENTMGR_OPERTOKEN;
}

#if 0
HContentManagerOperationToken::HContentManagerOperationToken( const HContentManagerOperationToken &original ) : m_identifier(original.identifier), WrapperObject<HContentManagerOperationToken>(original.identifier)
{
	classType = CLASS_CONTENTMGR_OPERTOKEN;
}

#endif

HContentManagerOperationToken::~HContentManagerOperationToken()
{
}

#if 0
void HContentManagerOperationToken::releaseNative( void* id )
{
	//NATIVE_WARN_NOK(native_HNetworkAccessPointReleaseHandle(id));
}
#endif

int HContentManagerOperationToken::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(addPath, 			obj, "s", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removePath, 		obj, "s", 	JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int HContentManagerOperationToken::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

int	HContentManagerOperationToken::addPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = 0;
	CHECK_ARGUMENT_COUNT(1);
	const char *pPath =	STRING_VALUE(argv[0]);
	HContentManagerOperationToken *instance = EXTRACT_OBJECT_OR_THROW( instance, this_obj, HContentManagerOperationToken );
	native_ContentMgr_OperToken_addPath( instance->getIdentifier(), pPath, &retval );
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HContentManagerOperationToken::removePath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = 0;
	CHECK_ARGUMENT_COUNT(1);
	const char *pPath =	STRING_VALUE(argv[0]);
	HContentManagerOperationToken *instance = EXTRACT_OBJECT_OR_THROW( instance, this_obj, HContentManagerOperationToken );
	native_ContentMgr_OperToken_removePath( instance->getIdentifier(), pPath, &retval );
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

void HContentManagerOperationToken::cbEventHandler(void *identifier, int event, void * resultInfo)
{


}

/*******************************************************************/
/********    		HContentManagerOperateResultEvent   ******************/
/*******************************************************************/
HContentManagerOperateResultEvent::HContentManagerOperateResultEvent( jsplugin_obj *c, const char *eventName, int ticket, int result)
						: Event( c, eventName ), m_nTicket(ticket), m_nResult( result )
{
	ENTRY;
	OOIF_LOG_DEBUG("[%s] name(%s), ticket(%d), result(%d)\n", __FUNCTION__, eventName, ticket, result);
}
HContentManagerOperateResultEvent:: ~HContentManagerOperateResultEvent()
{
	ENTRY;

}

int HContentManagerOperateResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("ticket", 	m_nTicket,	JSP_GET_VALUE);
	GET_NUMBER("result", 	m_nResult, 	JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HContentManagerOperateResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}
int HContentManagerOperateResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	const int RETURN_PARAM_COUNT = 2;
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nTicket;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_nResult;

	return RETURN_PARAM_COUNT;
}

/*******************************************************************/
/********    		HContentManagerMonitorResultEvent   ******************/
/*******************************************************************/
HContentManagerMonitorResultEvent::HContentManagerMonitorResultEvent( jsplugin_obj *c, const char *eventName, int state, char* handleId )
						: Event( c, eventName )

{
	m_handleId = HLIB_STD_StrDup(handleId);
	m_nState = state;
	OOIF_LOG_DEBUG("[%s] name(%s), state(%d), handleId(%p)\n", __FUNCTION__, eventName, state, handleId);
}
HContentManagerMonitorResultEvent:: ~HContentManagerMonitorResultEvent()
{
	ENTRY;
	if(m_handleId)
	{
		HLIB_STD_MemFree(m_handleId);
		m_handleId = NULL;
	}
}

int HContentManagerMonitorResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
    GET_NUMBER("state", 		m_nState, 				JSP_GET_VALUE);
    GET_STRING("filepath",		m_handleId, 			JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HContentManagerMonitorResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

int HContentManagerMonitorResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	const int RETURN_PARAM_COUNT = 2;
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nState;

	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = m_handleId;

	return RETURN_PARAM_COUNT;
}
/*******************************************************************/
/********    		HContentManagerScanSurveyResultEvent   ******************/
/*******************************************************************/
HContentManagerScanSurveyResultEvent::HContentManagerScanSurveyResultEvent( jsplugin_obj *c, const char *eventName, int state, char* handleId, int requestId )
						: Event( c, eventName )

{
	if(handleId) m_handleId = HLIB_STD_StrDup(handleId);
	m_nState = state;
	m_nRequestId = requestId;
	OOIF_LOG_DEBUG("[%s] name(%s), state(%d), handleId(%p)\n", __FUNCTION__, eventName, state, handleId);
}
HContentManagerScanSurveyResultEvent:: ~HContentManagerScanSurveyResultEvent()
{
	ENTRY;
	if(m_handleId)
	{
		HLIB_STD_MemFree(m_handleId);
		m_handleId = NULL;
	}
}

int HContentManagerScanSurveyResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
    GET_NUMBER("state", 		m_nState, 				JSP_GET_VALUE);
    GET_STRING("path",			m_handleId, 			JSP_GET_VALUE);
	GET_NUMBER("requestId",		m_nRequestId, 			JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HContentManagerScanSurveyResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

int HContentManagerScanSurveyResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	const int RETURN_PARAM_COUNT = 3;
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nState;

	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = m_handleId;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_nRequestId;

	return RETURN_PARAM_COUNT;
}

/*******************************************************************/
/********    		HContentManagerBrowseResultEvent   ******************/
/*******************************************************************/
HContentManagerBrowseResultEvent::HContentManagerBrowseResultEvent( jsplugin_obj *c, const char *eventName, int state, unsigned int requestId, int size, BrowserContentHandle* handleId )
						: Event( c, eventName )
{
	m_handleId = handleId;
	m_nState = state;
	m_nTicketId = requestId;
	m_nHandleCount =  size;
	OOIF_LOG_DEBUG("HContentManagerBrowseResultEvent name(%s), state(%d), size(%d), handleId(%p)\n", eventName, state, size, handleId);
}
HContentManagerBrowseResultEvent:: ~HContentManagerBrowseResultEvent()
{
    ENTRY;
    if ( m_handleId )
    {
        HLIB_STD_MemFree( m_handleId );
    }
}

int HContentManagerBrowseResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_OBJECT("contents", obj, GetContentCollection(), JSP_GET_VALUE);
    GET_NUMBER("state", 		m_nState, 				JSP_GET_VALUE);
	GET_NUMBER("ticket",		m_nTicketId, 			JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HContentManagerBrowseResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

HContentCollection* HContentManagerBrowseResultEvent::GetContentCollection()
{
	ENTRY;
	HContentCollection *collection = new HContentCollection;
	HContent *content = NULL;
	for ( int i = 0; i< m_nHandleCount; i++ )
	{
		content = new HContent(m_handleId[i]);
		collection->addItem( content  );
	}
	return collection;
}
int HContentManagerBrowseResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	const int RETURN_PARAM_COUNT = 3;
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	HContentCollection *collection = new HContentCollection;
	HContent *content = NULL;
	for ( int i = 0; i< m_nHandleCount; i++ )
	{
		content = new HContent(m_handleId[i]);
		collection->addItem( content  );
	}
	SET_SERIALIZED_OBJECT( collection, 0 );

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_nState;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_nTicketId;

	return RETURN_PARAM_COUNT;
}

/*******************************************************************/
/********    		HContentManagerSearchResultEvent   ******************/
/*******************************************************************/
HContentManagerSearchResultEvent::HContentManagerSearchResultEvent( jsplugin_obj *c, const char *eventName, int state, unsigned int requestId, int size, BrowserContentHandle* handleId )
						: HContentManagerBrowseResultEvent( c, eventName, state, requestId, size, handleId )
{
	OOIF_LOG_DEBUG("HContentManagerSearchResultEvent name(%s), state(%d), size(%d), handleId(%p)\n", eventName, state, size, handleId);
}
int HContentManagerSearchResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	const int RETURN_PARAM_COUNT = 3;
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	HContentCollection *collection = new HContentCollection;
	HContent *content = NULL;
	for ( int i = 0; i< m_nHandleCount; i++ )
	{
		content = new HContent(m_handleId[i]);
		collection->addItem( content  );
	}
	SET_SERIALIZED_OBJECT( collection, 1 );

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_nState;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nTicketId;

	return RETURN_PARAM_COUNT;
}

/*******************************************************************/
/******************    		HContent          **********************/
/*******************************************************************/
HContent::HContent( BrowserContentHandle id) : WrapperObject<HContent>(id)
{
	ENTRY;
	classType = CLASS_CONTENT;

}
HContent::HContent(const HContent &original) : WrapperObject<HContent>(original.identifier)
{
	ENTRY;
	classType = CLASS_CONTENT;
}

HContent::~HContent()
{
	ENTRY;

}

int HContent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_NUMBER( 	"contentType", 		native_Content_GetType(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_NUMBER( 	"fileType", 		native_Content_GetFileType(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING(	"fullPath", 		native_Content_GetFullpath(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN( "hasSubitem", 		native_Content_GetHasSubitem(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_NUMBER( 	"size", 			native_Content_GetSize(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING( 	"date", 			native_Content_GetDate(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_NUMBER( 	"duration", 		native_Content_GetDuration(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_NUMBER( 	"lastViewTime", 	native_Content_GetLastViewTime(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( 	"largeThumbnailUrl", 	native_Content_GetLargeThumbnailUrl(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( 	"thumbnailUrl", 	native_Content_GetThumbnailUrl(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( 	"resolution", 		native_Content_GetResolution(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING( 	"thumbResolution", 	native_Content_GetThumbResolution(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING( 	"pictureDate", 		native_Content_GetPictureDate(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( 	"album", 			native_Content_GetAlbum(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( 	"artist", 			native_Content_GetArtist(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( 	"title", 			native_Content_GetTitle(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_NUMBER( 	"bitRate", 			native_Content_GetBitRate(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_NUMBER( 	"sampleRate", 		native_Content_GetSampleRate(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING(	"genre",			native_Content_GetGenre(identifier, &RETVAL),			JSP_GET_VALUE);
	NATIVE_GET_NUMBER( 	"watched", 			native_Content_GetWatched(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_NUMBER(  "storageCategory", 	native_Content_GetStorageCategory(identifier, &RETVAL),	JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}
int HContent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

void HContent::releaseNative( BrowserContentHandle id)
{
	ENTRY;
	NATIVE_WARN_NOK(native_Content_ReleaseHandle(id));
}

/*******************************************************************/
/******************    HContentCollection          *****************/
/*******************************************************************/

ContentCollection::ContentCollection()
{
	classType = CLASS_CONTENTCOLLECTION;

}
ContentCollection::~ContentCollection()
{
}
int ContentCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	int ret = Collection<HContent>::getter(obj, name, result);

	return	ret;
}
int ContentCollection::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return	JSP_PUT_NOTFOUND;
}

#endif // OIPF

