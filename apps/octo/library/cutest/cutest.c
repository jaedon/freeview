#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "./include/cutest.h"

/*-------------------------------------------------------------------------*
 * internal functions
 *-------------------------------------------------------------------------*/


static char* cutest_StrAlloc(int size)
{
	char* newStr = (char*) malloc( sizeof(char) * (size) );
	return newStr;
}

static char* cutest_StrCopy(const char* pSrcStr)
{
	int len = strlen(pSrcStr);
	char* newStr = cutest_StrAlloc(len + 1);
	strcpy(newStr, pSrcStr);
	return newStr;
}

static void cutest_StrInit(Cutest_String_t* pStr)
{
	pStr->length = 0;
	pStr->size = CUTEST_STRING_MAX;
	pStr->buffer = (char*) malloc(sizeof(char) * pStr->size);
	pStr->buffer[0] = '\0';
}




static void cutest_StrResize(Cutest_String_t* pStr, int newSize)
{
	pStr->buffer = (char*) realloc(pStr->buffer, sizeof(char) * newSize);
	pStr->size = newSize;
}

static void cutest_StrAppend(Cutest_String_t* pStr, const char* text)
{
	int length;

	if (text == NULL) {
		text = "NULL";
	}

	length = strlen(text);
	if (pStr->length + length + 1 >= pStr->size)
		cutest_StrResize(pStr, pStr->length + length + 1 + CUTEST_STRING_INC);
	pStr->length += length;
	strcat(pStr->buffer, text);
}

static void cutest_StrAppendChar(Cutest_String_t* pStr, char ch)
{
	char text[2];
	text[0] = ch;
	text[1] = '\0';
	cutest_StrAppend(pStr, text);
}

static void cutest_StrAppendFormat(Cutest_String_t* pStr, const char* format, ...)
{
	va_list argp;
	char buf[CUTEST_HUGE_STRING_LEN];
	va_start(argp, format);
	vsprintf(buf, format, argp);
	va_end(argp);
	cutest_StrAppend(pStr, buf);
}

static void cutest_StrInsert(Cutest_String_t* pStr, const char* text, int pos)
{
	int length = strlen(text);
	if (pos > pStr->length)
	{
		pos = pStr->length;
	}
	if (pStr->length + length + 1 >= pStr->size)
	{
		cutest_StrResize(pStr, pStr->length + length + 1 + CUTEST_STRING_INC);
	}
	memmove(pStr->buffer + pos + length, pStr->buffer + pos, (pStr->length - pos) + 1);
	pStr->length += length;
	memcpy(pStr->buffer + pos, text, length);
}

static void cutest_Fail(Cutest_TestCase_t* pTestCase, const char* pFileNameStr, int line, Cutest_String_t* pString)
{

	char buf[CUTEST_STRING_MAX];


	pTestCase->failed = 1;
	cutest_StrAppend(pTestCase->pstMessageString, (char*)"\n");
	cutest_StrAppend(pTestCase->pstMessageString, pString->buffer);

	if(pFileNameStr)
	{
		sprintf(buf, "%s:%d: ", pFileNameStr, line);

		cutest_StrAppend(pTestCase->pstErrorLocationString, (char*)"\n");
		cutest_StrAppend(pTestCase->pstErrorLocationString, buf);
	}

	if (pTestCase->jumpBuf != 0 && pTestCase->bUseJump == 1)
	{
		longjmp(*(pTestCase->jumpBuf), 0);
	}

}

static void cutest_TestInit(Cutest_TestCase_t* pTestCase, const char* pName, const char* pDescription, Cutest_TestFunction_t function)
{
	pTestCase->name = cutest_StrCopy(pName);
	pTestCase->description = cutest_StrCopy(pDescription);
	pTestCase->failed = 0;
	pTestCase->ran = 0;
	pTestCase->function = function;
	pTestCase->pstMessageString = CUTEST_StringNew();
	cutest_StrInit(pTestCase->pstMessageString);
	pTestCase->pstErrorLocationString = CUTEST_StringNew();
	cutest_StrInit(pTestCase->pstErrorLocationString);
	pTestCase->jumpBuf = NULL;
}

