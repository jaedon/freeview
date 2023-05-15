/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application Test Parameters
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_TESTPARAMS_H_
#define _FAI_TESTPARAMS_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_widget.h"
#include "fai_app_config.h"

/* model specific configuration header */
#include "factorysw_config.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define TOTAL_OSD_TEST_PARAM				90 // 1sheet(30) x 3
#define TOTAL_RCU_KEY						50
#define TOTAL_FRONT_KEY						30

#define MAX_TMD_TEST_ITEM					120
#define MAX_TMD_TEST_RESULT					100
#define MAX_TMD_TEST_MULTI_INDEX			32


typedef enum 
{ 
	FAI_TEST_RESULT_NOT_TESTED = 0, 
	FAI_TEST_RESULT_PASS, 
	FAI_TEST_RESULT_FAIL,
	FAI_TEST_RESULT_CONTINUE, 
	FAI_TEST_RESULT_NOT_CHANGED, 
	FAI_TEST_RESULT_ON, 
	FAI_TEST_RESULT_OFF
} FAI_TEST_RESULT;

typedef enum 
{
	TEST_INPUT_TYPE_FKEY = 0, 
	TEST_INPUT_TYPE_RCU
} TEST_INPUT_TYPE;

typedef struct position
{
	int nX;
	int nY;
} POSITION_T;

typedef struct rcu2str
{
	int nRcuKeyCode;
	char *pcRcuStr;
} RCU2STR_T;

/* OSD Test Item Parameter */
typedef struct testparam
{
	TEST_INPUT_TYPE eInputType;
	unsigned int unSheet;
	unsigned int unPosition;
	unsigned int unRcuKey;
	char *pcTestName;
	FAI_ERR_CODE (*pfnTestFunc)(void);
} OSDTESTPARAM_T;

/* TMD Test Item Parameter */
typedef struct tmdtestparam
{
	unsigned char ucCategory;
	unsigned char ucModule;
	unsigned char ucAction;
	BOOL bMulti;
	FAI_ERR_CODE (*pfnTestFunc)(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
} FAI_TMDTESTPARAM_T;

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* Global variables and structures
******************************************************************************/
extern OSDTESTPARAM_T g_atAllTestParamTable[TOTAL_OSD_TEST_PARAM];
extern POSITION_T g_atTestMenuPositionFor5x4[TOTAL_OSD_TEST_PARAM];
extern POSITION_T g_atTestMenuPositionFor4x4[TOTAL_OSD_TEST_PARAM];
extern OSDTESTPARAM_T *g_atExeTestParamTable[TOTAL_SHEET_NUMBER*PP_TOTAL_MENU_NUM];

extern FAI_TMDTESTPARAM_T g_stTmdTestItemTable[MAX_TMD_TEST_ITEM];

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/******************************************************************************
* function : RCUCode2Str
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
char *RCUCode2Str(int nCode);

/******************************************************************************
* function : FKeyCode2Str
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
char *FKeyCode2Str(int nCode);

#endif

