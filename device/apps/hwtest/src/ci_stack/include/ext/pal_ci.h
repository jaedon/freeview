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

/* CI ���� Ȯ�ο� : CI ���� ���ο����� ����� �� */
//#define	_CI_DEBUG_

/* CI+ �ֿ� ���� Ȯ�ο� : CI ���� ���ο����� ����� �� */
#if 0 // #if defined(CONFIG_DEBUG) && defined(CONFIG_LF7300) // Ziggo �� LF7300 �� �޸ƽ� ���� CI+ ���̴�. CI+ ���� �׽�Ʈ�� �ֿ� ���ɻ��̹Ƿ� LF7300 ������ �⺻ �����Ѵ�.
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

#ifdef _CI_PLUS_DEBUG_		/* CI+ �ֿ� ���� Ȯ�ο� : CI ���� ���ο����� ����� �� */
#define	PrintNotify(x)		VK_Print x
#else
#define	PrintNotify(x)
#endif


// add by chojh
// for robust handling Timeout
// #define _INCLUDE_CI_TRPT_RECOVERY_
// hmkim's note : ���� define �� �ʿ��� ��Ȳ�� �߻��ϴ� ���� ķ ��ü ������ ���� �ְ����� ķ���� �ԷµǴ� TS (��, Ʃ�ʴ�) �� Ŭ�� ������ ���� �ִ�.
// �� define �ڵ�δ� ķ �������� �츮 �������� Ȯ�ε� �ȵǰ� �ٺ����� ó���� �� ���� ������ �� define �� ������ ������� �ʴ� ������ �Ѵ�.
// �� define �ڵ忡 ���� ķ�� ���� ���µǴ� ��Ȳ�� �߻��� �� �ֱ� �����̴�.


/*--- CI+ specific defines ---*/

#ifdef CONFIG_CI_PLUS

/* to support CI+ : CI_ENABLE (OCTO���� CONFIG_CI) �� ���ٿ� CI+ ���� �߰� ������ Ȱ��ȭ��. CI ���� ���ο��� �� define�� ����ϰ� �ܺ� ��⿡�� CONFIG_CI_PLUS �� ����ϵ��� �Ѵ�. */
	#define CI_PLUS_ENABLE

/* ����ϴ� credentials : �Ʒ� define���� ��Ÿ������ Ȱ��ȭ �Ǿ�� �ϰ� ��ǰ ������ �ÿ��� �ƹ� �͵� define �Ǿ� �־�� �ȵ�! */
//	#define CI_PLUS_PRODUCTION_Credentials				/* �׽�Ʈ�� ���� credentials : ���� �׽�Ʈ �뵵�� ����ϰ� ��ǰ ������ �ÿ� define �Ǿ�� �ȵȴ�! */
//	#define CI_PLUS_TEST_Technology_from_TrustCenter	/* LLP test credentials : ���� credentials �Լ� �������� �ѽ������� ����ϰ� ��ǰ ������ �ÿ� define �Ǿ�� �ȵȴ�! */
//	#define CI_PLUS_TEST_Technology_from_SmarDTV		/* SmarDTV test credentials : ���� SmarDTV CI+ �׽�Ʈ Ŷ�� ���ؼ��� ����ϰ� ��ǰ ������ �ÿ� define �Ǿ�� �ȵȴ�! */

#endif


/*--- H/W platform specific defines ---*/

#ifdef CONFIG_DOUGLAS

/* douglas�� read/write memory functions�� �������� �ʰ� CIS/COR ó���� ���� functions���� �����ϱ� ������ �Ʒ� define�� ����ϵ��� �Ѵ� */
	#define USE_DOUGLAS_PRIMITIVE_FUNC
	#ifdef USE_DOUGLAS_PRIMITIVE_FUNC
		extern int DRV_CI_ReadCIS (unsigned short usSlotId, unsigned char *pcis, unsigned short *plen);
		extern int DRV_CI_WriteCOR (unsigned short usSlotId);
	#endif