static void cutest_TestDelete(Cutest_TestCase_t *pTestCase)
{
        if (pTestCase)
	{
		if(pTestCase->name)
		{
			free(pTestCase->name);
		}

		if(pTestCase->description)
		{
			free(pTestCase->description);
		}

		if(pTestCase->pstMessageString)
		{
			CUTEST_StringDelete(pTestCase->pstMessageString);
		}

		if(pTestCase->pstErrorLocationString)
		{
			CUTEST_StringDelete(pTestCase->pstErrorLocationString);
		}

		free(pTestCase);

        }

	return;


}

#define	CUTEST_STR_BUFF_SIZE			128
#define	CUTEST_BUFFER_SIZE		1024
#define	CUTEST_DUMP_NUMBER_IN_LINE		16
#define	CUTEST_DUMP_NUMBER_IN_PAGE		0x200

static void cutest_MemDump(char *pucBuffer, int unSize, int unBaseAddr, int bFlagDetail, char *pszPrintBuf)
{
	int		 unChar, unCount, unNumOfDump;
	int		 ullAddr = 0;
	char		 aucChar[CUTEST_DUMP_NUMBER_IN_LINE + 1];
	int			 i;
	char		 szBuff[CUTEST_STR_BUFF_SIZE];
	char		*pszLine;

	memset(aucChar, 0, CUTEST_DUMP_NUMBER_IN_LINE + 1);

	i = 0;
	unNumOfDump = 0;

	ullAddr += unBaseAddr;

	pszLine = pszPrintBuf;

	while (unNumOfDump < unSize)
	{
		unChar = 0x000000ff & (*pucBuffer++);

		if ((unChar >= 0x20) && (unChar <= 0x7e))
			aucChar[i++] = (char)unChar;
		else
			aucChar[i++] = '.';

		if ((ullAddr % CUTEST_DUMP_NUMBER_IN_PAGE) == 0)
		{
			if (bFlagDetail)
			{
				strncat((char *)pszLine, "\n-----------------------------------------------\n", CUTEST_STR_BUFF_SIZE);
			}
			else
			{
				strncat((char *)pszLine, (const char *)"\n"
							"\t\t\t00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n"
							"\t\t\t-----------------------------------------------\n"
							"\t\t\t |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |",
							200
						);
			}
		}

		if ((ullAddr % CUTEST_DUMP_NUMBER_IN_LINE) == 0)
		{
			if (bFlagDetail)
			{
				snprintf((char *)szBuff, CUTEST_STR_BUFF_SIZE, (const char *)"\n");
			}
			else
			{
				snprintf((char *)szBuff, CUTEST_STR_BUFF_SIZE, (const char *)"\n\t%8x : \t", ullAddr);
			}
			strncat((char *)pszLine, szBuff, CUTEST_STR_BUFF_SIZE);
		}

		snprintf((char *)szBuff, CUTEST_STR_BUFF_SIZE, (const char *)"%02x ", unChar);
		strncat((char *)pszLine, szBuff, CUTEST_STR_BUFF_SIZE);

		if ((ullAddr % CUTEST_DUMP_NUMBER_IN_LINE) == (CUTEST_DUMP_NUMBER_IN_LINE - 1))
		{
			snprintf((char *)szBuff, CUTEST_STR_BUFF_SIZE, (const char *)"\t%s", aucChar);
			strncat((char *)pszLine, szBuff, CUTEST_STR_BUFF_SIZE);
			i = 0;
		}


		ullAddr++;
		unNumOfDump++;
	}

	unCount = ullAddr % CUTEST_DUMP_NUMBER_IN_LINE;

	aucChar[i] = 0;
	if (unCount != 0)
	{
		for (i = unCount; i < CUTEST_DUMP_NUMBER_IN_LINE; ++i)
		{
			strncat((char *)pszLine, "   ", 10);
		}
	}

	snprintf((char *)szBuff, CUTEST_STR_BUFF_SIZE, (const char *)"\t%s", aucChar);
	strncat((char *)pszLine, szBuff, CUTEST_STR_BUFF_SIZE);
}


