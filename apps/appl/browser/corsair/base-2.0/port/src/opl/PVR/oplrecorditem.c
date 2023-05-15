/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


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

/**************************************************************************************************/
#define _________OPLRecordItem_Private_Include_________________________________________________
/**************************************************************************************************/
#include "oplrecording.h"
#include "oplscheduler.h"
#include "oplrecorditem.h"
#include "hlib.h"
#include "apk.h"
/**************************************************************************************************/
#define _________OPLRecordItem_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________OPLRecordItem_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________OPLRecordItem_Private_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________OPLRecordItem_Private_Prototype_________________________________________________
/**************************************************************************************************/
static void 	opl_recorditem_EventListener (ApkMetaRec_Handle_t self, HINT32 event, const HINT32 *args, void *userdata);


/**************************************************************************************************/
#define _________OPLRecordItem_Private_Init__________________________________________________________
/**************************************************************************************************/
OPL_HANDLE	OPL_recorditem_GetInstance (void)
{
	static OPL_RecordListHandle_t *s_handle = NULL;


	if(s_handle == NULL)
	{
		s_handle = (OPL_RecordListHandle_t*)HLIB_STD_MemCalloc(sizeof(OPL_RecordListHandle_t));
	}

	return s_handle;
}



/**************************************************************************************************/
#define _________OPLRecordItem_Private_Functions____________________________________________________
/**************************************************************************************************/
static HBOOL		opl_recorditem_EmitEvent (int state, int error, OPL_Recording_t *recorditem)
{
	OPL_RecordListHandle_t *handle;

	HxLOG_Trace();
	HxLOG_Warning("opl_recorditem_EmitEvent send state = %d\n", state);
	handle = OPL_recorditem_GetInstance();

	if (handle->listener)
	{
		return handle->listener(state, error, recorditem) ? TRUE : FALSE;
	}
	return FALSE;
}

static int 	opl_recorditem_convert_cmd_ApkToState(APK_META_REC_CMD_e eAPK,HINT32 oplRecState)
{
	int eRet;

	switch (eAPK)
	{
	case eAPK_META_REC_EVENT_ADDED:
		{
			switch(oplRecState)
			{
				case eOPL_SCHEDULER_EVENT_STARTED:
					eRet = eOPL_SCHEDULER_EVENT_STARTED;
					break;
				default:
					eRet = eOPL_SCHEDULER_EVENT_SCHEDULED;
					break;
			}
		}
		break;
	case eAPK_META_REC_DO_DELETE:
	case eAPK_META_REC_EVENT_DELETED:
		eRet = eOPL_SCHEDULER_EVENT_DELETED; //³ìÈ­¹° »èÁ¦
		break;
	case eAPK_META_REC_EVENT_UPDATED:
		eRet = eOPL_SCHEDULER_EVENT_UPDATED;
		break;
	case eAPK_META_REC_EVENT_LOAD:
		eRet = eOPL_SCHEDULER_EVENT_LOAD;
		break;
	default:
		eRet = eOPL_SCHEDULER_EVENT_ERROR;
	}
	//printf("[%s:%d]eRet[%d],oplRecState[%d]\n",__FUNCTION__,__LINE__,eRet,oplRecState);
	return eRet;
}


static HCHAR *opl_recorditem_enumToApkRecCMD(APK_META_REC_CMD_e eCmd)
{
	switch(eCmd)
	{
		ENUM_TO_STR(eAPK_META_REC_DoNothing)
		ENUM_TO_STR(eAPK_META_REC_DO_SCAN)
		ENUM_TO_STR(eAPK_META_REC_DO_COPY)
		ENUM_TO_STR(eAPK_META_REC_DO_DELETE)
		ENUM_TO_STR(eAPK_META_REC_EVENT_START)
		ENUM_TO_STR(eAPK_META_REC_EVENT_DELETED)
		ENUM_TO_STR(eAPK_META_REC_EVENT_UPDATED)
		ENUM_TO_STR(eAPK_META_REC_EVENT_ADDED)
		ENUM_TO_STR(eAPK_META_REC_EVENT_LOAD)
		ENUM_TO_STR(eAPK_META_REC_CmdEnd)
	}

	return "unknown....";
}


static HCHAR *opl_recorditem_enumToOplRecState(int  oplstate)
{
	switch(oplstate)
	{
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_UNREALIZED)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_SCHEDULED)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_PRESTART)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_ACQUIRING_RESOURCES)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_STARTED)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_UPDATED)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_COMPLETED)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_PARTIALY_COMPLETED)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_DELETED)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_RECLIST_COMPLETED)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_START_ERROR)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_START_CONFLICT)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_LOAD)
		ENUM_TO_STR(eOPL_SCHEDULER_EVENT_ERROR)
	}

	return "unknown....";
}