/* 2009.03.29 WorkAround For Douglas Platform (H/W ECO �ȵ� Douglas �÷���) :
   ���۷��� ������ ķ �ʱ�ȭ�� ���ҽ��� �ö���� �ʴ� ��� (Resource Manager ���Ŀ� Application Information �� ������ �ٸ� ���ҽ��� �ȿö���� ��찡 ����)
   ķ ������ �ɱ� ���� work-around �� */
	#define CI_WORKAROUND_FOR_DOUGLAS
	#ifdef CI_WORKAROUND_FOR_DOUGLAS
		#define CI_WORKAROUND_FOR_DOUGLAS_TIMEOUT	20000
		#define CI_WORKAROUND_FOR_DOUGLAS_LIMIT		3
	#endif

/* CI Read Error ������ ���� CI/Flash �� �������� �߰� */
//	#define	USE_CI_FLASH_SEM

#endif

/* CI Slot Number. */
#define PAL_MAX_CISLOT_NUMBER		2 /* ������ �޷� �ִ� ������ �ϳ��� �� ���� 1�� ���������� ����. CI ��⿡���� �׽� 2������ ó���� �� �ֵ��� ����Ѵ�. see CI_MAX_NUM_OF_CAM */

typedef unsigned short CI_SLOT_ID; /* INVALID_CI_SLOT_ID �� �ʱ�ȭ�Ǿ�� �Ѵ� */

#define INVALID_CI_SLOT_ID	(0xFFFF)

/* MMI : MMI ������ CI�� ���ѽ�Ű�� �ʰ� ����ȭ�� ��ȹ --> pal_mmi.h ���� ������ �̵� */
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
	unsigned char ucBlindOnOff;	/* ����� �Է��� ȭ�� ǥ�� ��� */
	unsigned char ucAnswerLen;		/* ���ǿ� �����ؾ� �� ��Ʈ�� ���� */
	unsigned char *pszPrompt;		/* �����ϴ� ��Ʈ�� */
} MMI_ENQ_PARAM;

typedef struct {
	unsigned char ucNumOfItem;		/* �������� ���� */
	unsigned char *pszTitle;		/* Ÿ��Ʋ ������ ǥ�õ� ��Ʈ�� */
	unsigned char *pszSubTitle;	/* ����Ÿ��Ʋ ������ ǥ�õ� ��Ʈ�� */
	unsigned char *pszBottom;		/* �ϴ� ������ ǥ�õ� ��Ʈ�� */
	unsigned char **ppszItem;		/* �� �������� ��Ʈ�� */
} MMI_MENU_LIST_PARAM;

typedef enum
{
	PAL_CI_EXTRACTED = 0,	/* ķ�� ���ŵ� ��� �Ǵ� ķ ����� power off �� ��� */
	PAL_CI_INSERTED		/* ķ�� ���Ե� ���� */
} PAL_CI_STATE;			/* DI_CI_STATE �� ���ε� */

typedef enum
{
	PAL_CI_BYPASS = 0,
	PAL_CI_PASSTHROUGH
} PAL_CI_PATH;			/* DI_CI_PATH �� ���ε� */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if 1 //disable for HW_TEST
HERROR PAL_CI_Init(void);

unsigned short PAL_CI_GetNumberOfModule (void); /* �����ϴ� CI ���� ���� ������ */

HERROR PAL_CI_RegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag)); /* ķ�� ���¿� ���� CI ����̹��κ��� ȣ��Ǳ⸦ ���ϴ� �ݹ��� ����ϱ� ���� �Լ� */

PAL_CI_STATE PAL_CI_CheckModule (CI_SLOT_ID usSlotId); /* ķ ���� ������ ������ */

HERROR PAL_CI_ResetModule (CI_SLOT_ID usSlotId); /* ķ ����� ������ */

HERROR PAL_CI_SetTsPath (CI_SLOT_ID usSlotId, PAL_CI_PATH path); /* bypass �Ǵ� pass-thru�� �����ϱ� ���� �Լ� */
#ifdef CONFIG_DEBUG
PAL_CI_PATH PAL_CI_GetTsPath (CI_SLOT_ID usSlotId); /* debugging �뵵�θ� ����ϱ� ���� �Լ� */
#endif

#ifdef CONFIG_PVR // for 2 tuner PVR ��
HERROR	PAL_CI_SetPath(HINT32 nSrcId, HINT32 nSlotId, HINT32 nDmxId); /* ���� �������� nSlotId �� -1 ���� ���� �� ������ ���� */
#endif

//HERROR PAL_CI_GetVersion(HUINT8 *ucMajorVer, HUINT8 *ucMinorVer);
#endif //disable for HW_TEST
#endif	//__PAL_CI_H__