#define CUTEST_MAX_FILENAME_LENGTH		1025


static void cutest_MakeTestList(Cutest_Suite_t* pSuite, const char* szFilename)
{
	Cutest_TestCase_t* pTest = NULL;
	FILE* pTestListFile = NULL;
	char* szTime;
	time_t tTime = 0;
	int i = 0;


	if (NULL == pSuite)
	{
		// TODO: error.
		return ;
	}
	else if ((NULL == szFilename) || (0 == strlen(szFilename)))
	{
		// TODO: error.
		return ;
	}
	else if (NULL == (pTestListFile = fopen(szFilename, "w")))
	{
		// TODO: error.
		return ;
	}



	setvbuf(pTestListFile, NULL, _IONBF, 0);

	fprintf(pTestListFile,
		"<?xml version=\"1.0\" ?> \n"
		"<?xml-stylesheet type=\"text/xsl\" href=\"cutest_list.xsl\" ?> \n"
		"<!DOCTYPE CUTEST_LIST_REPORT SYSTEM \"cutest_list.dtd\"> \n"
		"<CUTEST_LIST_REPORT> \n"
		"  <CUTEST_HEADER/> \n"
		"  <CUTEST_LIST_SUMMARY> \n");

	fprintf(pTestListFile,
		"    <CUTEST_LIST_SUMMARY_RECORD> \n"
		"      <CUTEST_LIST_SUMMARY_RECORD_TEXT> %s </CUTEST_LIST_SUMMARY_RECORD_TEXT> \n"
		"      <CUTEST_LIST_SUMMARY_RECORD_VALUE> %u </CUTEST_LIST_SUMMARY_RECORD_VALUE> \n"
		"    </CUTEST_LIST_SUMMARY_RECORD> \n"
		"  </CUTEST_LIST_SUMMARY> \n",
		("Total Number of Test Cases"),
		pSuite->count);

	fprintf(pTestListFile,
		"  <CUTEST_LIST_ALL_TEST_LIST> \n");

	for(i == 0; i < pSuite->count; i ++)
	{
		pTest = pSuite->list[i];

		fprintf(pTestListFile,
			" 	   <CUTEST_LIST_TEST_CASE_DEFINITION> \n"
			" 		 <CUTEST_LIST_TEST_CASE_NAME> %s </CUTEST_LIST_TEST_CASE_NAME> \n"
			" 		 <CUTEST_LIST_TEST_ACTIVE_VALUE> %s </CUTEST_LIST_TEST_ACTIVE_VALUE> \n"
			" 		 <CUTEST_LIST_TEST_DESCRIPTION>%s\n"
			"        </CUTEST_LIST_TEST_DESCRIPTION> \n"
			" 	   </CUTEST_LIST_TEST_CASE_DEFINITION> \n",
			pTest->name,
			(NULL != pTest->function) ? ("Yes") : ("No"),
			pTest->description);


	}


	fprintf(pTestListFile, "  </CUTEST_LIST_ALL_TEST_LIST> \n");

	time(&tTime);
	szTime = ctime(&tTime);
	fprintf(pTestListFile,
		"  <CUTEST_FOOTER> File Generated at %s </CUTEST_FOOTER> \n"
		"</CUTEST_LIST_REPORT>",
		(NULL != szTime) ? szTime : "");


	if (0 != fclose(pTestListFile))
	{
		// TODO: error.
		return ;
	}


	return ;
}