static void 	opl_recorditem_EventListener (ApkMetaRec_Handle_t self, HINT32 event, const HINT32 *args, void *userdata)
{
	int				state, error = eOPL_SCHEDULER_ERROR_NONE;
	char			*szUrl = NULL;
	OPL_Recording_t	*pstOplRecording = NULL;
	DxRecListData_t	*pDxRecListData;

	HxLOG_Trace();
	if (NULL == args)
	{
		HxLOG_Error("args is NULL, event = (%d) \n", event);
		pstOplRecording = (OPL_Recording_t*)HLIB_STD_MemCalloc(sizeof(OPL_Recording_t)); //dummy...
		pstOplRecording->id = "dummy";
		opl_recorditem_EmitEvent(eOPL_SCHEDULER_EVENT_LOAD,eOPL_SCHEDULER_ERROR_NONE,pstOplRecording);
		return;
	}

	szUrl = (char*)args;
	HxLOG_Print("szUrl (%s)\n", szUrl);

	pDxRecListData = APK_META_RECORD_GetByUrl(szUrl);

	if(pDxRecListData == NULL)
	{
		HxLOG_Error("Can't find Record... url(%s)!\n",szUrl);
		return;
	}
        
        pstOplRecording = OPL_Recording_New("o", pDxRecListData);

        if ( pstOplRecording )
        {
        	error = pstOplRecording->error;
        	state =opl_recorditem_convert_cmd_ApkToState(event,pstOplRecording->state);

        	HxLOG_Debug("*************************************************\n");
        	HxLOG_Debug("DxRec Url [%s]\n",pDxRecListData->szUrl);
        	HxLOG_Debug("APK Event[%s],OPL State[%s],Err[%d]\n",opl_recorditem_enumToApkRecCMD(event),
        			opl_recorditem_enumToOplRecState(pstOplRecording->state),pstOplRecording->error);
        	HxLOG_Debug("EmitEvent [%s] oplid[%s]\n",opl_recorditem_enumToOplRecState(state),pstOplRecording->id);
        	HxLOG_Debug("*************************************************\n");

        	opl_recorditem_EmitEvent(state,error,pstOplRecording);

        	if(event == eAPK_META_REC_EVENT_UPDATED )
        	{
        		if(pstOplRecording->state ==eOPL_SCHEDULER_EVENT_COMPLETED ||
        			pstOplRecording->state ==eOPL_SCHEDULER_EVENT_PARTIALY_COMPLETED)
        		{
        			pstOplRecording = OPL_Recording_New("o", pDxRecListData);
        			opl_recorditem_EmitEvent(eOPL_SCHEDULER_EVENT_COMPLETED,error,pstOplRecording);
        		}
        		
        		if(pstOplRecording->error != eOPL_SCHEDULER_ERROR_NONE)
        		{
        			pstOplRecording = OPL_Recording_New("o", pDxRecListData);
        			opl_recorditem_EmitEvent(eOPL_SCHEDULER_EVENT_ERROR,error,pstOplRecording);
        		}
        	}
        }

	APK_META_RECORD_Release(pDxRecListData);
}



/**************************************************************************************************/
#define _________OPLRecordItem_Public_Functions_____________________________________________________
/**************************************************************************************************/
void		OPL_RecordItem_AddEventListener (OPL_RecordingItem_EventListener_t listener)
{
	OPL_RecordListHandle_t *handle;

	HxLOG_Trace();

	if (listener)
	{
		handle = OPL_recorditem_GetInstance();
		handle->listener = listener;
		APK_META_RECORD_EnableLoadEvent();
		APK_META_RECORD_AddNotifier(opl_recorditem_EventListener, NULL);
	}
}

int			OPL_RecordItem_CountRecordings (void)
{
	HUINT32 count;
	HxLOG_Trace();
	count = APK_META_RECORD_Count();
	HxLOG_Debug("[%s:%d]count[%d]\n",__FUNCTION__,__LINE__,count);


	return count;
}

int			OPL_RecordItem_CountRecordingsByDiscId (const char *uuid)
{
	HUINT32	ulLen,ulCount = 0;
	HINT32	i;
	DxRecListData_t *item;

	HxLOG_Trace();
	if(HxSTD_StrEmpty(uuid))
	{
		HxLOG_Warning("OPL_Scheduler_CountRecordingsByDiscId (input...NULL!!)");
		return ulCount;
	}

	ulLen = APK_META_RECORD_Count();
	for (i = 0 ; i < ulLen   ; i++)
	{
		item = APK_META_RECORD_GetByIdx(i);
		if(item)
		{
			if(HxSTD_StrEmpty(item->szStorageId)== FALSE && HxSTD_StrCmp(item->szStorageId,uuid)==0)
			{
				ulCount++;
			}
			APK_META_RECORD_Release(item);
		}
	}
	return ulCount;
}


