#define __HEAD___________________________________________________________________________

#include <dapi.h>
#include "atkit.h"

#define __DEFINE_________________________________________________________________________

#define MAX_LENGTH	128

#define INT_TEST_KEY_01	"UDB/IntTestKey_01"
#define STR_TEST_KEY_01	"UDB/StrTestKey_01"



#define __STATIC_VARIABLES_______________________________________________________________

static HxJSON_t s_manifest = NULL;


#define __STATIC_FUNCTIONS_______________________________________________________________


#define __TESTCASE_______________________________________________________________________

HINT32 TC_DB_ValidateIntegerData(HxJSON_t j_args)
{
	HINT32 lInTestInteger = 0;
	HINT32 lOutTestInteger = 0;
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;

	printf("TC NAME( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));
	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if (j_argument != NULL)
	{
		lInTestInteger = HLIB_JSON_Object_GetInteger(j_argument, "data_integer");
	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);
		fflush(stdout);
	}

	printf("[%s] TESTING.. : Store Integer ( %d )\n", __FUNCTION__, lInTestInteger);
	hErr = APK_DB_SetInt(INT_TEST_KEY_01, lInTestInteger);
	if (ERR_OK != hErr)
	{
		printf("[%s] TEST FAIL : Fail to APK_DB_SetInt() - Err( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	hErr = APK_DB_GetInt(INT_TEST_KEY_01, &lOutTestInteger);
	if (ERR_OK != hErr)
	{
		printf("[%s] TEST FAIL : Fail to APK_DB_GetInt() - Err( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	if (lInTestInteger != lOutTestInteger)
	{
		printf("[%s] TEST FAIL : Set integer( %d ), Get Integer ( %d )\n", __FUNCTION__, lInTestInteger, lOutTestInteger);
		fflush(stdout);
		return FALSE;
	}

	printf("[%s] TEST SUCCESS : TC_DB_ValidateIntegerData()\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}

HINT32 TC_DB_ValidateStringData(HxJSON_t j_args)
{
	const HCHAR *szInTestString = NULL;
	HCHAR *szOutTestString = (HCHAR *)malloc(MAX_LENGTH);
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;

	printf("TC_NAME( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));
	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if (j_argument != NULL)
	{
		szInTestString = HLIB_JSON_Object_GetString(j_argument, "data_string");
	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);
		fflush(stdout);
	}

	printf("[%s] TESTING.. : Store String ( %s )\n", __FUNCTION__, szInTestString);
	hErr = APK_DB_SetStr(STR_TEST_KEY_01, szInTestString);
	if (ERR_OK != hErr)
	{
		printf("[%s] TEST FAIL : Fail to APK_DB_SetStr() - Err( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	hErr = APK_DB_GetStr(STR_TEST_KEY_01, szOutTestString, MAX_LENGTH);
	if (ERR_OK != hErr)
	{
		printf("[%s] TEST FAIL : Fail to APK_DB_GetStr() - Err( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	if (strncmp(szInTestString, szOutTestString, MAX_LENGTH) != 0)
	{
		printf("[%s] TEST FAIL : Set string ( %s ), Get string ( %s )\n", __FUNCTION__, szInTestString, szOutTestString);
		fflush(stdout);
		return FALSE;
	}

	printf("[%s] TEST SUCCESS : TC_DB_ValidateStringData()\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}

void TC_DB_ValidateDataInit(HxJSON_t jManifest)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);

	s_manifest = jManifest;

	DAPI_Init();
	APK_DB_Init();

	ADD_TC(TC_DB_ValidateIntegerData);
	ADD_TC(TC_DB_ValidateStringData);
}

void TC_DB_ValidateDataDeinit(void)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}