static void cutest_MakeTestResult(Cutest_Suite_t* pSuite, const char* szFilename)
{
	Cutest_TestCase_t* pTest = NULL;
	FILE* pTestResultFile = NULL;
	char* szTime;
	time_t tTime = 0;
	int i = 0;


	if (NULL == pSuite)
	{
		// TODO: error.
		return ;
	}
	else if ((NULL == szFilename) || (0 == strlen(szFilename)))
	{
		// TODO: error.
		return ;
	}
	else if (NULL == (pTestResultFile = fopen(szFilename, "w")))
	{
		// TODO: error.
		return ;
	}


	setvbuf(pTestResultFile, NULL, _IONBF, 0);

	fprintf(pTestResultFile,
	        "<?xml version=\"1.0\" ?> \n"
	        "<?xml-stylesheet type=\"text/xsl\" href=\"cutest_run.xsl\" ?> \n"
	        "<!DOCTYPE CUTEST_RUN_REPORT SYSTEM \"cutest_run.dtd\"> \n"
	        "<CUTEST_RUN_REPORT> \n"
	        "  <CUTEST_HEADER/> \n");

	fprintf(pTestResultFile,"  <CUTEST_RESULT_ALL_RESULT_LIST> \n");

	for(i == 0; i < pSuite->count; i ++)
	{
		pTest = pSuite->list[i];


		if(pTest->failed == 1)
		{
			fprintf(pTestResultFile,
					"		 <CUTEST_RESULT_RUN_TEST_RECORD> \n"
					"		   <CUTEST_RESULT_RUN_TEST_FAIL> \n"
					"			 <CUTEST_RESULT_RUN_TEST_NAME> %s </CUTEST_RESULT_RUN_TEST_NAME> \n"
					"			 <CUTEST_RESULT_RUN_ERROR_LOCATION>%s \n"
					"            </CUTEST_RESULT_RUN_ERROR_LOCATION> \n"
					"			 <CUTEST_RESULT_RUN_CONDITION>%s \n"
					"            </CUTEST_RESULT_RUN_CONDITION> \n"
					"		   </CUTEST_RESULT_RUN_TEST_FAIL> \n"
					"		 </CUTEST_RESULT_RUN_TEST_RECORD> \n",
					pTest->name,
					(NULL != pTest->pstErrorLocationString->buffer) ? pTest->pstErrorLocationString->buffer : "",
					(NULL != pTest->pstMessageString->buffer) ? pTest->pstMessageString->buffer : ""
					);

		}
		else
		{
			fprintf(pTestResultFile,
					"		 <CUTEST_RESULT_RUN_TEST_RECORD> \n"
					"		   <CUTEST_RESULT_RUN_TEST_SUCCESS> \n"
					"			 <CUTEST_RESULT_RUN_TEST_NAME> %s </CUTEST_RESULT_RUN_TEST_NAME> \n"
					"		   </CUTEST_RESULT_RUN_TEST_SUCCESS> \n"
					"		 </CUTEST_RESULT_RUN_TEST_RECORD> \n",
					pTest->name);

		}


	}

	fprintf(pTestResultFile,
			"  </CUTEST_RESULT_ALL_RESULT_LIST>\n");

	fprintf(pTestResultFile,
			"  <CUTEST_RESULT_RUN_SUMMARY> \n");

	fprintf(pTestResultFile,
			"	 <CUTEST_RESULT_RUN_SUMMARY_RECORD> \n"
			"	   <CUTEST_RESULT_RUN_SUMMARY_TYPE> %s </CUTEST_RESULT_RUN_SUMMARY_TYPE> \n"
			"	   <CUTEST_RESULT_RUN_SUMMARY_RUN> %u </CUTEST_RESULT_RUN_SUMMARY_RUN> \n"
			"	   <CUTEST_RESULT_RUN_SUMMARY_SUCCEEDED> %u </CUTEST_RESULT_RUN_SUMMARY_SUCCEEDED> \n"
			"	   <CUTEST_RESULT_RUN_SUMMARY_FAILED> %u </CUTEST_RESULT_RUN_SUMMARY_FAILED> \n"
			"	 </CUTEST_RESULT_RUN_SUMMARY_RECORD> \n",
			("Test Cases"),
			pSuite->count,
			pSuite->count - pSuite->failCount,
			pSuite->failCount
			);

	fprintf(pTestResultFile,
			"  </CUTEST_RESULT_RUN_SUMMARY> \n");


	time(&tTime);
	szTime = ctime(&tTime);
	fprintf(pTestResultFile,
		"  <CUTEST_FOOTER> File Generated at %s </CUTEST_FOOTER> \n"
		"</CUTEST_RUN_REPORT>",
		(NULL != szTime) ? szTime : "");

	if (0 != fclose(pTestResultFile))
	{
		// TODO: error.
		return ;
	}


	return ;

}

