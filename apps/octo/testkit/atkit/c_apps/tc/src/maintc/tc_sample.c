#include "atkit.h"

static HxJSON_t s_manifest = NULL;

HBOOL TC_SAMPLE_CalcPlus(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_expectedResult = NULL;

	HINT32 nArg1, nArg2 = 0;
	HINT32 nEResult = 0;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	nArg1 = HLIB_JSON_Object_GetInteger(j_argument, "number1");
	nArg2 = HLIB_JSON_Object_GetInteger(j_argument, "number2");

	j_expectedResult = HLIB_JSON_Object_GetObject(j_args, "expect_result");
	nEResult = HLIB_JSON_Object_GetInteger(j_expectedResult, "result");

	printf("( %d ) + ( %d ) = %d\n", nArg1, nArg2, nEResult);
	fflush(stdout);

	return (nArg1 + nArg2 == nEResult) ? TRUE : FALSE;
}

HBOOL TC_SAMPLE_CalcMinus(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_expectedResult = NULL;

	HINT32 nArg1, nArg2 = 0;
	HINT32 nEResult = 0;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	nArg1 = HLIB_JSON_Object_GetInteger(j_argument, "number1");
	nArg2 = HLIB_JSON_Object_GetInteger(j_argument, "number2");

	j_expectedResult = HLIB_JSON_Object_GetObject(j_args, "expect_result");
	nEResult = HLIB_JSON_Object_GetInteger(j_expectedResult, "result");

	printf("( %d ) - ( %d ) = %d\n", nArg1, nArg2, nEResult);
	fflush(stdout);

	return (nArg1 - nArg2 == nEResult) ? TRUE : FALSE;
}

HBOOL TC_SAMPLE_CalcMultiply(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_expectedResult = NULL;

	HINT32 nArg1, nArg2 = 0;
	HINT32 nEResult = 0;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	nArg1 = HLIB_JSON_Object_GetInteger(j_argument, "number1");
	nArg2 = HLIB_JSON_Object_GetInteger(j_argument, "number2");

	j_expectedResult = HLIB_JSON_Object_GetObject(j_args, "expect_result");
	nEResult = HLIB_JSON_Object_GetInteger(j_expectedResult, "result");

	printf("( %d ) * ( %d ) = %d\n", nArg1, nArg2, nEResult);
	fflush(stdout);

	return (nArg1 * nArg2 == nEResult) ? TRUE : FALSE;
}

HBOOL TC_SAMPLE_CalcDivide(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_expectedResult = NULL;

	HINT32 nArg1, nArg2 = 0;
	HINT32 nEResult = 0;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	nArg1 = HLIB_JSON_Object_GetInteger(j_argument, "number1");
	nArg2 = HLIB_JSON_Object_GetInteger(j_argument, "number2");

	j_expectedResult = HLIB_JSON_Object_GetObject(j_args, "expect_result");
	nEResult = HLIB_JSON_Object_GetInteger(j_expectedResult, "result");

	printf("( %d ) / ( %d ) = %d\n", nArg1, nArg2, nEResult);
	fflush(stdout);

	return (nArg1 / nArg2 == nEResult) ? TRUE : FALSE;
}

// There is only expected_result
HBOOL TC_SAMPLE_GetMagicString(HxJSON_t j_args)
{
	HxJSON_t j_expectedResult = NULL;
	const HCHAR *pResultString = NULL;
	const HCHAR *strMyMagic = "Linear SVC";

	j_expectedResult = HLIB_JSON_Object_GetObject(j_args, "expect_result");
	pResultString = HLIB_JSON_Object_GetString(j_expectedResult, "magic_string");

	printf("My Magic String( %s ) - Expected Result( %s )\n", strMyMagic, pResultString);

	return HLIB_STD_StrCaseCmp(strMyMagic, pResultString) == 0 ? TRUE : FALSE;
}

HBOOL TC_SAMPLE_DummyTest(HxJSON_t j_args)
{
	printf("[%s]\n", __FUNCTION__);
	return TRUE;
}

void TC_SAMPLE_Init(HxJSON_t jManifest)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);

	s_manifest = jManifest;

	ADD_TC(TC_SAMPLE_CalcPlus);
	ADD_TC(TC_SAMPLE_CalcMinus);
	ADD_TC(TC_SAMPLE_CalcMultiply);
	ADD_TC(TC_SAMPLE_CalcDivide);
	ADD_TC(TC_SAMPLE_GetMagicString);
	ADD_TC(TC_SAMPLE_DummyTest);
}

void TC_SAMPLE_DeInit(void)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}

