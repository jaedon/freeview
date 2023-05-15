#define __HEAD___________________________________________________________________________

#include <dlib.h>
#include <dapi.h>
#include <vkernel.h>
#include "atkit.h"

#define __DEFINE_________________________________________________________________________

#define TC_DB_VALIDATEDATA_MANIFEST_FILE_NAME "/home/atkit/manifest/tc_appkit_db.manifest"
#define TC_DB_VALIDATEDATA_RESULT_FILE_NAME "/home/atkit/result/tc_appkit_db.result"

#define CATEGORY_NAME_LEN 32
#define TC_NAME_LEN 128

typedef struct _TC_t
{
	HCHAR name[64];
	HBOOL (*func)(HxJSON_t);
} TC_t;

STATIC HxJSON_t		s_manifest = NULL;
STATIC HxVector_t	*s_tcVector = NULL;
STATIC HBOOL		s_bTCRun = FALSE;

STATIC HBOOL local_tc_getCallback(void *data, void *tc_name)
{
	TC_t *tc = (TC_t *)data;
	HBOOL bRet = FALSE;
	if(tc)
	{
		if(strcmp(tc->name, tc_name) == 0)
			bRet = TRUE;
	}

	return bRet;
}

// APK Event Dispatcher
STATIC void local_tc_eventDispatcher(void *arg)
{
	while(s_bTCRun)
	{
		APK_EVENT_Dispatch();
		HLIB_STD_TaskSleep(500);
	}
}

HINT32 local_tc_init(void)
{
	HLONG	ulTaskId = 0;
	s_bTCRun = TRUE;

	VK_Init();
	VK_TASK_Create(local_tc_eventDispatcher, VK_TASK_PRIORITY_MW_REF, SIZE_256K, "atkit_event_dispatcher", NULL, &ulTaskId, 0);
	VK_TASK_Start(ulTaskId);

	printf("TC Init ---------------------------\n");

	TC_DB_ValidateDataInit(s_manifest);

	printf("TC Init ---------------------------\n");

	return 0;
}

void local_tc_deInit(void)
{
	printf("TC DeInit ---------------------------\n");

	TC_DB_ValidateDataDeinit();
	
	printf("TC DeInit ---------------------------\n");

}