/*-------------------------------------------------------------------------*
 * public functions
 *-------------------------------------------------------------------------*/

Cutest_String_t* CUTEST_StringNew(void)
{
	Cutest_String_t* pCuTestString = (Cutest_String_t*) malloc(sizeof(Cutest_String_t));
	pCuTestString->length = 0;
	pCuTestString->size = CUTEST_STRING_MAX;
	pCuTestString->buffer = (char*) malloc(sizeof(char) * pCuTestString->size);
	pCuTestString->buffer[0] = '\0';
	return pCuTestString;
}

void CUTEST_StringDelete(Cutest_String_t *pCuTestString)
{
        if (pCuTestString)
	{
		if(pCuTestString->buffer)
		{
			free(pCuTestString->buffer);
		}
		free(pCuTestString);
        }

        return;
}



Cutest_TestCase_t* CUTEST_TestNew(const char* name, const char* description, Cutest_TestFunction_t function)
{
	Cutest_TestCase_t* pTestCase = CUTEST_ALLOC(Cutest_TestCase_t);
	cutest_TestInit(pTestCase, name, description, function);
	return pTestCase;
}



void CUTEST_TestRun(Cutest_TestCase_t* pTestCase, int bUseJump)
{
	jmp_buf buf;
	pTestCase->jumpBuf = &buf;

	pTestCase->bUseJump = bUseJump;

	if (setjmp(buf) == 0)
	{

		if(pTestCase->function && pTestCase)
		{
			pTestCase->ran = 1;

			(pTestCase->function)(pTestCase);
		}

	}
	pTestCase->jumpBuf = 0;

}



void CUTEST_FailLine(Cutest_TestCase_t* pTestCase, const char* file, int line, const char* message, const char* compMessage)
{
	Cutest_String_t stCuString;

	cutest_StrInit(&stCuString);
	if (message != NULL)
	{
		cutest_StrAppend(&stCuString, message);
		cutest_StrAppend(&stCuString, ": ");
	}
	cutest_StrAppend(&stCuString, compMessage);

	cutest_Fail(pTestCase, file, line, &stCuString);


}

void CUTEST_AssertLine(Cutest_TestCase_t* pTestCase, const char* file, int line, const char* message, int condition)
{
	if (condition)
	{
		return;
	}
	CUTEST_FailLine(pTestCase, file, line, NULL, message);
}

void CUTEST_AssertStrEquals_LineMsg(Cutest_TestCase_t* pTestCase, const char* file, int line, const char* message,
	const char* expected, const char* actual)
{
	Cutest_String_t stCuString;
	if ((expected == NULL && actual == NULL) ||
	    (expected != NULL && actual != NULL &&
	     strcmp(expected, actual) == 0))
	{
		return;
	}

	cutest_StrInit(&stCuString);
	if (message != NULL)
	{
		cutest_StrAppend(&stCuString, message);
		cutest_StrAppend(&stCuString, ": ");
	}
	cutest_StrAppend(&stCuString, "expected \"");
	cutest_StrAppend(&stCuString, expected);
	cutest_StrAppend(&stCuString, "\" but was \"");
	cutest_StrAppend(&stCuString, actual);
	cutest_StrAppend(&stCuString, "\"");

	cutest_Fail(pTestCase, file, line, &stCuString);



}

void CUTEST_AssertIntEquals_LineMsg(Cutest_TestCase_t* pTestCase, const char* file, int line, const char* message,
	int expected, int actual)
{
	char buf[CUTEST_STRING_MAX];
	if (expected == actual)
	{
		return;
	}
	sprintf(buf, "expected \"%d\" but was \"%d\"", expected, actual);
	CUTEST_FailLine(pTestCase, file, line, message, buf);
}

