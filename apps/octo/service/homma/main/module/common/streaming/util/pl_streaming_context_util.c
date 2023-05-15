/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "../int_streaming.h"


/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC void	_plstreamingcontextutil_GetContentID(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
    DAxIterator_t		iter;
    HUID    *cid  = (HUID*)userdata;

    HxLOG_Info("%s(name:%s, uid:"HUID_FMT", data:0x%X, size:%u, qid:%d) time:%s(%u)\n"
            , __FUNCTION__, name, uid, data, size, (int)userdata
            , HLIB_DATETIME_UnixTimeToString(HLIB_STD_GetSystemTime(), NULL, 0), HLIB_STD_GetSystemTick());

    if( size == 0 )
    {
        HxLOG_Warning("failed because of size = 0\n" );
        return;
    }

    DAPI_InitIterator(&iter, data, size, 0);
    while (DAPI_NextIterator(&iter))
    {
        *cid = iter.uid;
        return; // 2013/11/27 mhkang: 같은 경로의 같은 파일이름으로 레코드가 2개 들어가 있을 수 없다. 바로 return.
    }
}

STATIC void	_plstreamingcontextutil_GetRecDuration(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
    DAxIterator_t		iter;
    HINT32    *pRecDuration  = (HINT32*)userdata;
    DxRecListData_t	*rec;

    HxLOG_Info("%s(name:%s, uid:"HUID_FMT", data:0x%X, size:%u, qid:%d) time:%s(%u)\n"
            , __FUNCTION__, name, uid, data, size, (int)userdata
            , HLIB_DATETIME_UnixTimeToString(HLIB_STD_GetSystemTime(), NULL, 0), HLIB_STD_GetSystemTick());

    if( size == 0 )
    {
        HxLOG_Warning("failed because of size = 0\n" );
        return;
    }

    DAPI_InitIterator(&iter, data, size, 0);
    while (DAPI_NextIterator(&iter))
    {
        rec  = (DxRecListData_t *)iter.data;
        *pRecDuration = rec->ulRecDuration;

        return;
    }
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

HOM_Result_e PL_StreamingContextUtil_MakeTuningParamAndDisplayString(PLStreaming_ModeInst_t *pstInst)
{
	HxSTR_t 		*pChNameStr = NULL;
	HOM_Result_e	eHomResult = eHOM_Result_Ok;

	pChNameStr = HLIB_STR_New("");
	if(NULL == pChNameStr)
	{
		HxLOG_Error("HLIB_STR_New fail!! \r\n");
		return eHOM_Result_Error_OutofMem;
	}

	eHomResult = PL_COMMON_GetTuningInfoChannelName(pstInst->stSetupInfo.uUID, &pstInst->stSetupInfo.stTuningParam,  &pChNameStr);
	if(eHOM_Result_Ok != eHomResult )
	{
		HxLOG_Error("PL_COMMON_GetTuningInfoChannelName fail!! \r\n");
		return eHOM_Result_Error_OutofMem;
	}

	pChNameStr = HLIB_STR_Append(pChNameStr, " (");
	if(NULL == pChNameStr)
	{
		HxLOG_Error("HLIB_STR_Append fail!! \r\n");
		return eHOM_Result_Error_OutofMem;
	}

	pChNameStr = HLIB_STR_Append(pChNameStr, pstInst->stSetupInfo.pszDisplayString);
	if(NULL == pChNameStr)
	{
		HxLOG_Error("HLIB_STR_Append fail!! \r\n");
		return eHOM_Result_Error_OutofMem;
	}

	if(NULL != pstInst->stSetupInfo.pszDisplayString)
	{
		HLIB_STD_MemFree(pstInst->stSetupInfo.pszDisplayString);
		pstInst->stSetupInfo.pszDisplayString = NULL;
	}

	pstInst->stSetupInfo.pszDisplayString = HLIB_STD_StrDup(HLIB_STR_GetBuffer(pChNameStr));

	HLIB_STR_Delete(pChNameStr);
	pChNameStr = NULL;

	return eHomResult;
}

HOM_Result_e PL_StreamingContextUtil_GetUID(const PLStreaming_StartInfo_t *pstStartInfo, HUID *ret_puUID)
{
    HCHAR*  pUIDStr = NULL;
    HCHAR*  path_value = NULL;
    HCHAR*  query = NULL;
    HUID	uUID = 0;
	HINT32	nUid = 0;
    HUINT32	rId;

	*ret_puUID = 0;

	switch(pstStartInfo->eStreamType)
	{
	case DxRSVSTREAM_TYPE_HLS_LIVE_VIDEO:
	case DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO:
	case DxRSVSTREAM_TYPE_HLS_FILE_TSR:
		pUIDStr = HLIB_STD_StrCaseStr(pstStartInfo->pszStreamingPath, PL_STREAMING_HLS_DEFAULT_CHANNEL_PREFIX);
		if(NULL != pUIDStr)
		{

			pUIDStr += HxSTD_StrLen(PL_STREAMING_HLS_DEFAULT_CHANNEL_PREFIX);
			nUid = (HINT32)HLIB_STD_StrToINT32(pUIDStr);

			HxLOG_Print("pszStreamingPath = %s, pUIDStr = %s, nUid = %d\n", pstStartInfo->pszStreamingPath, pUIDStr, nUid);
			*ret_puUID = nUid;
			return eHOM_Result_Ok;
		}
		break;

    case DxRSVSTREAM_TYPE_HLS_FILE_PVR:
	case DxRSVSTREAM_TYPE_HLS_FILE_PVR_AUDIO:
	case DxRSVSTREAM_TYPE_HLS_FILE_MUSIC:
        path_value = HLIB_STD_StrDup(pstStartInfo->pszStreamingPath);
        HLIB_PATH_SplitExt (pstStartInfo->pszStreamingPath, path_value,  strlen(pstStartInfo->pszStreamingPath));

        if (0)
        {
		// 2013/11/27 mhkang: path_value에 ' "" 등등 기호가 오면 sql query 실패할 수 있음(dama담당자에게 가이드 받아서 차후 수정필요)
		query = HLIB_STD_FmtString(NULL, 0, "SELECT * FROM %s WHERE path LIKE '%s'" , DxNAMEOF(DxRecListData_t) , path_value );
        }
        else
        {
		/* Fixed duplicated query result error;
		   Dont use 'SQE LIKE' because there can be existed overlapped title path like xxxx - (1), xxxx - (2), .... */
		query = HLIB_STD_FmtString(NULL, 0, "SELECT * FROM %s WHERE path = \"%s\"" , DxNAMEOF(DxRecListData_t) , path_value );
        }

        if (path_value)
        {
        	HLIB_STD_MemFree(path_value);
        }

        if (query == NULL)
        {
            HxLOG_Error("HLIB_STD_FmtString failed(out of memory)\n");
            break;
        }

        rId =  DAPI_Query(query, (DAPI_Getter)_plstreamingcontextutil_GetContentID, &uUID);
        HLIB_STD_FreeFmtString(query); query = NULL;

        if (rId != 0)
        {
            if( DAPI_Wait( 5000, rId ) == DERR_TIMEOUT) //  timeout 5 sec
            {
                HxLOG_Warning("DAPI_Wait api timeout!\n");
                break;
            }
        }
        else
        {
            HxLOG_Error("DAPI_Query failed. rid = 0\n", rId);
            break;
        }

        HxLOG_Error(" uid=%lld!\n", uUID);

        *ret_puUID = uUID;
        return eHOM_Result_Ok;

	case DxRSVSTREAM_TYPE_NTLS_LIVE_VIDEO:
	case DxRSVSTREAM_TYPE_NTLS_LIVE_AUDIO:
	case DxRSVSTREAM_TYPE_NTLS_FILE_TSR:
		pUIDStr = HLIB_STD_StrCaseStr(pstStartInfo->pszStreamingPath, PL_STREAMING_ByPass_DEFAULT_CHANNEL_PREFIX);
		if(NULL != pUIDStr)
		{

			pUIDStr += HxSTD_StrLen(PL_STREAMING_ByPass_DEFAULT_CHANNEL_PREFIX);
			nUid = (HINT32)HLIB_STD_StrToINT32(pUIDStr);

			HxLOG_Print("pszStreamingPath = %s, pUIDStr = %s, nUid = %d\n", pstStartInfo->pszStreamingPath, pUIDStr, nUid);
			*ret_puUID = nUid;
			return eHOM_Result_Ok;
		}
		break;

    case DxRSVSTREAM_TYPE_NTLS_FILE_PVR:
	case DxRSVSTREAM_TYPE_NTLS_FILE_PVR_AUDIO:
	case DxRSVSTREAM_TYPE_NTLS_FILE_MUSIC:
		#if 0
        path_value = HLIB_STD_StrDup(pstStartInfo->pszStreamingPath);
        HLIB_PATH_SplitExt (pstStartInfo->pszStreamingPath, path_value,  strlen(pstStartInfo->pszStreamingPath));

        // 2013/11/27 mhkang: path_value에 ' "" 등등 기호가 오면 sql query 실패할 수 있음(dama담당자에게 가이드 받아서 차후 수정필요)
        query = HLIB_STD_FmtString(NULL, 0, "SELECT * FROM %s WHERE path LIKE '%s'" , DxNAMEOF(DxRecListData_t) , path_value );
        if (query == NULL)
        {
            HxLOG_Error("HLIB_STD_FmtString failed(out of memory)\n");
            break;
        }

        rId =  DAPI_Query(query, (DAPI_Getter)_plstreamingcontextutil_GetContentID, &uUID);
        HLIB_STD_FreeFmtString(query); query = NULL;

        if (rId != 0)
        {
            if( DAPI_Wait( 5000, rId ) == DERR_TIMEOUT) //  timeout 5 sec
            {
                HxLOG_Warning("DAPI_Wait api timeout!\n");
                break;
            }
        }
        else
        {
            HxLOG_Error("DAPI_Query failed. rid = 0\n", rId);
            break;
        }

        HxLOG_Error(" uid=%lld!\n", uUID);

        *ret_puUID = uUID;
        return eHOM_Result_Ok;
		#else
		break;
		#endif

	default:
		HxLOG_Error("Unknown Streaming Type pstStartInfo->eStreamType(0x%x)\n", pstStartInfo->eStreamType);
		break;
	}

	return eHOM_Result_Error;
}

HOM_Result_e PL_StreamingContextUtil_GetRecDuration(HUID UID, HINT32* pRecDuration)
{
    HCHAR*  query = NULL;
    HUINT32	rId;
    HOM_Result_e	eHomResult = eHOM_Result_Ok;

    query = HLIB_STD_FmtString(NULL, 0, "SELECT * FROM %s WHERE uid=%lld" , DxNAMEOF(DxRecListData_t) , UID );
    if (query == NULL)
    {
        HxLOG_Error("HLIB_STD_FmtString failed(out of memory)\n");
        eHomResult = eHOM_Result_Error_Invalid_Arguments;
        goto ErrBlock;
    }

    rId =  DAPI_Query(query, (DAPI_Getter)_plstreamingcontextutil_GetRecDuration, pRecDuration);
    if (rId != 0)
    {
        if( DAPI_Wait( 5000, rId ) == DERR_TIMEOUT) //  timeout 5 sec
        {
            HxLOG_Warning("DAPI_Wait api timeout!\n");
            eHomResult = eHOM_Result_Error_Timeout;
            goto ErrBlock;
        }
    }
    else
    {
        HxLOG_Error("DAPI_Query failed(query:%s)\n", query);
        eHomResult = eHOM_Result_Error;
        goto ErrBlock;
    }

    if (query != NULL)
    {
        HLIB_STD_FreeFmtString(query); query = NULL;
    }

    return eHomResult;

ErrBlock:

    if (query != NULL)
    {
        HLIB_STD_FreeFmtString(query); query = NULL;
    }

    return eHomResult;
}


/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/


/* end of file */