HBOOL _ADD_TC(HINT32 (*func)(HxJSON_t), HCHAR *name)
{
	TC_t *pTC = NULL;

	if (NULL == func || NULL == name)
	{
		printf("%s:%d error\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	pTC = (TC_t *)HLIB_STD_MemCalloc(sizeof(TC_t));
	if (NULL == pTC)
	{
		printf("%s:%d error\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	snprintf(pTC->name, sizeof(pTC->name), "%s", name);
	pTC->func = func;

	HLIB_VECTOR_Add(s_tcVector, pTC);

	printf("TC( %s ) added!\n", name);

	return TRUE;
}

HBOOL _CALL_TC(const HCHAR *name, HxJSON_t j_args)
{
	HBOOL bRet = FALSE;
	TC_t *tc = HLIB_VECTOR_Find(s_tcVector, local_tc_getCallback, (void *)name);

	if (tc)
	{
		bRet = tc->func(j_args);
	}
	else
	{
		printf("TC( %s ) is not exist!\n", name);
	}

	return bRet;
}

int main(int argc, char **argv)
{
	HINT32		nTcCount = 0;
	HINT32		nSuccessCount = 0;
	HINT32		nFailCount = 0;
	HxJSON_t	j_tcArray = NULL;
	HxJSON_t	j_tc = NULL;
	HxJSON_t	j_resultRoot = NULL;
	HxJSON_t	j_resultArray = NULL;
	HxJSON_t	j_resultSummary = NULL;
	HxJSON_t	j_result = NULL;
	HBOOL		bRet = FALSE;

	HCHAR		szTcName[TC_NAME_LEN] = {0, };
	HERROR		hErr = ERR_FAIL;
	HCHAR		*szResultText = NULL;
	HUINT32		ulResultLen = 0;

	HBOOL		bRunTC = FALSE;
	HBOOL		bTcRunAll = TRUE;
	HxJSON_t	j_categoryArray = NULL;
	HxJSON_t	j_category = NULL;
	HCHAR		szCategoryName[CATEGORY_NAME_LEN] = {0, };

	HxJSON_t	j_myCategoryArray = NULL;
	HxJSON_t	j_myCategory = NULL;
	HCHAR		szMyCategoryName[CATEGORY_NAME_LEN] = {0, };

	HxJSON_t	j_eachTcArray = NULL;
	HxJSON_t	j_eachTc = NULL;
	HCHAR		szEachTcName[TC_NAME_LEN] = {0, };


	HINT32		nRet = 0;

	printf("#####################################################################################\n");
	printf("Unit Test for Appkit Database validation!\n");
	printf("#####################################################################################\n");

	HLIB_JSON_Init (eHLIB_JSON_Conf_Minimum, (void *(*)(unsigned long))HLIB_STD_MemAlloc_CB, (int (*)(void *))HLIB_STD_MemFree_CB);

	s_tcVector = HLIB_VECTOR_New(NULL, NULL);
	s_manifest = HLIB_JSON_DecodeFile(TC_DB_VALIDATEDATA_MANIFEST_FILE_NAME);

	// check TC Run mode : all, category, each tc
	bTcRunAll = HLIB_JSON_Object_GetBoolean(s_manifest, "RUN_ALL");
	if(bTcRunAll == FALSE)
	{
		j_categoryArray = HLIB_JSON_Object_GetArray(s_manifest, "RUN_CATEGORY");
		if(NULL == j_categoryArray || (HLIB_JSON_GetSize(j_categoryArray) == 0))
		{
			printf("[%s:%d] j_categoryArray is NULL!\n", __FUNCTION__, __LINE__);
			j_eachTcArray = HLIB_JSON_Object_GetArray(s_manifest, "RUN_TC");
			if(NULL == j_eachTcArray || (HLIB_JSON_GetSize(j_eachTcArray) == 0))
			{
				printf("[%s:%d] j_categoryArray is NULL!\n", __FUNCTION__, __LINE__);
				printf("[%s:%d] Manifest file is something wrong!\n", __FUNCTION__, __LINE__);
				nRet = -1;
				goto END_TC;
			}
		}
	}


	j_tcArray = HLIB_JSON_Object_GetArray(s_manifest, "TC_LIST");
	if(NULL == j_tcArray)
	{
		printf("[%s:%d] j_tcArray is NULL!\n", __FUNCTION__, __LINE__);
		nRet = -1;
		goto END_TC;
	}

	local_tc_init();	

	j_resultRoot = HxJSON_Object_New();
	j_resultArray = HxJSON_Array_New();
	HxJSON_ARRAY_EACH(j_tcArray, j_tc, {
		bRunTC = FALSE;

		HxSTD_MemSet(szTcName, 0x00, TC_NAME_LEN);
		HxSTD_snprintf(szTcName, TC_NAME_LEN, "%s", HLIB_JSON_Object_GetString(j_tc, "tc_name"));

		if(bTcRunAll == TRUE)
		{ // top priority : check RUN_ALL
			bRunTC = TRUE;
		}
		else
		{
		
			if(j_categoryArray != NULL && (HLIB_JSON_GetSize(j_categoryArray) > 0))
			{ // second priority : check RUN_CATEGORY
				j_myCategoryArray = HLIB_JSON_Object_GetArray(j_tc, "category");
				if(j_myCategoryArray == NULL)
				{ // if the TC don't have "category" field then continue
					continue;
				}

				HxJSON_ARRAY_EACH(j_categoryArray, j_category, {
					HxSTD_MemSet(szCategoryName, 0x00, CATEGORY_NAME_LEN);
					HxSTD_snprintf(szCategoryName, CATEGORY_NAME_LEN, "%s", HxJSON_String_Get(j_category));

					HxJSON_ARRAY_EACH(j_myCategoryArray, j_myCategory, {
						HxSTD_MemSet(szMyCategoryName, 0x00, CATEGORY_NAME_LEN);
						HxSTD_snprintf(szMyCategoryName, CATEGORY_NAME_LEN, "%s", HxJSON_String_Get(j_myCategory));

						if(HxSTD_StrNCmp(szCategoryName, szMyCategoryName, CATEGORY_NAME_LEN) == 0)
						{ // matched category
							bRunTC = TRUE;
							break;
						}
					});

					if(bRunTC == TRUE)
					{
						break;
					}
				});
			}
			else
			{ // else check RUN_TC
				if(j_eachTcArray != NULL && (HLIB_JSON_GetSize(j_eachTcArray) > 0))
				{
					HxJSON_ARRAY_EACH(j_eachTcArray, j_eachTc, {
						HxSTD_MemSet(szEachTcName, 0x00, TC_NAME_LEN);
						HxSTD_snprintf(szEachTcName, TC_NAME_LEN, "%s", HxJSON_String_Get(j_eachTc));

						if(HxSTD_StrNCmp(szEachTcName, szTcName, TC_NAME_LEN) == 0)
						{ // matched TC
							bRunTC = TRUE;
							break;
						}
					});
				}
			}
		}

		if(bRunTC)
		{ // RUN TC
			nTcCount++;
			j_result = HxJSON_Object_New();
			printf("TC_Name : %s\n", szTcName);
			
			HxJSON_Object_Set(j_result, "tc_name", HxJSON_String_New(szTcName));
			bRet = CALL_TC(szTcName, j_tc);
			if( bRet )
			{
				nSuccessCount++;
				printf("[%s] SUCCESS \n", szTcName);
				HxJSON_Object_Set(j_result, "result", HxJSON_String_New("success"));
			}
			else
			{
				nFailCount++;
				printf("[%s] FAIL \n", szTcName);
				HxJSON_Object_Set(j_result, "result", HxJSON_String_New("fail"));
			}
			
			HLIB_JSON_Array_Append(j_resultArray, j_result);
		}
		
	});

	// Make Summary
	j_resultSummary = HxJSON_Object_New();
	HxJSON_Object_Set(j_resultSummary, "total", HxJSON_Integer_New(nTcCount));
	HxJSON_Object_Set(j_resultSummary, "success", HxJSON_Integer_New(nSuccessCount));
	HxJSON_Object_Set(j_resultSummary, "fail", HxJSON_Integer_New(nFailCount));


	HxJSON_Object_Set(j_resultRoot, "summary", j_resultSummary);
	HxJSON_Object_Set(j_resultRoot, "result", j_resultArray);
	
	// write j_resultRoot to 'result.json' file
	szResultText = HLIB_JSON_Encode(j_resultRoot, NULL, 0);
	if(NULL == szResultText)
	{
		fprintf(stderr, "szResultText is NULL!!\n");
		goto END_TC;
	}

	ulResultLen = HxSTD_StrLen(szResultText);
	if(0 == ulResultLen)
	{
		fprintf(stderr, "Result text size is 0!\n");
		goto END_TC;
	}

	hErr = HLIB_FILE_WriteFile((const HUINT8 *)TC_DB_VALIDATEDATA_RESULT_FILE_NAME, (void *)szResultText, ulResultLen);
	if(ERR_OK != hErr)
	{
		fprintf(stderr, "Fail to HLIB_FILE_WriteFile( %s ) - %d\n", TC_DB_VALIDATEDATA_RESULT_FILE_NAME, hErr);
		goto END_TC;
	}


END_TC:
	s_bTCRun = FALSE;
	
	if(j_resultRoot != NULL)
	{
		HLIB_JSON_Delete(j_resultRoot);
	}

	if(s_manifest != NULL)
	{
		HLIB_JSON_Delete(s_manifest);
	}

	printf("#####################################################################################\n");
	printf("END TC( %d )\n", nRet);
	printf("#####################################################################################\n");

	return nRet;
}
