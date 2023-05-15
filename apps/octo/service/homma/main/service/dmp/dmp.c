/*************************************************************************************************************
	File            : dmp.h
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED 징째AS IS징짹 AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

#define	________________Header_Files_______________________________
#include "dmp.h"
#include "homma_bus.h"

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL				bIsStart;
	DMP_EventCallback_t	event_cb;
} dmp_context_t;

#define ________________Private_Static_Variables___________________
static dmp_context_t pst_dmp = {FALSE, NULL};

#define ________________Internal_Functions_________________________
static void __cb_device( PL_DMP_DmsInfo_t *pDMSInfo, HBOOL bAdded, HBOOL bCurrent )
{
	HxLOG_Trace();

	if(pst_dmp.bIsStart && pst_dmp.event_cb != NULL)
	{
		if(bAdded)
			pst_dmp.event_cb( eDMP_ADD_DMS, (HINT32) pDMSInfo, 1);
		else
			pst_dmp.event_cb( eDMP_REMOVE_DMS, (HINT32) pDMSInfo, 1);
	}
	else
	{
		if (pst_dmp.event_cb == NULL)
			HxLOG_Debug("[%s:%d] NOT Registed dmp event callback !! \n", __FUNCTION__, __LINE__);
		if (pst_dmp.bIsStart == FALSE)
			HxLOG_Debug("[%s:%d] pst_dmp.bIsStart NOT START !! \n", __FUNCTION__, __LINE__);
	}
}

static void __cb_browse_result(HCHAR *pUdn, CO_MediaInstence_t *pList, HINT32 nCount, HINT32 nErrCode)
{
	if(nErrCode)
		PL_DMP_RefreshDms(pUdn);

	if(pst_dmp.bIsStart && pst_dmp.event_cb != NULL)
		pst_dmp.event_cb(eDMP_ADD_ITEM, 0, 0);
}

static void __cb_search_result(HCHAR *pUdn, PL_DMP_CDSResultInfo_t *pResult, CO_MediaInstence_t *pList, HINT32 nErrCode)
{
	if(pst_dmp.bIsStart && pst_dmp.event_cb != NULL)
		pst_dmp.event_cb(eDMP_SEARCH_ITEM, (HINT32)pResult, (HINT32)pList);
}
#define ________________Public_Functions___________________________
HERROR	DMP_Start(void)
{
	pst_dmp.bIsStart = FALSE;

	if( PL_DMP_IsDMPStarted() == TRUE )
	{
		pst_dmp.bIsStart = TRUE;
		return ERR_OK;
	}

	( void )PL_DMP_Init();

	( void )PL_DMP_RegisterEventCB( __cb_device, __cb_browse_result, __cb_search_result );

	pst_dmp.bIsStart = TRUE; // dmp start 하자 마자 event가 올라와 면서, flag를 true로 변경해 준다. 

	( void )PL_DMP_Start();

	( void )PL_DMP_M_Search();

	return ERR_OK;
}

HERROR	DMP_Stop(void)
{
	if( PL_DMP_IsDMPStarted()  == TRUE )
		( void )PL_DMP_DeInit();

	pst_dmp.bIsStart = FALSE;

	return ERR_OK;
}

HBOOL	DMP_IsStart(void)
{
	return  pst_dmp.bIsStart;
}

HERROR 	DMP_Browse(const HCHAR *pUdn, const HCHAR *pCid, HUINT32 nSortOrder, HUINT32 nStartIdx, HUINT32 nReqIdx)
{
	if (PL_DMP_IsDMPStarted() == FALSE)
		HxLOG_Error( "[ DMS ] Error> No running DMP!\n" );

	if(nStartIdx == 0 && nReqIdx == 0)
		PL_DMP_Browse(Dmp_BrowseMode_Cache, (HCHAR *)pUdn, (HCHAR *)pCid, Dmp_BrowseFlag_Children, nSortOrder, nStartIdx, nReqIdx);
	else
		PL_DMP_Browse(Dmp_BrowseMode_Direct, (HCHAR *)pUdn, (HCHAR *)pCid, Dmp_BrowseFlag_Children, nSortOrder, nStartIdx, nReqIdx);

	return ERR_OK;
}

HERROR 	DMP_Search(const HCHAR *pUdn, const HCHAR *pCid, const HCHAR *pKeyword, HUINT32 nSortOrder, HUINT32 nStartIdx, HUINT32 nReqNum, HINT32 nReqId)
{
	if (PL_DMP_IsDMPStarted() == FALSE)
		HxLOG_Error( "[ DMS ] Error> No running DMP!\n" );

	return 	PL_DMP_Search( Dmp_SearchMode_Title, (HCHAR *)pUdn, (HCHAR *)pCid, (HCHAR *)pKeyword, nSortOrder, nStartIdx, nReqNum, nReqId);
}

HERROR 	DMP_SearchAll(const HCHAR *pCid, const HCHAR *pKeyword, HUINT32 nSortOrder, HINT32 nReqId)
{
	/* No UDN, startIdx, reqNum */
	return 	PL_DMP_SearchAll( Dmp_SearchMode_Title, (HCHAR *)pCid, (HCHAR *)pKeyword, nSortOrder, nReqId);
}

HERROR	DMP_GetDmsList(HUINT32 *ulCount, PL_DMP_DmsInfo_t **ppDmsList)
{
	return PL_DMP_GetDmsList(ulCount, ppDmsList);
}

HERROR	DMP_GetItemList(HUINT32 *ulCount, CO_MediaInstence_t **ppDmsList)
{
	return PL_DMP_GetCurrentItemList(ulCount, ppDmsList);
}

HERROR	DMP_GetCurrentDms (HCHAR **pUdn)
{
	return PL_DMP_GetCurrentDms(pUdn);
}

HERROR	DMP_RegisterEventCallback(DMP_EventCallback_t cb)
{
	pst_dmp.event_cb = cb;
	return ERR_OK;
}

HERROR	DMP_UnregisterEventCallback(void)
{
	pst_dmp.event_cb = NULL;
	return ERR_OK;
}

HERROR	DMP_RefreshDms(HCHAR *pUdn) /* for mxDlnaMscp_isAliveDevice() */
{
	return PL_DMP_RefreshDms(pUdn);
}

/* end od file */
