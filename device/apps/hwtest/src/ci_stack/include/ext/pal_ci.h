/**
	@file	  pal_ci.h
	@brief	  																					\n

	Description: 																				\n
	Module: PAL/CI				 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.															\n
	All rights reserved.																		\n
*/


#ifndef	__PAL_CI_H__
#define	__PAL_CI_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "ci_hwtest.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
/*--- debugging defines ---*/

/* CI 동작 확인용 : CI 스택 내부에서만 사용할 것 */
//#define	_CI_DEBUG_

/* CI+ 주요 동작 확인용 : CI 스택 내부에서만 사용할 것 */
#if 0 // #if defined(CONFIG_DEBUG) && defined(CONFIG_LF7300) // Ziggo 향 LF7300 은 휴맥스 최초 CI+ 모델이다. CI+ 동작 테스트가 주요 관심사이므로 LF7300 에서는 기본 설정한다.
#define	_CI_PLUS_DEBUG_
#else
//#define	_CI_PLUS_DEBUG_
#endif

#ifdef _CI_DEBUG_
#define	PrintDbg(x)		VK_Print x
#define	PrintErr(x)		VK_Print x
#else
#define	PrintDbg(x)
#define	PrintErr(x)		VK_Print x
#endif

#ifdef _CI_PLUS_DEBUG_		/* CI+ 주요 동작 확인용 : CI 스택 내부에서만 사용할 것 */
#define	PrintNotify(x)		VK_Print x
#else
#define	PrintNotify(x)
#endif


// add by chojh
// for robust handling Timeout
// #define _INCLUDE_CI_TRPT_RECOVERY_
// hmkim's note : 위의 define 이 필요한 상황이 발생하는 것은 캠 자체 문제일 수도 있겠지만 캠으로 입력되는 TS (즉, 튜너단) 나 클럭 문제일 수도 있다.
// 이 define 코드로는 캠 문제인지 우리 문제인지 확인도 안되고 근본적인 처방일 수 없기 때문에 이 define 은 앞으로 사용하지 않는 것으로 한다.
// 이 define 코드에 의해 캠이 무한 리셋되는 상황이 발생할 수 있기 때문이다.


/*--- CI+ specific defines ---*/

#ifdef CONFIG_CI_PLUS

/* to support CI+ : CI_ENABLE (OCTO에선 CONFIG_CI) 에 덧붙여 CI+ 스펙 추가 사항을 활성화함. CI 스택 내부에선 이 define을 사용하고 외부 모듈에선 CONFIG_CI_PLUS 를 사용하도록 한다. */
	#define CI_PLUS_ENABLE

/* 사용하는 credentials : 아래 define들은 배타적으로 활성화 되어야 하고 제품 릴리즈 시에는 아무 것도 define 되어 있어서는 안됨! */
//	#define CI_PLUS_PRODUCTION_Credentials				/* 테스트용 정식 credentials : 오직 테스트 용도만 사용하고 제품 릴리즈 시에 define 되어서는 안된다! */
//	#define CI_PLUS_TEST_Technology_from_TrustCenter	/* LLP test credentials : 정식 credentials 입수 전까지만 한시적으로 사용하고 제품 릴리즈 시에 define 되어서는 안된다! */
//	#define CI_PLUS_TEST_Technology_from_SmarDTV		/* SmarDTV test credentials : 오직 SmarDTV CI+ 테스트 킷을 위해서만 사용하고 제품 릴리즈 시에 define 되어서는 안된다! */

#endif


/*--- H/W platform specific defines ---*/

#ifdef CONFIG_DOUGLAS

/* douglas가 read/write memory functions을 지원하지 않고 CIS/COR 처리를 별도 functions으로 지원하기 때문에 아래 define을 사용하도록 한다 */
	#define USE_DOUGLAS_PRIMITIVE_FUNC
	#ifdef USE_DOUGLAS_PRIMITIVE_FUNC
		extern int DRV_CI_ReadCIS (unsigned short usSlotId, unsigned char *pcis, unsigned short *plen);
		extern int DRV_CI_WriteCOR (unsigned short usSlotId);
	#endif

/* 2009.03.29 WorkAround For Douglas Platform (H/W ECO 안된 Douglas 플랫폼) :
   더글러스 문제로 캠 초기화시 리소스가 올라오지 않는 경우 (Resource Manager 이후에 Application Information 를 포함한 다른 리소스가 안올라오는 경우가 있음)
   캠 리셋을 걸기 위한 work-around 임 */
	#define CI_WORKAROUND_FOR_DOUGLAS
	#ifdef CI_WORKAROUND_FOR_DOUGLAS
		#define CI_WORKAROUND_FOR_DOUGLAS_TIMEOUT	20000
		#define CI_WORKAROUND_FOR_DOUGLAS_LIMIT		3
	#endif