void CUTEST_AssertDblEquals_LineMsg(Cutest_TestCase_t* pTestCase, const char* file, int line, const char* message,
	double expected, double actual, double delta)
{
	char buf[CUTEST_STRING_MAX];
	if (fabs(expected - actual) <= delta) return;
	sprintf(buf, "expected \"%f\" but was \"%f\"", expected, actual);

	CUTEST_FailLine(pTestCase, file, line, message, buf);
}

void CUTEST_AssertPtrEquals_LineMsg(Cutest_TestCase_t* pTestCase, const char* file, int line, const char* message,
	void* expected, void* actual)
{
	char buf[CUTEST_STRING_MAX];
	if (expected == actual) return;
	sprintf(buf, "expected pointer \"0x%p\" but was \"0x%p\"", expected, actual);
	CUTEST_FailLine(pTestCase, file, line, message, buf);
}


void CUTEST_AssertMemEquals_LineMsg(Cutest_TestCase_t* pTestCase, const char* file, int line, const char* message,
	const void* expected, const void* actual, int size)
{
	Cutest_String_t stCuString;
	char	szPrintBuf1[CUTEST_BUFFER_SIZE] = {0, };
	char	szPrintBuf2[CUTEST_BUFFER_SIZE] = {0, };

	if (expected == NULL || actual == NULL ||
	    (expected != NULL && actual != NULL &&
	     memcmp(expected, actual, size) == 0))
	{
		return;
	}

	cutest_StrInit(&stCuString);
	if (message != NULL)
	{
		cutest_StrAppend(&stCuString, message);
		cutest_StrAppend(&stCuString, ": ");
	}
	cutest_StrAppend(&stCuString, "expected \" \n");
	cutest_MemDump((char*)expected, size, 0, 0, szPrintBuf1);
	cutest_StrAppend(&stCuString, szPrintBuf1);

	cutest_StrAppend(&stCuString, "\n\" but was \"");
	cutest_MemDump((char*)actual, size, 0, 0, szPrintBuf2);
	cutest_StrAppend(&stCuString, szPrintBuf2);
	cutest_StrAppend(&stCuString, "\n\"");

	cutest_Fail(pTestCase, file, line, &stCuString);


}




void CUTEST_SuiteInit(Cutest_Suite_t* pTestSuite)
{
	pTestSuite->count = 0;
	pTestSuite->failCount = 0;
        memset(pTestSuite->list, 0, sizeof(pTestSuite->list));
}

Cutest_Suite_t* CUTEST_SuiteNew(void)
{
	Cutest_Suite_t* pTestSuite = CUTEST_ALLOC(Cutest_Suite_t);
	CUTEST_SuiteInit(pTestSuite);
	return pTestSuite;
}

void CUTEST_SuiteDelete(Cutest_Suite_t *pTestSuite)
{
        unsigned int n;
        for (n=0; n < CUTEST_MAX_TEST_CASES; n++)
        {
                if (pTestSuite->list[n])
                {
                        cutest_TestDelete(pTestSuite->list[n]);
                }
        }
        free(pTestSuite);

}

void CUTEST_SuiteAdd(Cutest_Suite_t* pTestSuite, Cutest_TestCase_t *pTestCase)
{
	assert(pTestSuite->count < CUTEST_MAX_TEST_CASES);
	pTestSuite->list[pTestSuite->count] = pTestCase;
	pTestSuite->count++;
}

void CUTEST_SuiteMerge(Cutest_Suite_t* pTestSuite, Cutest_Suite_t* pTestSuite2)
{
	int i;
	for (i = 0 ; i < pTestSuite2->count ; ++i)
	{
		Cutest_TestCase_t* pTestCase = pTestSuite2->list[i];
		CUTEST_SuiteAdd(pTestSuite, pTestCase);
	}
}

