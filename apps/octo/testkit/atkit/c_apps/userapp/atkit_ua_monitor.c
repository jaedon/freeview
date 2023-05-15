#include <hapi.h>
#include <vkernel.h>
#include <hlib.h>

#define MONITOR_RESULT_FILE_NAME		"/home/atkit/result/atkit_ua_monitor.result"

void	local_monitor_callback(HSystemMointorInfo_t *pInfo)
{
	HERROR			hErr = ERR_FAIL;

	HxJSON_t 		j_root = NULL;
	HxJSON_t 		j_procInfoArray = NULL;
	HINT32			i = 0;
	HCHAR			*szResultText = NULL;
	HUINT32 		ulResultLen = 0;

	if(pInfo == NULL)
	{
		return;
	}

	j_root = HxJSON_Object_New();
	HxJSON_Object_Set(j_root, "cpuRate", HxJSON_Integer_New(pInfo->unCpuRate));
	HxJSON_Object_Set(j_root, "memRate", HxJSON_Integer_New(pInfo->unMemRate));
	HxJSON_Object_Set(j_root, "memFreeSize", HxJSON_Integer_New(pInfo->unMemFreeSizeMB));

	j_procInfoArray = HxJSON_Array_New();
	for(i = 0 ; i < HAMA_MAX_MONITOR_PROCESS_NUM ; i++)
	{
		HxJSON_t j_procInfo = HxJSON_Object_New();
		HxJSON_Object_Set(j_procInfo, "processName", HxJSON_String_New(pInfo->stProcess[i].szProcessName));
		HxJSON_Object_Set(j_procInfo, "usedMemSizeMB", HxJSON_Integer_New(pInfo->stProcess[i].unUsedMemSizeMB));
		HxJSON_Object_Set(j_procInfo, "usedMemRate", HxJSON_Integer_New(pInfo->stProcess[i].unMemRate));
		HxJSON_Object_Set(j_procInfo, "usedCpuRate", HxJSON_Integer_New(pInfo->stProcess[i].unCpuRate));

		HLIB_JSON_Array_Append(j_procInfoArray, j_procInfo);
	}
	HxJSON_Object_Set(j_root, "procInfo", j_procInfoArray);	



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

	hErr = HLIB_FILE_WriteFile((const HUINT8 *)MONITOR_RESULT_FILE_NAME, (void *)szResultText, ulResultLen);
	if(ERR_OK != hErr)
	{
		fprintf(stderr, "Fail to HLIB_FILE_WriteFile( %s ) - %d\n", MONITOR_RESULT_FILE_NAME, hErr);
		goto END_FUNC;
	}

END_FUNC:
	if(j_root != NULL)
	{
		HLIB_JSON_Delete(j_root);
	}

}


int main(int argc, char **argv)
{

	HLIB_JSON_Init (eHLIB_JSON_Conf_Minimum, (void *(*)(unsigned long))HLIB_STD_MemAlloc_CB, (int (*)(void *))HLIB_STD_MemFree_CB);

	VK_Init();
	HAPI_Init();
	HAPI_SysMonitorListener(local_monitor_callback);
	HAPI_SetSysMonitor(TRUE);

	while(1)
	{
		HLIB_STD_TaskSleep(2000);
	}

	printf("End of main()\n");

	return 0;
}

