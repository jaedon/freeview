#include "tc_manifest_manager.h"

static char *ConvertLongToString(unsigned long lVal)
{
    char *buff = NULL;
    int n = 0, c = 0;
    n = snprintf(NULL, 0, "%lu", lVal);
    if (n < 0)
    {
		printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "snprintf() to NULL");
        exit(EXIT_FAILURE);
    }
    
    buff = malloc(n+1);
    c = snprintf(buff, n+1, "%lu", lVal);
    if (buff[n] != '\0' || c != n)
    {
		printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "snprintf() to Buffer");
        exit(EXIT_FAILURE);
    }

    return buff;
}

static char *ConvertBoolToString(bool bVal)
{
    if (true == bVal)
    {
        return "true";
    }
    else if (false == bVal)
    {
        return "false";
    }
    else
    {
		printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "Invalid boolean value");
        exit(EXIT_FAILURE);
    }
}

static long ConvertStrToLong(char *str)
{
    char *endptr;
    long lVal;

    lVal = strtol(str, &endptr, 10);

    if ((errno == ERANGE && (lVal == LONG_MAX || lVal == LONG_MIN))
        || (errno != 0 && lVal == 0))
    {
		printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (endptr == str)
    {
		printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "No digits were found");
        exit(EXIT_FAILURE);
    }

    return lVal;
}

static bool ConvertStrToBool(char *str)
{
    if (strncmp(str, "true", 5) == 0)
    {
        return true;
    }
    else if (strncmp(str, "false", 6) == 0)
    {
        return false;
    }
    else
    {
		printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "Invalid string");
        exit(EXIT_FAILURE);
    }
}

void ManifestExporter(const char *pathName, stTc_manifest_t *manifest)
{
    HxJSON_t j_root= NULL;
    HxJSON_t j_tcArray = NULL;
    HxJSON_t j_tc = NULL;
    HxJSON_t j_param = NULL;
    HxJSON_t j_categoryArray = NULL;
    HxJSON_t j_category = NULL;
    HxJSON_t j_runCategoryArray = NULL;
    HxJSON_t j_runCategory = NULL;
    HxJSON_t j_result = NULL;

    HUINT32 nIndex, j;
    HUINT32 ulResultLen;
    HCHAR *szResultText = NULL;;

	HLIB_JSON_Init(eHLIB_JSON_Conf_Minimum, (void *(*)(unsigned long))HLIB_STD_MemAlloc_CB, (int (*)(void *))HLIB_STD_MemFree_CB);

    j_root = HxJSON_Object_New();
    HxJSON_Object_Set(j_root, "RUN_ALL", HxJSON_Boolean_New(manifest->bRunAll));

    j_runCategoryArray = HxJSON_Array_New();
    for (nIndex = 0; nIndex < manifest->nRunCategoryCnt; nIndex++)
    {
        j_runCategory = HxJSON_String_New(manifest->szRunCategory[nIndex]);
        HLIB_JSON_Array_Append(j_runCategoryArray, j_runCategory);
    }
    HxJSON_Object_Set(j_root, "RUN_CATEGORY", j_runCategoryArray);
    
    HxJSON_Object_Set(j_root, "RUN_COUNT", HxJSON_Integer_New(manifest->nRunTcCount));
    
    HxJSON_Object_Set(j_root, "NUMBER_OF_TC", HxJSON_Integer_New(manifest->nNumbTC));
    
    j_tcArray = HxJSON_Array_New();
    for (nIndex = 0; nIndex < manifest->nNumbTC; nIndex++)
    {
        j_tc = HxJSON_Object_New();
        
        HxJSON_Object_Set(j_tc, "tc_name", HxJSON_String_New(manifest->pTC[nIndex].szTcDesc)); 
        HxJSON_Object_Set(j_tc, "tc_runCount", HxJSON_Integer_New(manifest->pTC[nIndex].nRunCount));

        j_categoryArray = HxJSON_Array_New();
        for (j = 0; j < manifest->pTC[nIndex].nCategoryCnt; j++)
        {
            j_category = HxJSON_String_New(manifest->pTC[nIndex].szCategory[j]);
            HLIB_JSON_Array_Append(j_categoryArray, j_category);
        }
        HxJSON_Object_Set(j_tc, "category", j_categoryArray);

        HxJSON_Object_Set(j_tc, "number_of_param", HxJSON_Integer_New(manifest->pTC[nIndex].nNumbParam));

        j_param = HxJSON_Object_New();
        for (j = 0; j < manifest->pTC[nIndex].nNumbParam; j++)
        {
            if (manifest->pTC[nIndex].pParam[j].eDataType == STRING)
            {
                HxJSON_Object_Set(j_param, manifest->pTC[nIndex].pParam[j].szArgName, HxJSON_String_New(manifest->pTC[nIndex].pParam[j].szVal)); 
            }           
            else if (manifest->pTC[nIndex].pParam[j].eDataType == INTEGER)
            {
                HxJSON_Object_Set(j_param, manifest->pTC[nIndex].pParam[j].szArgName, HxJSON_Integer_New((HINT32)ConvertStrToLong(manifest->pTC[nIndex].pParam[j].szVal)));
            }
            else if (manifest->pTC[nIndex].pParam[j].eDataType == BOOLEAN)
            {
                HxJSON_Object_Set(j_param, manifest->pTC[nIndex].pParam[j].szArgName, HxJSON_Boolean_New(ConvertStrToBool(manifest->pTC[nIndex].pParam[j].szVal)));
            }
            else
            {
                printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "Invalid parameter type");
                exit(EXIT_FAILURE);
            }
        }
        HxJSON_Object_Set(j_tc, "argument", j_param);
        
        j_result = HxJSON_Object_New();
        if (manifest->pTC[nIndex].eExpectedResult == STRING)
        {
            HxJSON_Object_Set(j_result, manifest->pTC[nIndex].szResultDesc, HxJSON_String_New(manifest->pTC[nIndex].szResultValue)); 
        }           
        else if (manifest->pTC[nIndex].eExpectedResult == INTEGER)
        {
            HxJSON_Object_Set(j_result, manifest->pTC[nIndex].szResultDesc, HxJSON_Integer_New(ConvertStrToLong(manifest->pTC[nIndex].szResultValue)));
        }
        else if (manifest->pTC[nIndex].eExpectedResult == BOOLEAN)
        {
            HxJSON_Object_Set(j_result, manifest->pTC[nIndex].szResultDesc, HxJSON_Boolean_New(ConvertStrToBool(manifest->pTC[nIndex].szResultValue)));
        }
        else
        {
            printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "Invalid parameter type");
            exit(EXIT_FAILURE);
        }
        HxJSON_Object_Set(j_tc, "expected_result", j_result);
        
        HLIB_JSON_Array_Append(j_tcArray, j_tc);
    }

    HxJSON_Object_Set(j_root, "TC_LIST", j_tcArray);

    szResultText = HLIB_JSON_Encode(j_root, NULL, 0);
    if (NULL == szResultText)
    {
        printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "szResultText is NULL!!");
        exit(EXIT_FAILURE);
    }

    ulResultLen = HxSTD_StrLen(szResultText);
    if (0 == ulResultLen)
    {
        printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "Result text size is 0!");
        exit(EXIT_FAILURE);
    }

    if (0 != HLIB_FILE_WriteFile((const unsigned char *)pathName, (void *)szResultText, ulResultLen))
    {
        printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "HLIB_FILE_WriteFile()");
        exit(EXIT_FAILURE);
    }
}

