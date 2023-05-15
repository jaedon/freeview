
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

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <trauma_int.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define TRAUMA_ERIN_TIMEOUT				(3 *60 *1000)
#define TRAUMA_ERIN_REPORT_LEV			(HxLOG_ERROR | HxLOG_CRITICAL | HxLOG_ASSERT)
#define TRAUMA_ERIN_TRIGGER_REMOVE		(50)
#define TRAUMA_ERIN_REMOVE_COUNT		(TRAUMA_ERIN_TRIGGER_REMOVE- 5)

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */
static HINT32 		s_rmcount =0;
static HxList_t 	*s_pLogData = NULL;
/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
static HBOOL trauma_erin_checklevel_condition(void * userdata,void * data)
{
	HUINT32			ulErrcode 		= (HUINT32)userdata;
	TRAPI_ERIN_t	*pItem			= (TRAPI_ERIN_t*)data;

	if(pItem->ulErr_code == ulErrcode)
		return TRUE;
	return FALSE;
}

static HxList_t* trauma_erin_checklevel(HxLOG_Level_e eLev,HUINT32 ulErrcode)
{
	HxList_t *pRet = NULL,*pList;

	if(eLev & TRAUMA_ERIN_REPORT_LEV)
	{
		pList = s_pLogData;

		while(pList)
		{
			TRAPI_ERIN_t *data = (TRAPI_ERIN_t*)HLIB_LIST_Data(pList);

			if(data->ulErr_code == ulErrcode)
			{
				pRet = HLIB_LIST_Append(pRet,HLIB_MEM_MemDup(data,sizeof(TRAPI_ERIN_t)));
			}
			pList = HLIB_LIST_Next(pList);
		}
		s_pLogData = HLIB_LIST_RemoveIf(s_pLogData,(void*)ulErrcode,trauma_erin_checklevel_condition,HLIB_MEM_Free_CB);
	}
	return pRet;
}

static HCHAR trauma_erin_getLevToCharactor(HxLOG_Level_e eLev)
{
	switch(eLev)
	{
	case HxLOG_PRINT	:	return 'P';
	case HxLOG_MESSAGE	:	return 'M';
	case HxLOG_INFO		:	return 'I';
	case HxLOG_DEBUG	:	return 'D';
	case HxLOG_TRACE	:	return 'T';
	case HxLOG_WARNING	:	return 'W';
	case HxLOG_ERROR	:	return 'E';
	case HxLOG_CRITICAL	:	return 'C';
	case HxLOG_ASSERT 	:	return 'A';
	default:
		return '-';
	}
	return '-';
}

static HBOOL trauma_erin_checktimeout_condition(void * userdata,void * data)
{
	UNIXTIME		cur_time = (UNIXTIME)userdata;
	TRAPI_ERIN_t	*pData = (TRAPI_ERIN_t*)data;
	HINT32 			diff = cur_time - pData->ulSystime;

	if(abs(diff) >= (TRAUMA_ERIN_TIMEOUT)/1000)
	{
		return TRUE;
	}
	return FALSE;

}

static void trauma_erin_checktimeoutCallback(HULONG timerid,void   *userdata)
{
	TRAUMA_MSG_t stMsg;

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));

	stMsg.msg = eTRAUMA_MSG_ERIN_CHECK_TIMEOUT;
	TRAUMA_MSG_Send(&stMsg);
}

static void trauma_erin_checktimeout(void)
{
	UNIXTIME					cur_time;

	cur_time = HLIB_STD_GetSystemTime();
	HxLOG_Debug(">>Before Len[%d]\n",HLIB_LIST_Length(s_pLogData));
	s_pLogData = HLIB_LIST_RemoveIf(s_pLogData,(void*)cur_time,trauma_erin_checktimeout_condition,HLIB_MEM_Free_CB);
	HxLOG_Debug(">>After Len[%d]\n",HLIB_LIST_Length(s_pLogData));

}

static HUINT32 trauma_erin_getcount_from_errcode(HxList_t *pList,HUINT32 ulErrcode)
{
	HUINT32 count=0;

	while(pList)
	{
		TRAPI_ERIN_t *data = (TRAPI_ERIN_t*)HLIB_LIST_Data(pList);

		if(data->ulErr_code == ulErrcode)
			count++;
		pList = HLIB_LIST_Next(pList);
	}
	return count;
}

static HBOOL trauma_erin_checkMaxcount_condition(void * userdata,void * data)
{
	TRAPI_ERIN_t	*pData = (TRAPI_ERIN_t*)data;
	HUINT32			ulErrcode = (HUINT32)userdata;

	if((pData->ulErr_code == ulErrcode) && s_rmcount-- >= 0)
		return TRUE;

	return FALSE;
}

