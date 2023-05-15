#include <dlib.h>
#include <hapi.h>
#include <vkernel.h>
#include <apk.h>
#include <hlib.h>

#define GET_ALL_SCHEDULE_RESULT_FILE_NAME		"/home/atkit/result/get_all_schedule.result"

static HBOOL bRun = FALSE;
static HBOOL bScheduleLoaded = FALSE;
static HUINT32 ulNumSch = 0;


static void local_schedule_callback(void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	printf("===========================================\n");
	printf("[%s] eMsgType( %d )\n", __FUNCTION__, eMsgType);
	printf("===========================================\n");

	// we need a only 1 event.
	if(eMsgType == eAPK_METASCH_MSG_SchedulesUpdated)
	{
		APK_META_SCHEDULE_CountSchedule(&ulNumSch);
		printf("Schedule Count(%d)\n", ulNumSch);
		bScheduleLoaded = TRUE;

		// Don't need onother event 
		bRun = FALSE;
	}
}

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
	DxSchedule_t	*pstSchArray = NULL;
	HINT32			i = 0;
	HULONG			ulTaskId = 0;
	HUINT32			ulCnt = 0;

	HxJSON_t 		j_root = NULL;
	HxJSON_t 		j_schedules = NULL;

	bRun = TRUE;
	VK_Init();

	VK_TASK_Create(local_eventTask, VK_TASK_PRIORITY_MW_REF, SIZE_256K, "getAllSchedule", NULL, &ulTaskId, 0);
	VK_TASK_Start(ulTaskId);

	HLIB_JSON_Init (eHLIB_JSON_Conf_Minimum, (void *(*)(unsigned long))HLIB_STD_MemAlloc_CB, (int (*)(void *))HLIB_STD_MemFree_CB);

	APK_META_SVC_Init(FALSE);
	APK_META_SCHEDULE_RegisterNotifier(local_schedule_callback, APK_META_SCHEDULE_GetInstance());
	APK_META_SCHEDULE_Refresh(NULL);
	printf("APK_META_SCHEDULE_Refresh()\n");

	// wait for schedule is refreshed
	for(i = 0; i < 10 ; ++i)
	{
		printf("wait for schedule refresh( %d )\n", i);
		HLIB_STD_TaskSleep(1000);
		if(bScheduleLoaded == TRUE)
		{
			break;
		}
	}

	if(bScheduleLoaded == FALSE || ulNumSch == 0)
	{
		printf("can not load Schedule info or no schedules.\n");
		goto END_FUNC;
	}
	

	// Success to Schedule is loaded
	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if(NULL == pstSchArray)
	{
		printf("pstSchArray is NULL!!\n");
		goto END_FUNC;
	}

	hErr = APK_META_SCHEDULE_GetAllSchedule(ulNumSch, NULL, pstSchArray);
	if(ERR_OK != hErr)
	{
		printf("Fail to APK_META_SCHEDULE_GetAllSchedule - %d\n", hErr);
		goto END_FUNC;
	}

	j_root = HxJSON_Object_New();
	j_schedules = HxJSON_Array_New();
	for(ulCnt = 0 ; ulCnt < ulNumSch ; ulCnt++)
	{
		DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
		switch(pstSchedule->eRsvType)
		{
			case DxRSVTYPE_RECORD_EBR:
			case DxRSVTYPE_RECORD_TBR:
			case DxRSVTYPE_RECORD_INSTANT:
			case DxRSVTYPE_RECORD_DELAYED:
			case DxRSVTYPE_RECORD_SERIES_EBR:
				{
					HxJSON_t j_sch = HxJSON_Object_New();
					UNIXTIME startTime = 0;
					UNIXTIME endTime = 0;

					// Mandatory : SVC Name, Event Name, Start Time, End Time
					{
						DxService_t *svc = APK_META_SVC_GetService(pstSchedule->ulSvcUid);
						if(svc != NULL) {
							HxJSON_Object_Set(j_sch, "svc_name", HxJSON_String_New(svc->name));
							APK_META_SVC_Delete(svc);
						}
						HxJSON_Object_Set(j_sch, "event_id", HxJSON_Integer_New(pstSchedule->uExtInfo.stRec.nEvtId));
						HxJSON_Object_Set(j_sch, "event_name", HxJSON_String_New(pstSchedule->uExtInfo.stRec.szEventName));

						HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime, &startTime);
						HxJSON_Object_Set(j_sch, "start_time", HxJSON_Integer_New(startTime));

						endTime = startTime + pstSchedule->ulDuration;
						HxJSON_Object_Set(j_sch, "end_time", HxJSON_Integer_New(endTime));
					}
					// Optional
					{
						HxJSON_Object_Set(j_sch, "svc_uid", HxJSON_Integer_New(pstSchedule->ulSvcUid));
						HxJSON_Object_Set(j_sch, "type", HxJSON_Integer_New(pstSchedule->eRsvType));
						HxJSON_Object_Set(j_sch, "seriesId", HxJSON_Integer_New(pstSchedule->uExtInfo.stRec.ulSeriesId));
						HxJSON_Object_Set(j_sch, "programId", HxJSON_Integer_New(pstSchedule->uExtInfo.stRec.ulProgramId));

						if(pstSchedule->eScheduleSpec == eDxSchedule_Spec_TvAnyTime) {
							HxJSON_Object_Set(j_sch, "seriesCRID", HxJSON_String_New(pstSchedule->uExtInfo.stRec.ex.tva.szCRID));
							HxJSON_Object_Set(j_sch, "programCRID", HxJSON_String_New(pstSchedule->uExtInfo.stRec.ex.tva.szProgCRID));
						} else if(pstSchedule->eScheduleSpec == eDxSchedule_Spec_Freesat) {
							HxJSON_Object_Set(j_sch, "seriesCRID", HxJSON_String_New(pstSchedule->uExtInfo.stRec.ex.freesat.szCRID));
							HxJSON_Object_Set(j_sch, "programCRID", HxJSON_String_New(pstSchedule->uExtInfo.stRec.ex.freesat.szRecordedProgCrid));
						} else {
							// 

						}

					}


					HLIB_JSON_Array_Append(j_schedules, j_sch);
				}
				break;

			default:
				break;
		}

	}

	HxJSON_Object_Set(j_root, "schedules", j_schedules);

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

	hErr = HLIB_FILE_WriteFile((const HUINT8 *)GET_ALL_SCHEDULE_RESULT_FILE_NAME, (void *)szResultText, ulResultLen);
	if(ERR_OK != hErr)
	{
		fprintf(stderr, "Fail to HLIB_FILE_WriteFile( %s ) - %d\n", GET_ALL_SCHEDULE_RESULT_FILE_NAME, hErr);
		goto END_FUNC;
	}


END_FUNC:
	bRun = FALSE;
	if(pstSchArray)
	{
		HLIB_STD_MemFree(pstSchArray);
	}

	if(j_root != NULL)
	{
		HLIB_JSON_Delete(j_root);
	}

	APK_META_SCHEDULE_UnregisterNotifier(local_schedule_callback, APK_META_SCHEDULE_GetInstance());
	APK_META_SVC_DeInit();

	printf("End of main()\n");

	return 0;
}