/* CI Read Error 방지를 위한 CI/Flash 간 세마포어 추가 */
//	#define	USE_CI_FLASH_SEM

#endif

/* CI Slot Number. */
#define PAL_MAX_CISLOT_NUMBER		2 /* 실제로 달려 있는 슬롯이 하나라도 이 값을 1로 변경하지는 말기. CI 모듈에서는 항시 2개까지 처리할 수 있도록 고려한다. see CI_MAX_NUM_OF_CAM */

typedef unsigned short CI_SLOT_ID; /* INVALID_CI_SLOT_ID 로 초기화되어야 한다 */

#define INVALID_CI_SLOT_ID	(0xFFFF)

/* MMI : MMI 관련은 CI에 국한시키지 않고 공용화할 계획 --> pal_mmi.h 같은 곳으로 이동 */
typedef unsigned long MMI_HANDLE; /* handle for MMI_INSTANCE */

typedef struct {
	unsigned short	usSsNo;
} MMI_INSTANCE;

typedef enum {
	MMI_OPEN = 0,
	MMI_CLOSE,
	MMI_TEXT,
	MMI_ENQ,
	MMI_MENU,
	MMI_LIST,
	MMI_ERROR
} MMI_REQ;

typedef struct {
	unsigned char ucBlindOnOff;	/* 사용자 입력의 화면 표시 방식 */
	unsigned char ucAnswerLen;		/* 질의에 응답해야 할 스트링 길이 */
	unsigned char *pszPrompt;		/* 질의하는 스트링 */
} MMI_ENQ_PARAM;

typedef struct {
	unsigned char ucNumOfItem;		/* 아이템의 개수 */
	unsigned char *pszTitle;		/* 타이틀 영역에 표시될 스트링 */
	unsigned char *pszSubTitle;	/* 서브타이틀 영역에 표시될 스트링 */
	unsigned char *pszBottom;		/* 하단 영역에 표시된 스트링 */
	unsigned char **ppszItem;		/* 각 아이템의 스트링 */
} MMI_MENU_LIST_PARAM;

typedef enum
{
	PAL_CI_EXTRACTED = 0,	/* 캠이 제거된 경우 또는 캠 모듈을 power off 한 경우 */
	PAL_CI_INSERTED		/* 캠이 삽입된 직후 */
} PAL_CI_STATE;			/* DI_CI_STATE 와 매핑됨 */

typedef enum
{
	PAL_CI_BYPASS = 0,
	PAL_CI_PASSTHROUGH
} PAL_CI_PATH;			/* DI_CI_PATH 와 매핑됨 */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if 1 //disable for HW_TEST
HERROR PAL_CI_Init(void);

unsigned short PAL_CI_GetNumberOfModule (void); /* 지원하는 CI 슬롯 수를 리턴함 */

HERROR PAL_CI_RegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag)); /* 캠의 상태에 따라 CI 드라이버로부터 호출되기를 원하는 콜백을 등록하기 위한 함수 */

PAL_CI_STATE PAL_CI_CheckModule (CI_SLOT_ID usSlotId); /* 캠 삽입 유무를 리턴함 */

HERROR PAL_CI_ResetModule (CI_SLOT_ID usSlotId); /* 캠 모듈을 리셋함 */

HERROR PAL_CI_SetTsPath (CI_SLOT_ID usSlotId, PAL_CI_PATH path); /* bypass 또는 pass-thru를 지정하기 위한 함수 */
#ifdef CONFIG_DEBUG
PAL_CI_PATH PAL_CI_GetTsPath (CI_SLOT_ID usSlotId); /* debugging 용도로만 사용하기 위한 함수 */
#endif

#ifdef CONFIG_PVR // for 2 tuner PVR 모델
HERROR	PAL_CI_SetPath(HINT32 nSrcId, HINT32 nSlotId, HINT32 nDmxId); /* 현재 구현으로 nSlotId 은 -1 값을 취할 수 있음에 유의 */
#endif

//HERROR PAL_CI_GetVersion(HUINT8 *ucMajorVer, HUINT8 *ucMinorVer);
#endif //disable for HW_TEST
#endif	//__PAL_CI_H__