OPL_Recording_t *	OPL_RecordItem_GetRecordingAt (int index)
{
	DxRecListData_t *pDxRecListData;
	OPL_Recording_t	*pstOplRecording = NULL;

	HxLOG_Trace();

	pDxRecListData = APK_META_RECORD_GetByIdx(index);

	if(pDxRecListData)
	{
		pstOplRecording = OPL_Recording_New("o", pDxRecListData);
		HxLOG_Debug("[%d]FileName[%s],ID[%s]\n",index,pstOplRecording->szFileName,pstOplRecording->id);

		APK_META_RECORD_Release(pDxRecListData);
	}
	else{
		HxLOG_Error("OPL_RecordItem_GetRecordingAt- Can't find Record... index(%d)!\n",index);
	}
	return pstOplRecording;
}


OPL_Recording_t * 	OPL_RecordItem_GetRecordingAtDiscId (int index, const char* discId)
{
	OPL_Recording_t	*pstRecItem = NULL;

	HxLOG_Trace();

	pstRecItem =OPL_RecordItem_GetRecordingAt(index);
	if (pstRecItem)
	{
		if (HxSTD_StrCmp(pstRecItem->super.storageID, discId) ==0)
			return pstRecItem;
	}
	else{
		HxLOG_Error("OPL_RecordItem_GetRecordingAtDiscId- Can't find Record... discId(%s)!\n",discId);
	}
	OPL_Recording_Delete(pstRecItem);
	return NULL;
}


OPL_Recording_t *	OPL_RecordItem_GetRecordingByID (const char *id)
{
	OPL_Recording_t	*pstOplRecording = NULL;
	DxRecListData_t *pDxRecListData;

	HxLOG_Trace();
	pDxRecListData = APK_META_RECORD_GetByUrl(id);

	if(pDxRecListData)
	{
		pstOplRecording = OPL_Recording_New("o", pDxRecListData);
		APK_META_RECORD_Release(pDxRecListData);
	}
	else{
		HxLOG_Error("OPL_RecordItem_GetRecordingByID- Can't find Record... id(%s)!\n",id);
	}
	return pstOplRecording;
}


OPL_Collection_t *	OPL_RecordItem_GetRunningRecordings(void)
{
	OPL_Collection_t *RecordCollection = NULL;
	OPL_Recording_t	 *pstOplRecording = NULL;
	DxRecListData_t	*rec;
	HINT32 			 RecLen,i;
	HxVector_t		 *vector;
	HxLOG_Trace();

	vector=	APK_META_RECORD_GetItems();
	if(vector== NULL)
		return RecordCollection;

	RecordCollection = OPL_Collection_NewEx(OPL_Recording_Delete, (void *(*)(void *))OPL_Recording_Clone);
	if(RecordCollection == NULL)
	{
		HLIB_VECTOR_Delete(vector);
		return NULL;
	}

	RecLen = HLIB_VECTOR_Length(vector);

	for(i=0; i< RecLen; i++)
	{
		rec   = (DxRecListData_t*)HLIB_VECTOR_ItemAt(vector, i);
		// condition:  eOPL_SCHEDULER_EVENT_STARTED ( eRecStats -> opl (opl_recording_GetState))
		if (rec && (rec->eRecStatus == eDxREC_STS_RECORD_START || rec->eRecStatus == eDxREC_STS_RECORDING))
		{
			pstOplRecording = OPL_Recording_New("o", rec);
			if (pstOplRecording != NULL)
			{
				(void) OPL_Collection_Add(RecordCollection, (void *)pstOplRecording);
			}
		}

	}
	HLIB_VECTOR_Delete(vector);
	return RecordCollection;
}



OPL_Collection_t *	OPL_RecordItem_GetRecordings(void)
{
	OPL_Collection_t *RecordCollection = NULL;
	OPL_Recording_t	 *pstOplRecording = NULL;
	DxRecListData_t	*rec;
	HINT32 			 RecLen,i;
	HxVector_t		 *vector;
	HxLOG_Trace();

	vector=	APK_META_RECORD_GetItems();
	if(vector== NULL)
		return RecordCollection;

	RecordCollection = OPL_Collection_NewEx(OPL_Recording_Delete, (void *(*)(void *))OPL_Recording_Clone);
	if(RecordCollection == NULL)
	{
		HLIB_VECTOR_Delete(vector);
		return NULL;
	}

	RecLen = HLIB_VECTOR_Length(vector);

	for(i=0; i< RecLen; i++)
	{
		rec   = (DxRecListData_t*)HLIB_VECTOR_ItemAt(vector, i);
		if (rec)
		{
			pstOplRecording = OPL_Recording_New("o", rec);
			(void) OPL_Collection_Add(RecordCollection, (void *)pstOplRecording);
		}
	}
	HLIB_VECTOR_Delete(vector);
	return RecordCollection;
}




/**************************************************************************************************/
#define _________OPLRecordItem_End_Of_File____________________________________________________
/**************************************************************************************************/

