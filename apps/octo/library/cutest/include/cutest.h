#ifndef CU_TEST_H
#define CU_TEST_H

#include <setjmp.h>
#include <stdarg.h>




#define CUTEST_ALLOC(TYPE)		((TYPE*) malloc(sizeof(TYPE)))

#define CUTEST_HUGE_STRING_LEN	8192
#define CUTEST_STRING_MAX		256
#define CUTEST_STRING_INC		256

typedef struct
{
	int length;
	int size;
	char* buffer;
} Cutest_String_t;


extern Cutest_String_t* CUTEST_StringNew(void);
extern void CUTEST_StringDelete(Cutest_String_t* pCuTestString);

/* Cutest_TestCase_t */

typedef struct Cutest_TestCase_t Cutest_TestCase_t;

typedef void (*Cutest_TestFunction_t)(Cutest_TestCase_t *);

struct Cutest_TestCase_t
{
	char* name;
	char* description;
	Cutest_TestFunction_t function;
	int failed;
	int ran;
	Cutest_String_t *pstMessageString;
	Cutest_String_t *pstErrorLocationString;
	jmp_buf *jumpBuf;
	int bUseJump;
};


extern Cutest_TestCase_t* CUTEST_TestNew(const char* name, const char* description, Cutest_TestFunction_t function);
extern void CUTEST_TestRun(Cutest_TestCase_t* pTestCase, int bUseJump);


/* Internal versions of assert functions -- use the public versions */
extern void CUTEST_FailLine(Cutest_TestCase_t* pTestCase, const char* file, int line, const char* message, const char* compMessage);
extern void CUTEST_AssertLine(Cutest_TestCase_t* pTestCase, const char* file, int line, const char* message, int condition);
extern void CUTEST_AssertStrEquals_LineMsg(Cutest_TestCase_t* pTestCase,
	const char* file, int line, const char* message,
	const char* expected, const char* actual);
extern void CUTEST_AssertIntEquals_LineMsg(Cutest_TestCase_t* pTestCase,
	const char* file, int line, const char* message,
	int expected, int actual);
extern void CUTEST_AssertDblEquals_LineMsg(Cutest_TestCase_t* pTestCase,
	const char* file, int line, const char* message,
	double expected, double actual, double delta);
extern void CUTEST_AssertPtrEquals_LineMsg(Cutest_TestCase_t* pTestCase,
	const char* file, int line, const char* message,
	void* expected, void* actual);
extern void CUTEST_AssertMemEquals_LineMsg(Cutest_TestCase_t* pTestCase,
	const char* file, int line, const char* message,
	const void* expected, const void* actual, int size);

/* public assert functions */

#define CutestFail(tc, ms)                        CUTEST_FailLine(  (tc), __FILE__, __LINE__, NULL, (ms))
#define CutestAssert(tc, ms, cond)                CUTEST_AssertLine((tc), __FILE__, __LINE__, (ms), (cond))
#define CutestAssertTrue(tc, cond)                CUTEST_AssertLine((tc), __FILE__, __LINE__, "assert failed", (cond))

#define CutestAssertStrEquals(tc,ex,ac)           CUTEST_AssertStrEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define CutestAssertStrEquals_Msg(tc,ms,ex,ac)    CUTEST_AssertStrEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define CutestAssertIntEquals(tc,ex,ac)           CUTEST_AssertIntEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define CutestAssertIntEquals_Msg(tc,ms,ex,ac)    CUTEST_AssertIntEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define CutestAssertDblEquals(tc,ex,ac,dl)        CUTEST_AssertDblEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac),(dl))
#define CutestAssertDblEquals_Msg(tc,ms,ex,ac,dl) CUTEST_AssertDblEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac),(dl))
#define CutestAssertPtrEquals(tc,ex,ac)           CUTEST_AssertPtrEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define CutestAssertPtrEquals_Msg(tc,ms,ex,ac)    CUTEST_AssertPtrEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define CutestAssertMemEquals(tc,ex,ac,size)           CUTEST_AssertMemEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac), (size))
#define CutestAssertMemEquals_Msg(tc,ms,ex,ac,size)    CUTEST_AssertMemEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac), (size))
#define CutestAssertPtrNotNull(tc,p)        CUTEST_AssertLine((tc),__FILE__,__LINE__,"null pointer unexpected",(p != NULL))
#define CutestAssertPtrNotNull_Msg(tc,msg,p) CUTEST_AssertLine((tc),__FILE__,__LINE__,(msg),(p != NULL))

/* Cutest_Suite_t */

#define CUTEST_MAX_TEST_CASES	1024

#define CUTEST_SUITE_ADD_TEST(SUITE,TEST,DESCRIPTION)	CUTEST_SuiteAdd(SUITE, CUTEST_TestNew(#TEST, DESCRIPTION, TEST))

typedef struct
{
	int count;
	Cutest_TestCase_t* list[CUTEST_MAX_TEST_CASES];
	int failCount;
	int bUseJump;
} Cutest_Suite_t;


extern void CUTEST_SuiteInit(Cutest_Suite_t* pTestSuite);
extern Cutest_Suite_t* CUTEST_SuiteNew(void);
extern void CUTEST_SuiteDelete(Cutest_Suite_t *pTestSuite);
extern void CUTEST_SuiteAdd(Cutest_Suite_t* pTestSuite, Cutest_TestCase_t *pTestCase);
extern void CUTEST_SuiteMerge(Cutest_Suite_t* pTestSuite, Cutest_Suite_t* pTestSuite2);
extern void CUTEST_SuiteRun(Cutest_Suite_t* pTestSuite, int bUseJump);
extern void CUTEST_SuiteSummary(Cutest_Suite_t* pTestSuite, Cutest_String_t* summary);
extern void CUTEST_SuiteDetail(Cutest_Suite_t* pTestSuite, Cutest_String_t* pDetails);

/*-------------------------------------------------------------------------*
 * Reporting data.
 *-------------------------------------------------------------------------*/
extern void CUTEST_MakeTestListToFile(Cutest_Suite_t* pTestSuite, const char* pszFilenamePrefix);
extern void CUTEST_MakeTestResultToFile(Cutest_Suite_t* pTestSuite, const char* pszFilenamePrefix);
#endif /* CU_TEST_H */
