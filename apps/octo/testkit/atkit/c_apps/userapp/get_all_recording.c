#include <dlib.h>
#include <hapi.h>
#include <vkernel.h>
#include <apk.h>
#include <hlib.h>
#include <dapi.h>
#define GET_ALL_RECORDINGS_RESULT_FILE_NAME		"/home/atkit/result/get_all_recording.result"

static HBOOL bRun = FALSE;

// Event Dispatcher
static void local_eventTask(void *arg)
{
	while(bRun)
	{
		APK_EVENT_Dispatch();
		HLIB_STD_TaskSleep(500);
	}

}

int main(int argc, char **argv)
{
	HERROR			hErr = ERR_FAIL;
	HCHAR			*szResultText = NULL;
	HUINT32			ulResultLen = 0;

	HINT32			i = 0;
	HULONG			ulTaskId = 0;
	HUINT32			nTryCount = 0;	

	HxJSON_t 		j_root = NULL;
	HxJSON_t 		j_recordings = NULL;

	HxVector_t		*pstRecVector = NULL;


	bRun = TRUE;
	VK_Init();

	VK_TASK_Create(local_eventTask, VK_TASK_PRIORITY_MW_REF, SIZE_256K, "getAllRecording", NULL, &ulTaskId, 0);
	VK_TASK_Start(ulTaskId);

	HLIB_JSON_Init (eHLIB_JSON_Conf_Minimum, (void *(*)(unsigned long))HLIB_STD_MemAlloc_CB, (int (*)(void *))HLIB_STD_MemFree_CB);

	APK_META_RECORD_Init();
	APK_META_RECORD_Load();

	// Wait for first scan( max 30 sec )
	while(APK_META_RECORD_IsFirstScanDone() == FALSE)
	{
		HLIB_STD_TaskSleep(200);
		++nTryCount;
		if(nTryCount == 150)	// 200ms * 150 = 30 sec
		{
			printf("Time Out : Wait for first scan\n");
			return ERR_FAIL;
		}
		printf("Wait for First Recording Scan[%d]\n", nTryCount);
	}
	printf("HERE\n\n\n");

	j_root = HxJSON_Object_New();
	j_recordings = HxJSON_Array_New();

	pstRecVector = APK_META_RECORD_GetItems();
	for(i = 0; i < HLIB_VECTOR_Length(pstRecVector) ; ++i)
	{
		DxRecListData_t *pstRecData = NULL;
		
		pstRecData = HLIB_VECTOR_ItemAt(pstRecVector, (HINT32)i);
		if(pstRecData != NULL)
		{
			HxJSON_t j_rec = HxJSON_Object_New();
			UNIXTIME startTime = 0;
			UNIXTIME endTime = 0;
			
			// Mandatory : svc_name, content_name, start_time, end_time
			{
				HxJSON_Object_Set(j_rec, "svc_name", HxJSON_String_New(pstRecData->szSvcName));
				HxJSON_Object_Set(j_rec, "content_name", HxJSON_String_New(pstRecData->szName));

				startTime = pstRecData->ulRecStartTime;
				HxJSON_Object_Set(j_rec, "start_time", HxJSON_Integer_New(startTime));
			
				endTime = pstRecData->ulRecStartTime + pstRecData->ulRecDuration;
				HxJSON_Object_Set(j_rec, "end_time", HxJSON_Integer_New(endTime));
			}

			// Optional
			{
				HxJSON_Object_Set(j_rec, "content_id", HxJSON_Integer_New(pstRecData->ulContentId));
				HxJSON_Object_Set(j_rec, "content_url", HxJSON_String_New(pstRecData->szUrl));

				HxJSON_Object_Set(j_rec, "rec_status", HxJSON_Integer_New(pstRecData->eRecStatus));
				HxJSON_Object_Set(j_rec, "rec_endreason", HxJSON_Integer_New(pstRecData->eRecEndReason));
				HxJSON_Object_Set(j_rec, "video_type", HxJSON_Integer_New(pstRecData->eVideoType));				
			}

			
			HLIB_JSON_Array_Append(j_recordings, j_rec);
		}
		
	}
	HxJSON_Object_Set(j_root, "recordings", j_recordings);

	// write j_root to 'schedules.json' file
	szResultText = HLIB_JSON_Encode(j_root, NULL, 0);
	if(NULL == szResultText)
	{
		fprintf(stderr, "szResultText is NULL!!\n");
		goto END_FUNC;
	}

	ulResultLen = HxSTD_StrLen(szResultText);
	if(0 == ulResultLen)
	{
		fprintf(stderr, "Result text size is 0!\n");
		goto END_FUNC;
	}

	printf("%s\n", szResultText);

	hErr = HLIB_FILE_WriteFile((const HUINT8 *)GET_ALL_RECORDINGS_RESULT_FILE_NAME, (void *)szResultText, ulResultLen);
	if(ERR_OK != hErr)
	{
		fprintf(stderr, "Fail to HLIB_FILE_WriteFile( %s ) - %d\n", GET_ALL_RECORDINGS_RESULT_FILE_NAME, hErr);
		goto END_FUNC;
	}

END_FUNC:
	bRun = FALSE;
	
	if(pstRecVector != NULL)
	{
		printf("pstRecVector = %p\n", pstRecVector);
		HLIB_VECTOR_Delete(pstRecVector);
	}

	if(j_root != NULL)
	{
		HLIB_JSON_Delete(j_root);
	}

	APK_META_RECORD_DeInit();

	printf("End of main()\n");

	return 0;
}