void CUTEST_SuiteRun(Cutest_Suite_t* pTestSuite, int bUseJump)
{
	int i;

	for (i = 0 ; i < pTestSuite->count ; ++i)
	{
		Cutest_TestCase_t* pTestCase = pTestSuite->list[i];

		CUTEST_TestRun(pTestCase, bUseJump);

		if (pTestCase->failed)
		{

			pTestSuite->failCount += 1;
		}
	}
}

void CUTEST_SuiteSummary(Cutest_Suite_t* pTestSuite, Cutest_String_t* summary)
{
	int i;
	for (i = 0 ; i < pTestSuite->count ; ++i)
	{
		Cutest_TestCase_t* pTestCase = pTestSuite->list[i];
		cutest_StrAppend(summary, pTestCase->failed ? "F" : ".");
	}
	cutest_StrAppend(summary, "\n\n");
}

void CUTEST_SuiteDetail(Cutest_Suite_t* pTestSuite, Cutest_String_t* pDetails)
{
	int i;
	int failCount = 0;

	if (pTestSuite->failCount == 0)
	{
		int passCount = pTestSuite->count - pTestSuite->failCount;
		const char* testWord = passCount == 1 ? "test" : "tests";
		cutest_StrAppendFormat(pDetails, "OK (%d %s)\n", passCount, testWord);
	}
	else
	{
		if (pTestSuite->failCount == 1)
			cutest_StrAppend(pDetails, "There was 1 failure:\n");
		else
			cutest_StrAppendFormat(pDetails, "There were %d failures:\n", pTestSuite->failCount);

		for (i = 0 ; i < pTestSuite->count ; ++i)
		{
			Cutest_TestCase_t* pTestCase = pTestSuite->list[i];
			if (pTestCase->failed)
			{
				failCount++;
				cutest_StrAppendFormat(pDetails, "%d) %s: %s\n",
					failCount, pTestCase->name, pTestCase->pstMessageString->buffer);
			}
		}
		cutest_StrAppend(pDetails, "\n!!!FAILURES!!!\n");

		cutest_StrAppendFormat(pDetails, "Runs: %d ",   pTestSuite->count);
		cutest_StrAppendFormat(pDetails, "Passes: %d ", pTestSuite->count - pTestSuite->failCount);
		cutest_StrAppendFormat(pDetails, "Fails: %d\n",  pTestSuite->failCount);
	}
}











void CUTEST_MakeTestListToFile(Cutest_Suite_t* pTestSuite, const char* pszFilenamePrefix)
{
	const char* pszListFilePostfix = "_cutestList.xml";
	char 	 szCutestTestListFileName[CUTEST_MAX_FILENAME_LENGTH] = "";

	if (NULL == pszFilenamePrefix)
	{
		return;
	}

	strncpy(szCutestTestListFileName, pszFilenamePrefix, CUTEST_MAX_FILENAME_LENGTH - strlen(pszListFilePostfix) - 1);


	szCutestTestListFileName[CUTEST_MAX_FILENAME_LENGTH - strlen(pszListFilePostfix) - 1] = '\0';
	strcat(szCutestTestListFileName, pszListFilePostfix);

	cutest_MakeTestList(pTestSuite, szCutestTestListFileName);

	return ;
}


void CUTEST_MakeTestResultToFile(Cutest_Suite_t* pTestSuite, const char* pszFilenamePrefix)
{
	const char* pszResultFilePostfix = "_cutestResults.xml";
	char 	 szCutestTestResultFileName[CUTEST_MAX_FILENAME_LENGTH] = "";

	if (NULL == pszFilenamePrefix)
	{
		return;
	}

	strncpy(szCutestTestResultFileName, pszFilenamePrefix, CUTEST_MAX_FILENAME_LENGTH - strlen(pszResultFilePostfix) - 1);

	szCutestTestResultFileName[CUTEST_MAX_FILENAME_LENGTH - strlen(pszResultFilePostfix) - 1] = '\0';
	strcat(szCutestTestResultFileName, pszResultFilePostfix);

	cutest_MakeTestResult(pTestSuite, szCutestTestResultFileName);

	return ;
}