static void trauma_erin_checkMaxcount(HxLOG_Level_e lev,HUINT32 ulErrcode)
{
	HxList_t *pList = NULL;
	HUINT32	 count =0;
	pList = s_pLogData;

	count = trauma_erin_getcount_from_errcode(pList,ulErrcode);
	if(count > TRAUMA_ERIN_TRIGGER_REMOVE)
	{
		s_rmcount = TRAUMA_ERIN_REMOVE_COUNT;
		HxLOG_Debug(">>Before Len[%d]\n",HLIB_LIST_Length(s_pLogData));
		s_pLogData = HLIB_LIST_RemoveIf(s_pLogData,(void*)ulErrcode,trauma_erin_checkMaxcount_condition,HLIB_MEM_Free_CB);
		HxLOG_Debug(">>Before Len[%d]\n",HLIB_LIST_Length(s_pLogData));
	}
}

static HxList_t* trauma_erin_check(TRAPI_ERIN_t *pErIn)
{
	HxLOG_Level_e	eLev;
	HUINT32 		ulErr_code;
	HxList_t 		*pReport = NULL;

	eLev 		= pErIn->eLevel;
	ulErr_code 	= pErIn->ulErr_code;

	pReport = trauma_erin_checklevel(eLev,ulErr_code);
	trauma_erin_checkMaxcount(eLev,ulErr_code);

	return pReport;
}

static HCHAR* trauma_erin_MakeReport(HxList_t *pReport)
{
	HINT32 		nDescStream,nRet,i=0;
	HCHAR		*pszReport = NULL;

	nDescStream 	= HLIB_RWSTREAM_OpenEx(NULL, 1024, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);

	HLIB_RWSTREAM_Print(nDescStream,"\"CODE\":\"ERIN\",");

	while(pReport)
	{
		TRAPI_ERIN_t *pErIn = (TRAPI_ERIN_t*)HLIB_LIST_Data(pReport);

		HLIB_STD_StrReplaceChar(pErIn->szLog,pErIn->szLog,'\n','\0');	//encode json defense code..
		HLIB_STD_StrReplaceChar(pErIn->szLog,pErIn->szLog,'\\','/');	//encode json defense code..
		HLIB_STD_StrReplaceChar(pErIn->szFilePath,pErIn->szFilePath,'\\','/');	//encode json defense code..

		if(i++ == 0)
		{
			HLIB_RWSTREAM_Print(nDescStream,"\"ERR_CODE\":\"0x%x\",",pErIn->ulErr_code);
			HLIB_RWSTREAM_Print(nDescStream,"\"ERR_INFO\":[");
		}
		else
		{
			HLIB_RWSTREAM_Print(nDescStream,",");
		}

		HLIB_RWSTREAM_Print(nDescStream,"{");
		HLIB_RWSTREAM_Print(nDescStream,"\"TIME\":%d,",pErIn->ulSystime);
		HLIB_RWSTREAM_Print(nDescStream,"\"PROCESS\":\"%s\",",pErIn->szProcess);
		HLIB_RWSTREAM_Print(nDescStream,"\"FILE\":\"%s\",",pErIn->szFilePath);
		HLIB_RWSTREAM_Print(nDescStream,"\"FUNC\":\"%s\",",pErIn->szFunction);
		HLIB_RWSTREAM_Print(nDescStream,"\"LINE\":%d,",pErIn->ulLine);
		HLIB_RWSTREAM_Print(nDescStream,"\"LEV\":\"%C\",",trauma_erin_getLevToCharactor(pErIn->eLevel));
		HLIB_RWSTREAM_Print(nDescStream,"\"LOG\":\"%s\"",pErIn->szLog);
		HLIB_RWSTREAM_Print(nDescStream,"}");

		pReport = HLIB_LIST_Next(pReport);
	}
	HLIB_RWSTREAM_Print(nDescStream,"]");

	nRet = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nDescStream));
	HLIB_RWSTREAM_Close(nDescStream);

	pszReport = HLIB_RWSTREAM_CloseWithoutBuf(nRet);
	return 	pszReport;
}

static void trauma_erin_receive(TRAUMA_MSG_t *pstMsgData)
{
	HxList_t	*pReport = NULL;
	TRAPI_ERIN_t *pErIn =(TRAPI_ERIN_t*)pstMsgData->param1;

	s_pLogData = HLIB_LIST_Append(s_pLogData,pErIn);
	pReport = trauma_erin_check(pErIn);
	if(pReport && HLIB_LIST_Length(pReport) >0)
	{
		TRAUMA_STATISTICS_AddReport(trauma_erin_MakeReport(pReport));
		(void)HLIB_LIST_RemoveAllFunc(pReport,HLIB_MEM_Free_CB);
	}
}

/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
void TRAUMA_ERIN_Process(TRAUMA_MSG_t *pstMsgData)
{
	static HULONG s_ulTimer = 0;
	if(s_ulTimer ==0 )
		(void)VK_TIMER_EventEvery(TRAUMA_ERIN_TIMEOUT, trauma_erin_checktimeoutCallback, NULL, 0,&s_ulTimer);

	switch(pstMsgData->msg)
	{
	case eTRAUMA_MSG_ERIN_RECEIVE:
		trauma_erin_receive(pstMsgData);
		break;
	case eTRAUMA_MSG_ERIN_CHECK_TIMEOUT:
		trauma_erin_checktimeout();
		break;
	default:
		break;
	}
 }