void ManifestImporter(const char *pathName, stTc_manifest_t *manifest)
{
    /** memset(manifest, 0, sizeof(stTc_manifest_t)); */
    /** manifest->pTC->pParam = NULL; */
    /** manifest->pTC = NULL; */


    HxJSON_t j_root = NULL;
    HxJSON_t j_categoryArray = NULL;
    HxJSON_t j_category = NULL;
    HxJSON_t j_tcArray = NULL;
    HxJSON_t j_tc = NULL;
    HxJSON_t j_tcCategoryArray = NULL;
    HxJSON_t j_tcCategory = NULL;
    HxJSON_t j_argument = NULL;
    HxJSON_t j_result = NULL;

    HUINT32 i = 0, j = 0;
    
    if (NULL == manifest)
    {
        exit(EXIT_FAILURE);
    }
    
    HLIB_JSON_Init(eHLIB_JSON_Conf_Minimum, (void *(*)(unsigned long))HLIB_STD_MemAlloc_CB, (int (*)(void *))HLIB_STD_MemFree_CB);

    j_root = HLIB_JSON_DecodeFile(pathName);

    manifest->bRunAll = HLIB_JSON_Object_GetBoolean(j_root, "RUN_ALL");
    printf("%s-%s\n", "RUN_ALL", ConvertBoolToString(manifest->bRunAll));
	j_categoryArray = HLIB_JSON_Object_GetArray(j_root, "RUN_CATEGORY");

    manifest->nRunCategoryCnt = HLIB_JSON_GetSize(j_categoryArray);
    if (0 != manifest->nRunCategoryCnt)
    {
        HxJSON_ARRAY_EACH(j_categoryArray, j_category, {
            HxSTD_snprintf(manifest->szRunCategory[i], 64 /* MACRO here */, "%s", HxJSON_String_Get(j_category));            
            printf("%s-%s\n", "RUN_CATEGORY", manifest->szRunCategory[i]);
            i = i + 1;
                });
    }

    manifest->nRunTcCount = HLIB_JSON_Object_GetInteger(j_root, "RUN_COUNT");
    printf("%s-%d\n", "RUN_COUNT", manifest->nRunTcCount);
    manifest->nNumbTC = HLIB_JSON_Object_GetInteger(j_root, "NUMBER_OF_TC");
    printf("%s-%d\n", "NUMBER_OF_TC", manifest->nNumbTC);
    
    j_tcArray = HLIB_JSON_Object_GetArray(j_root, "TC_LIST");
    
    i = 0;
    if (0 != manifest->nNumbTC)
    {
        HxJSON_ARRAY_EACH(j_tcArray, j_tc, {
            HxSTD_snprintf(manifest->pTC[i].szTcDesc, 256, "%s", HLIB_JSON_Object_GetString(j_tc, "tc_name"));
            printf("%s-%s\n", "tc_name", manifest->pTC[i].szTcDesc);
            manifest->pTC[i].nRunCount = HLIB_JSON_Object_GetInteger(j_tc, "tc_runCount");
            printf("%s-%d\n", "tc_runCount", manifest->pTC[i].nRunCount);
            
            j_tcCategoryArray = HLIB_JSON_Object_GetArray(j_tc, "category");
            manifest->pTC[i].nCategoryCnt = HLIB_JSON_GetSize(j_tcCategoryArray);
            if (0 != manifest->pTC[i].nCategoryCnt)
            {
                j = 0;
                HxJSON_ARRAY_EACH(j_tcCategoryArray, j_tcCategory, {
                    HxSTD_snprintf(manifest->pTC[i].szCategory[j], 64, "%s", HxJSON_String_Get(j_tcCategory));                        
                    printf("%s-%s\n", "tc_category", manifest->pTC[i].szCategory[j]);
                    j = j + 1;
                        });
            }
            
            manifest->pTC[i].nNumbParam = HLIB_JSON_Object_GetInteger(j_tc, "number_of_param");
            if (0 != manifest->pTC[i].nNumbParam)
            {
                j_argument = HLIB_JSON_Object_GetObject(j_tc, "argument");
                for (j = 0; j < manifest->pTC[i].nNumbParam; j++) 
                {
                    if (manifest->pTC[i].pParam[j].eDataType == STRING)
                    {
                        strncpy(manifest->pTC[i].pParam[j].szVal,  HLIB_JSON_Object_GetString(j_argument, manifest->pTC[i].pParam[j].szArgName), 256);
                    }
                    else if (manifest->pTC[i].pParam[j].eDataType == INTEGER)
                    {
                        strncpy(manifest->pTC[i].pParam[j].szVal,  ConvertLongToString(HLIB_JSON_Object_GetInteger(j_argument, manifest->pTC[i].pParam[j].szArgName)), 256);
                    }
                    else if (manifest->pTC[i].pParam[j].eDataType == BOOLEAN)
                    {
                        strncpy(manifest->pTC[i].pParam[j].szVal,  ConvertBoolToString(HLIB_JSON_Object_GetBoolean(j_argument, manifest->pTC[i].pParam[j].szArgName)), 256);
                    }
                    else
                    {
                        printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "Invalid parameter type");
                        exit(EXIT_FAILURE);
                    }
                    printf("%s-%s\n", "Param value", manifest->pTC[i].pParam[j].szVal);
                }
            }

            j_result = HLIB_JSON_Object_GetObject(j_tc, "expected_result");
            
            if (manifest->pTC[i].eExpectedResult == STRING)
            {
                strncpy(manifest->pTC[i].szResultValue, HLIB_JSON_Object_GetString(j_result, manifest->pTC[i].szResultDesc), 256);
            }
            else if (manifest->pTC[i].eExpectedResult == INTEGER)
            {
                strncpy(manifest->pTC[i].szResultValue, ConvertLongToString(HLIB_JSON_Object_GetInteger(j_result, manifest->pTC[i].szResultDesc)), 256);
            }
            else if (manifest->pTC[i].eExpectedResult == BOOLEAN)
            {
                strncpy(manifest->pTC[i].szResultValue, ConvertBoolToString(HLIB_JSON_Object_GetBoolean(j_result, manifest->pTC[i].szResultDesc)), 256);
            }
            else
            {
                printf("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, "Invalid parameter type");
                exit(EXIT_FAILURE);
            }
            printf("%s-%s\n", "Result value", manifest->pTC[i].szResultValue);

            i = i + 1;
                });
    }
}

HBOOL IsManifestExist(const char *pathName)
{
   return HLIB_FILE_Exist(pathName); 
}
